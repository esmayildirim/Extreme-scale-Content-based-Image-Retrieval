/*
 ============================================================================
 Name        : svs_reader.cpp
 Author      : ESMA YILDIRIM
 Version     : 2
 Copyright   : Your copyright notice
 Description : Parallel SVS File Reader
 ============================================================================
 */
#define __STDC_FORMAT_MACROS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <glib.h>
#include <dirent.h>
#include <getopt.h>
#include <opencv2/opencv.hpp>
#include "mpi.h"
#include "openslide.h"
#include "dataspaces.h"

#include "cbir.h"
#include "../cbir_common/mpi_utility.h"
#include "../cbir_common/mpi_sort_fs_results.h"
#include "../clustering/tileCBIR_cluster_omp.h"
#include "../final_fine_searching/final_fs.h"
#include "../write_image/write_image.h"

#define PATH_SIZE 200
#define MEMORY_SIZE 104857600

using namespace cv;

static void write_as_ppm(const char *, int64_t, int64_t, uint32_t *, uint8_t, uint8_t, uint8_t);
static char ** get_file_list(const char *, int *);


/* Reads a folder and returns a list of file names */
static char ** get_file_list(const char *path, int *no_of_files) {
    
    *no_of_files = 0;
    DIR* directory;
    struct dirent *dir;
    struct stat path_stat;
    stat(path, &path_stat);
    char **file_list;
    
    if(!S_ISDIR(path_stat.st_mode))
    {
        *no_of_files = 1;
    }
    else{
        directory = opendir(path);
        
        while ((dir = readdir(directory)) != NULL)
        {
            if(strcmp(dir->d_name,".")==0 ||strcmp(dir->d_name,"..") ==0)
                continue;
            (*no_of_files)++;
        }
        closedir(directory);
    }
    
    file_list = (char **)malloc(*no_of_files * sizeof(char *));
    int i;
    
    for (i=0; i< *no_of_files; i++)
        file_list[i] = (char *)calloc(PATH_SIZE, sizeof(char));
    
    directory = opendir(path);
    i = 0;
    while ((dir = readdir(directory)) != NULL)
    {
        
        if(strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0 || strncmp(".", dir->d_name, 1) == 0)
            continue;
        //file_list[i] = g_strdup_printf("%s/%s", path, dir->d_name);
        sprintf(file_list[i], "%s/%s", path, dir->d_name);
        //printf("%s\n",file_list[i]);
        i++;
    }
    closedir(directory);
    //sorting the file list
    
    
    return file_list;
    
}

/*reads the number of levels in each file given a file list parameter
 returns the number of levels for each file*/

static int32_t * get_level_number_list(char ** file_list, int no_of_files)
{
    int i;
    int32_t *level_numbers = (int32_t*)malloc(no_of_files * sizeof(int));
    for (i=0; i< no_of_files; i++) {
        openslide_t *osr = openslide_open(file_list[i]);
        int32_t levels = openslide_get_level_count(osr);
        level_numbers[i] = levels;
        openslide_close(osr);
        printf("Level numbers: %d\n",level_numbers[i]);
    }
    return level_numbers;
    
}

//reads the dimension numbers in every level of every file given a file list and level numbers
//returns a 3D array with dimensions file_index, level_index, dimension_index
static int64_t*** get_dimensions(char ** file_list, int no_of_files, int *level_numbers)
{
    int64_t *** size_width_height = (int64_t ***)malloc(no_of_files * sizeof(int64_t **));
    int i,j;
    for(i=0; i<no_of_files; i++)
    {
        size_width_height[i] = (int64_t **)malloc(level_numbers[i]*sizeof(int64_t *));
        for(j=0; j< level_numbers[i]; j++)
            size_width_height[i][j] = (int64_t *)malloc (2* sizeof(int64_t));
    }
    for(i=0; i<no_of_files; i++)
    {
        openslide_t *osr = openslide_open(file_list[i]);
        for(j=0; j< level_numbers[i]; j++)
        {    openslide_get_level_dimensions(osr, j, &size_width_height[i][j][0], &size_width_height[i][j][1]);
            printf("SIZES %lld x %lld\n",size_width_height[i][j][0],size_width_height[i][j][1]);
        }
        openslide_close(osr);
    }
    return size_width_height;
}

//calculates the number of tiles for every file and level given a dimension list for every file, level, tile width and tile height
//returns the tile number for each image
static int64_t ** get_tile_numbers(int64_t ***size_width_height, int no_of_files, int *level_numbers, int64_t tile_width, int tile_height )
{
    int64_t **tile_numbers = (int64_t **)malloc(no_of_files *sizeof(int64_t*));
    int i,j;
    for (i=0; i<no_of_files; i++) {
        tile_numbers[i] = (int64_t *)malloc(level_numbers[i]* sizeof(int64_t));
        for(j=0; j< level_numbers[i]; j++)
        {
            double t_row, t_col;
            t_row = ceil(size_width_height[i][j][1]/(tile_height *1.0));
            t_col = ceil(size_width_height[i][j][0]/(tile_height *1.0));
            tile_numbers[i][j] = t_row * t_col;
            //printf("Tile numbers file %d,level %d: %lld\n", i,j,tile_numbers[i][j]);
        }
    }
    return tile_numbers;
}

//calculates the total number of tiles to be read/written based on the given level number or all levels(level = -1)
//returns the total number of tiles
static int64_t get_total_tile_number(int64_t ** tile_numbers,int no_of_files, int * level_numbers, int level)
{
    int total_tile_number = 0;
    int i, j;
    for(i=0; i<no_of_files; i++)
    {
        if(level == -1)
            for(j = 0; j<level_numbers[i]; j++)
                total_tile_number += tile_numbers[i][j];
        else
            total_tile_number += tile_numbers[i][level];
    }
    return total_tile_number;
}

//given processor rank, number of processors, decides the global tile range and returns it as the parameters start and end
static void get_tile_range(int64_t total_tile_number, int rank, int p, int64_t *start, int64_t *end)
{
    int64_t tile_per_p = total_tile_number/p;
    int64_t remainder = total_tile_number % p;
    
    if(remainder > 0)
    {
        if(rank <= remainder -1 )
        {
            tile_per_p +=1;
            *start = rank * tile_per_p;
            *end = *start + tile_per_p-1;
            
        }
        else{
            if(tile_per_p == 0)
            {
                *start = -1;
                *end = -1;
            }
            else{
                *start = tile_per_p * rank + remainder;
                *end = *start + tile_per_p-1;
            }
        }
        
        
    }
    else{
        
        *start = tile_per_p * rank;
        *end = *start + tile_per_p-1;
    }
}

//returns file index, level index, local tilex and tiley coordinate, tilewidth and tileheight
static void get_tile_coordinate(int64_t start_index,
                                int64_t tile_index,
                                int no_of_files,
                                int32_t * level_numbers,
                                int64_t *** size_width_height,
                                int64_t ** tile_numbers,
                                int level,
                                int *file_index,
                                int *level_index,
                                int64_t *tilex_coordinate,
                                int64_t *tiley_coordinate,
                                int64_t *tile_width,
                                int64_t *tile_height)
{
    int i,j,k;
    int64_t temp_index = 0;
    bool flag = false;
    int64_t local_tile_index;
    for(i = 0; i< no_of_files; i++)
    {
        if(level == -1)
        {
            for(j=0; j< level_numbers[i]; j++)
            {
                if(temp_index  <= tile_index && tile_index < (temp_index + tile_numbers[i][j]))
                {
                    *file_index = i;
                    *level_index = j;
                    local_tile_index = tile_index - temp_index;
                    flag = true;
                    break;
                }
                else   temp_index += tile_numbers[i][j];
            }
        }else
        {
            if(temp_index  <= tile_index && tile_index < (temp_index + tile_numbers[i][level]))
            {
                *file_index = i;
                *level_index = level;
                local_tile_index = tile_index - temp_index;
                flag = true;
                break;
                
            }
            else   temp_index += tile_numbers[i][level];
            
        }
        if(flag ==true)
            break;
        else{
            *file_index = no_of_files -1;
            *level_index = level_numbers[no_of_files-1]-1;
            local_tile_index = tile_index - temp_index;
            
        }
        
        
    }
    
    
    //local_tile_index = tile_index - start_index  ;
    int64_t row_numbers = ceil((size_width_height[*file_index][*level_index][1]*1.0)/(*tile_height*1.0));
    int64_t col_numbers = ceil((size_width_height[*file_index][*level_index][0]*1.0)/(*tile_width*1.0));
    int64_t row_index = local_tile_index / col_numbers;
    int64_t col_index = local_tile_index % col_numbers;
    *tiley_coordinate = *tile_height * row_index;
    *tilex_coordinate = *tile_width * col_index;
    
    if((*tiley_coordinate + *tile_height)> size_width_height[*file_index][*level_index][1])
        *tile_height = size_width_height[*file_index][*level_index][1] % (*tile_height);
    
    if((*tilex_coordinate + *tile_width)> size_width_height[*file_index][*level_index][0])
        *tile_width = size_width_height[*file_index][*level_index][0] % (*tile_width);
    
    
}

//reads a specific tile to the buffer given as a parameter and returns it
static uint32_t * read_tile(int file_index,
                            char ** file_list,
                            int level_index,
                            int64_t xcoord,
                            int64_t ycoord,
                            int64_t tile_width,
                            int64_t tile_height,
                            int level_factor)
{
    /* if(osr == NULL)
     osr = openslide_open(file_list[file_index]);
     else if(file_index != *prev_file_index){
     openslide_close(osr);
     osr = openslide_open(file_list[file_index]);
     }*/
    
    openslide_t *osr2 = openslide_open(file_list[file_index]);
    int64_t num_bytes = tile_width * tile_height * 4;
    // printf("Going to allocate %"PRId64" bytes...\n", num_bytes);
    uint32_t *buf = (uint32_t *)malloc(num_bytes);
    //printf("Reading file %d region %"PRId64", %"PRId64",%"PRId64",%"PRId64"\n ",file_index, xcoord, ycoord,tile_width, tile_height);
    //calculate level0 coordinates, openslide can only read correct level 0 coordinates. Other level coordinates must be calculated.
    xcoord *= level_factor;
    ycoord *= level_factor;
    
    openslide_read_region(osr2, buf, xcoord, ycoord, level_index, tile_width, tile_height);
    //*prev_file_index = file_index;
    openslide_close(osr2);
    return buf;
    
}

//reads a query image given by the file path and returns it as uint32_t array in ARGB pixel format
static uint32_t * read_query_image(char * file_path, int64_t * width, int64_t *height )
{
    //openslide_t *osr = openslide_open(file_path);
    uint32_t *buffer = NULL;
    int64_t xdim,ydim;
    Mat image1,image2;
    image1 = imread(file_path, 1);//opencv 2.4 version, newer version use imread() function
    //imshow( "Display window",image1);
    //waitKey(0);
    
    image2 = Mat(image1.size().height, image1.size().width,CV_8UC4);
    //printf("TYPE: %d\n", image1.type());
    *width = image2.cols;
    *height = image2.rows;
    cvtColor( image1,image2, CV_BGR2BGRA);
    buffer = (uint32_t *)malloc(image2.rows*image2.cols*(sizeof(uint32_t)));
    int i, j;
    uint32_t val = 0;
    uint32_t a,r,g,b;
    
    for(i = 0; i < image2.rows; i++)
        for(j = 0; j< image2.cols;j++)
        {
            val = 0;
            a =image2.at<Vec4b>(i,j)[3];
            r =image2.at<Vec4b>(i,j)[2];
            g =image2.at<Vec4b>(i,j)[1];
            b =image2.at<Vec4b>(i,j)[0];
            //printf("%d %d %d %d \n", a,r,g,b);
            val += ((a << 24)& 0xFF000000 ) | ((r <<16) & 0x00FF0000) | ((g << 8) & 0x0000FF00 ) | ((b <<0) & 0x000000FF);
            buffer [i * image2.cols + j] = val;
        }
    
    return buffer;
    
}
//writes a specific tile into a ppm file
static void write_tile_as_ppm(openslide_t *osr,
                              uint32_t *buffer,
                              uint64_t width,
                              uint64_t height,
                              char * output_filename_prefix,
                              int processor_no,
                              int file_index,
                              char **file_list,
                              int level_index,
                              int64_t tile_index)
{
    uint8_t bg_r = 0xFF;
    uint8_t bg_g = 0xFF;
    uint8_t bg_b = 0xFF;
    osr = openslide_open(file_list[file_index]); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    const char *bgcolor = openslide_get_property_value(osr, OPENSLIDE_PROPERTY_NAME_BACKGROUND_COLOR);
    openslide_close(osr);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (bgcolor) {
        uint64_t bg = g_ascii_strtoull(bgcolor, NULL, 16);
        bg_r = (bg >> 16) & 0xFF;
        bg_g = (bg >> 8) & 0xFF;
        bg_b = bg & 0xFF;
        //printf("background: (%d, %d, %d)\n", bg_r, bg_g, bg_b);
    }
    char * filename = g_strdup_printf("%s-F%d-L%d-T%"PRId64"-P%d.ppm", output_filename_prefix, file_index, level_index, tile_index, processor_no);
    write_as_ppm(filename, width, height, buffer, bg_r, bg_g, bg_b);
    //free(buffer);
    g_free(filename);
    
}


static uint8_t apply_alpha(uint8_t s, uint8_t a, uint8_t d) {
    double ss = s / 255.0;
    double aa = a / 255.0;
    double dd = d / 255.0;
    return round((ss + (1 - aa) * dd) * 255.0);
}
static void write_as_ppm(const char *filename,
                         int64_t w, int64_t h, uint32_t *buf,
                         uint8_t br, uint8_t bg, uint8_t bb) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        perror("Cannot open file");
        return;
    }
    
    fprintf(f, "P6\n%"PRId64" %"PRId64"\n255\n", w, h);
    for (int64_t i = 0; i < w * h; i++) {
        uint32_t val = buf[i];
        uint8_t a = (val >> 24) & 0xFF;
        uint8_t r = (val >> 16) & 0xFF;
        uint8_t g = (val >> 8) & 0xFF;
        uint8_t b = (val >> 0) & 0xFF;
        
        // composite against background with OVER
        r = apply_alpha(r, a, br);
        g = apply_alpha(g, a, bg);
        b = apply_alpha(b, a, bb);
        
        putc(r, f);
        putc(g, f);
        putc(b, f);
    }
    
    fclose(f);
}

//calculates the ratio of the size of the level 0 image to level x image
static int calculate_level_factor(int64_t *** size_width_height, int level_index, int file_index)
{
    return (int)(size_width_height[file_index][0][0]/size_width_height[file_index][level_index][0]);
    
}

int main(int argc, char* argv[]){
    int  my_rank; /* rank of process */
    int  p;       /* number of processes */
    
    FILE *log_file;
    MPI_Status status ;   /* return status for receive */
    
    /* start up MPI */
    MPI_Init(&argc, &argv);
    
    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm gcomm = MPI_COMM_WORLD;
    //printf("Initializing dspaces\n");
    //dspaces_init(p,0,&gcomm, NULL);
    //printf("Initialized dspaces\n");
    
    int no_of_files;
    char **file_list;
    int32_t * level_numbers;
    int64_t *** size_width_height;
    int64_t ** tile_numbers;
    int i,j;
    int64_t tilex,tiley;
    int level = -1;
    char **path =(char **) malloc(2 * sizeof(char*));
    path[0] = (char *)malloc(PATH_SIZE *sizeof(char));//dataset path
    path[1] = (char *)malloc(PATH_SIZE *sizeof(char));//query image path
    char c;
    
    double t_start, t_end;
    int64_t t_read_bytes; //, t_write_bytes;
    t_read_bytes = 0;// t_write_bytes = 0 ;
    t_start = MPI_Wtime();
    //master
    if(my_rank == 0)
    {
        while(1)
        {
            static struct option long_options[] = {
                {"tilex",  required_argument, 0, 'x'},
                {"tiley",  required_argument, 0, 'y'},
                {"level",    required_argument, 0, 'l'},
                {0, 0, 0, 0}};
            int option_index = 0;
            c = getopt_long (argc, argv, "x:y:l:",long_options, &option_index);
            if (c == -1)
                break;
            switch (c)
            {
                case 0:
                    /* If this option set a flag, do nothing else now. */
                    if (long_options[option_index].flag != 0)
                        break;
                    printf ("option %s", long_options[option_index].name);
                    if (optarg)
                        printf (" with arg %s", optarg);
                    printf ("\n");
                    break;
                    
                case 'x':
                    tilex = (int64_t) strtoull(optarg,NULL,10);
                    //printf("tilex %"PRId64"\n",tilex);
                    break;
                    
                case 'y':
                    tiley = (int64_t) strtoull(optarg,NULL,10);
                    //printf("tiley %"PRId64"\n",tiley);
                    break;
                    
                case 'l':
                    level = (int32_t) strtoul(optarg,NULL,10);
                    //printf("level %d\n",level);
                    break;
                    
                default:
                    abort ();
            }
        }
        if (optind < argc)
        {
            i = 0;
            while (optind < argc)
            {
                path[i++] = g_strdup_printf("%s",argv[optind++]);
                
            }
        }
        
    }
    //broadcast the arguments to everybody
    MPI_Bcast(&tilex, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tiley, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&level, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(path[0], PATH_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    
    double t1, t2;
    t1 = MPI_Wtime();//start timer for total time
    
    file_list = get_file_list(path[0], &no_of_files);
    
    MPI_Bcast(&no_of_files, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    //master
    if(my_rank == 0)
        level_numbers = get_level_number_list(file_list, no_of_files);
    else{//slaves
        printf("ALLOCATING\n");
        level_numbers =(int32_t *) malloc(no_of_files * sizeof(int32_t));
        printf("ALLOCATED");
    }
    printf("%d:Hello\n", my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    //everybody
    MPI_Bcast(level_numbers, no_of_files, MPI_INT32_T, 0, MPI_COMM_WORLD);
    printf("Hello after broadcast\n");
    //master
    if(my_rank == 0)
        size_width_height = get_dimensions(file_list, no_of_files, level_numbers);
    else{//slaves
        size_width_height = (int64_t ***)malloc(no_of_files * sizeof(int64_t **));
        for(i=0; i<no_of_files; i++)
        {
            size_width_height[i] = (int64_t **)malloc(level_numbers[i]*sizeof(int64_t *));
            for(j=0; j< level_numbers[i]; j++)
                size_width_height[i][j] = (int64_t *)malloc (2* sizeof(int64_t));
        }
    }
    //everybody
    for(i=0; i<no_of_files; i++)
        for(j=0; j< level_numbers[i]; j++)
            MPI_Bcast(size_width_height[i][j], 2, MPI_INT64_T, 0, MPI_COMM_WORLD);
    
    uint32_t *query_image_buffer;
    int64_t query_image_width, query_image_height;
    
    
    //read Query image
    if(my_rank == 0)
        query_image_buffer = read_query_image(path[1], &query_image_width, &query_image_height);
    //distribute to everybody
    MPI_Bcast(&query_image_width, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&query_image_height, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);
    if(my_rank!=0)
        query_image_buffer = (uint32_t *)malloc(query_image_width*query_image_height*sizeof(uint32_t));
    MPI_Bcast(query_image_buffer,query_image_width*query_image_height , MPI_UINT32_T, 0, MPI_COMM_WORLD);
    
    
    tile_numbers = get_tile_numbers(size_width_height, no_of_files, level_numbers, tilex, tiley );
    int64_t total_tile_number  =  get_total_tile_number(tile_numbers, no_of_files, level_numbers, level);
    
    int64_t start_tile, end_tile;
    get_tile_range(total_tile_number, my_rank, p, &start_tile, &end_tile);
    printf("My RANK IS %d and I am assigned tiles %"PRId64" - %"PRId64" ranges \n",my_rank,start_tile,end_tile);
    
    int file_index, level_index;
    int64_t tilex_coordinate, tiley_coordinate, tile_height, tile_width;
    tile_height = tiley;
    tile_width = tilex;
    
    //cbir input parameters
    float OLP=0.25; //can be given as a parameter
    int KBINS = 6;//same
    int FEA_INDEX = 0; //same
    float BETA = 1.2;
    vector<result_distance_t> distance_rs_fs;
    double total_rs_read_time = 0;
    double total_fs_read_time = 0;
    double total_final_fs_read_time = 0;
    double total_rs_time = 0;
    double total_fs_time = 0;
    double total_cl_fs_time = 0;
    double total_ex_sort_1 = 0;
    double total_ex_sort_2 =0;
    double total_ex_sort_3 = 0;
    double read_start, read_end;
    double rs_start, rs_end;
    double fs_start, fs_end;
    double cl_fs_start, cl_fs_end;
    double ex_sort_start, ex_sort_end;
    
    Mat QueryImg, DataImg;
    if(start_tile != -1)
        //start read and write
    {
         //ROUGH SEARCHING : read + rs + sort_write
        rs_start=MPI_Wtime();
        for(i=start_tile; i <= end_tile; i++ )
        {
            
            get_tile_coordinate(start_tile,
                                i,
                                no_of_files,
                                level_numbers,
                                size_width_height,
                                tile_numbers,
                                level,
                                &file_index,
                                &level_index,
                                &tilex_coordinate,
                                &tiley_coordinate,
                                &tile_width,
                                &tile_height);
            
            //read the tile
            uint32_t * buffer = NULL;
            
            
            int level_factor = calculate_level_factor(size_width_height, level_index, file_index);
            read_start = MPI_Wtime();
            buffer = read_tile(file_index,
                               file_list,
                               level_index,
                               tilex_coordinate,
                               tiley_coordinate,
                               tile_width,
                               tile_height,
                               level_factor);
            read_end = MPI_Wtime();
            total_rs_read_time += read_end - read_start;
            t_read_bytes += tile_height * tile_width * 4;
            //Then reset tile height and tile width back to original form
           
            vector<result_distance_t> patch_distance_vector;
            
            DataImg = Mat( tile_height, tile_width, CV_8UC3 );
            QueryImg = Mat( query_image_height, query_image_width, CV_8UC3 );
            unsigned char * tileBuf_ptr = (unsigned char *)buffer;
            unsigned char * queryBuf_ptr = (unsigned char *)query_image_buffer;
            for( int j=0; j<tile_height; j++ ){
                for( int i=0; i<tile_width; i++ ){
                    for( int iChn=0; iChn<3; iChn++ ){
                        DataImg.at<unsigned char>(j,3*i+iChn) = tileBuf_ptr[j*tile_width*4+i*4+(iChn)]; //a change was made
                    }//END_FOR_iCh
                }//END_FOR_I
            }//END_FOR_J
            
            for( int j=0; j<query_image_height; j++ ){
                for( int i=0; i<query_image_width; i++ ){
                    for( int iChn=0; iChn<3; iChn++ ){
                        QueryImg.at<unsigned char>(j,3*i+iChn) = queryBuf_ptr[j*query_image_width*4+i*4+(iChn)]; //a change was made
                    }//END_FOR_iCh
                }//END_FOR_I
            }//END_FOR_J
            
        
            tileCBIR_roughS_omp(patch_distance_vector,
                                DataImg,
                                tile_height,
                                tile_width,
                                QueryImg,
                                query_image_height,
                                query_image_width,
                                i,
                                OLP,
                                KBINS,
                                FEA_INDEX,
                                file_list[file_index],
                                file_index);
            
            total_rs_time += rs_end - rs_start;
            
            insertionsort_patch_distance_asc(patch_distance_vector);// since the results of only a single is sorted, it is ok to use merge sort.

            for(int x = 0 ; x< patch_distance_vector.size(); x++)
            {
                patch_distance_vector[x].tile_x = tilex_coordinate;
                patch_distance_vector[x].tile_y = tiley_coordinate;
                patch_distance_vector[x].tile_width = tile_width;
                patch_distance_vector[x].tile_height = tile_height;
            }
            
            distance_rs_fs.insert(distance_rs_fs.end(), patch_distance_vector.begin(), patch_distance_vector.end());
            patch_distance_vector.clear();//will be deleted after checking
            long long current_vector_size_in_bytes = distance_rs_fs.size() * sizeof(result_distance_t);
            if(current_vector_size_in_bytes > MEMORY_SIZE)
            {
                sort_write_vector_to_disk(distance_rs_fs, i, my_rank, "./sorted/");
                printf("%d:VECTOR SIZE:%d\n",my_rank, distance_rs_fs.size());
                distance_rs_fs.clear();
            }
            
            
            //Then reset tile height and tile width back to original form
            tile_height = tiley;
            tile_width = tilex;
            free(buffer);
            
            
        }
        
        if(distance_rs_fs.size()>0)
        {
            sort_write_vector_to_disk(distance_rs_fs, i, my_rank, "./sorted/");
            printf("%d:VECTOR SIZE:%d\n",my_rank, distance_rs_fs.size());
            distance_rs_fs.clear();
        }
        
        rs_end = MPI_Wtime();
    }
    total_rs_time = rs_end - rs_start;
    
    
    
    printf("%d:Coming to the barrier\n",my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    //EXTERNAL SORT - CROP - SORT
    ex_sort_start = MPI_Wtime();
    
    external_parallel_sort_multifile_MPIIO(my_rank,
                                           p,
                                           "./sorted",
                                           "sorted_rs_results.bin",
                                           &two_way_merge_based_on_distance);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(my_rank == 0)
        printf("RESULTS ARE SORTED BASED ON DISTANCE\n");
    
    
    top_x_percent_sorted_fs_results_MPIIO(0.1, my_rank, p, "sorted_rs_results.bin", "top_10_sorted_rs_results.bin");
    
    
    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank == 0)
        printf("TOP 10 PERCENT IS CROPPED\n");
    
    external_parallel_sort_MPIIO(my_rank,
                                 p ,
                                 "top_10_sorted_rs_results.bin",
                                 "./sorted2",
                                 "sorted_top_10_tileID_rs_results.bin",
                                 &two_way_merge_based_on_tile_ID,
                                 &compare_patch_tile_ID_asc);    //read spilled fine searching results file is sorted in the disk in parallel.
    ex_sort_end = MPI_Wtime();
    
    total_ex_sort_1 = ex_sort_end - ex_sort_start;
    
    
    //FINE SEARCHING
    
    fs_start = MPI_Wtime();
    
    long long start_offset, end_offset, my_number_of_tiles;
    get_tile_record_positions(start_offset, end_offset, my_number_of_tiles, my_rank, "sorted_top_10_tileID_rs_results.bin", p);
    
    printf("%d: offsets: %lld %lld\n",my_rank, start_offset, end_offset );
    
    //READ THEM INTO BUFFER AND REPEAT STEPS FOR FS
    
    total_fs_read_time = cbir_fs_after_rs_workflow(start_offset,
                               end_offset,
                               my_number_of_tiles,
                               "sorted_top_10_tileID_rs_results.bin",
                               level,
                               QueryImg,
                               query_image_height,
                               query_image_width,
                               OLP,
                               KBINS,
                               FEA_INDEX,
                               "./sorted/",
                               my_rank,
                               size_width_height
                               );
    
    MPI_Barrier(MPI_COMM_WORLD);
    fs_end = MPI_Wtime();
    
    total_fs_time = fs_end - fs_start;
    
    if(my_rank == 0)
        printf("FINE SEARCHING FINISHED\n");
    
    //EXTERNAL SORT THE RESULTS
    ex_sort_start = MPI_Wtime();
    external_parallel_sort_multifile_MPIIO(my_rank,
                                           p,
                                           "./sorted",
                                           "sorted_fs_results.bin",
                                           &two_way_merge_based_on_distance);
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    if(my_rank == 0)
        printf("FS RESULTS ARE SORTED based on distance\n");
    
    //CROP TOP 10 PERCENT
    
    top_x_percent_sorted_fs_results_MPIIO(0.1, my_rank, p, "sorted_fs_results.bin", "top_10_sorted_fs_results.bin");
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    if(my_rank == 0)
        printf("FS RESULTS ARE CROPPED\n");

    //SORT BY TILE ID
    external_parallel_sort_MPIIO(my_rank,
                                 p ,
                                 "top_10_sorted_fs_results.bin",
                                 "./sorted2",
                                 "sorted_top_10_tileID_fs_results.bin",
                                 &two_way_merge_based_on_tile_ID,
                                 &compare_patch_tile_ID_asc);
    
    
    if(my_rank == 0)
        printf("FS RESULTS ARE SORTED based on tile id\n");
    MPI_Barrier(MPI_COMM_WORLD);
    ex_sort_end = MPI_Wtime();
    
    total_ex_sort_2 = ex_sort_end - ex_sort_start;
    
    //CLUSTERING + FINE SEARCHING
    cl_fs_start = MPI_Wtime();
    
    get_tile_record_positions(start_offset, end_offset, my_number_of_tiles, my_rank, "sorted_top_10_tileID_fs_results.bin", p);
   
    printf("%d: offsets: %lld %lld\n",my_rank, start_offset, end_offset );
   
    
    total_final_fs_read_time = cbir_cluster_fs_workflow(start_offset,
                             end_offset,
                             my_number_of_tiles,
                             "sorted_top_10_tileID_fs_results.bin",
                             BETA,
                             level,
                             (unsigned char *)query_image_buffer,
                             query_image_height,
                             query_image_width,
                             OLP,
                             KBINS,
                             FEA_INDEX,
                             "./sorted/",
                             my_rank,
                             size_width_height);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    cl_fs_end = MPI_Wtime();
    total_cl_fs_time = cl_fs_end - cl_fs_start;
    if(my_rank == 0)
        printf("ClUSTERING AND FINAL FINE SEARCHING FINISHED BUT NOT SORTED\n");

    
    ex_sort_start = MPI_Wtime();
    external_parallel_sort_multifile_MPIIO(my_rank,
                                           p,
                                           "./sorted",
                                           "final_sorted_results.bin",
                                           &two_way_merge_based_on_distance);
    ex_sort_end = MPI_Wtime();
    total_ex_sort_3 = ex_sort_end - ex_sort_start;
    
    t_end = MPI_Wtime();
    
    double total = t_end - t_start;
    
    
   // write_image("final_sorted_results.bin", query_image_width, query_image_height, my_rank, level);
    free(query_image_buffer);
    
    
  
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(my_rank == 0)
    {
    //char output_file_path[300];
     //   sprintf(output_file_path, "results-proc-%d.log", my_rank);
        log_file = fopen("results.log","a+");
        fprintf(log_file, "RS_Read_time\tFS_Read_time\tFINAL_FS_Read_time\tRS_time\tEX_Sort_1\tFS_time\tEx_Sort_time2\tCL_FS_Time\tEx_sort_3\tTotal_time\tBytes_processed\n");
        fprintf(log_file, "%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%"PRId64"\n",my_rank, total_rs_read_time, total_fs_read_time, total_final_fs_read_time, total_rs_time, total_ex_sort_1, total_fs_time, total_ex_sort_2, total_cl_fs_time, total_ex_sort_3, total, t_read_bytes);
        
        fclose(log_file);
    }
    //dspaces_finalize();
    /* shut down MPI */
    MPI_Finalize();
    
    
    return 0;
}
