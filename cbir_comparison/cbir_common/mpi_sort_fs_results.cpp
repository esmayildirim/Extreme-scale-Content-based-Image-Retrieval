
#include "mpi_sort_fs_results.h"

#define MAX_MEMORY_SIZE 104857600
#define PATH_SIZE 200

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
        file_list[i] = (char *)malloc(PATH_SIZE * sizeof(char));
    
    directory = opendir(path);
    i = 0;
    while ((dir = readdir(directory)) != NULL)
    {
        
        if(strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0)
            continue;
        file_list[i] = g_strdup_printf("%s/%s", path, dir->d_name);
        //printf("%s\n",file_list[i]);
        i++;
    }
    closedir(directory);
    return file_list;
    
}


void sort_write_vector_to_disk(vector<result_distance_t> fs_results, int iteration_no, int pid, char * write_folder)
{
    //sort(fs_results.begin(), fs_results.end(), compare_patch_distance_asc);  //standard library sort funtion - faster but consumes a lot of stack memory
    insertionsort_patch_distance_asc(fs_results); //my sort function -slower but consumes less stack memory
    
    result_distance_t temp;
    MPI_Datatype MPI_fs_results;
    MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT, MPI_INT};
    int blocklen[10] = {1,1,1,200,1,1,1,1,1,1};
    MPI_Aint disp[10];
    disp[0] = offsetof(result_distance_t, iPatX);
    disp[1] = offsetof(result_distance_t, iPatY);
    disp[2] = offsetof(result_distance_t, tile_ID);
    disp[3] = offsetof(result_distance_t, file_name);
    disp[4] = offsetof(result_distance_t, dis);
    disp[5] = offsetof(result_distance_t, tile_x);
    disp[6] = offsetof(result_distance_t, tile_y);
    disp[7] = offsetof(result_distance_t, tile_width);
    disp[8] = offsetof(result_distance_t, tile_height);
    disp[9] = offsetof(result_distance_t, file_index);
    MPI_Type_create_struct(10,blocklen, disp, type, &MPI_fs_results);
    MPI_Type_commit(&MPI_fs_results);
    int data_type_size;
    MPI_Type_size(MPI_fs_results, &data_type_size);
    
    char *output_file_name = g_strdup_printf("%s/sorted-%d-%d.bin",write_folder,iteration_no, pid);
    
    MPI_File write_fh;
    MPI_Status status;
    
    MPI_File_open(MPI_COMM_SELF, output_file_name, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &write_fh);
    for (int i = 0; i<fs_results.size(); i++) {
        MPI_File_write(write_fh, &fs_results[i], 1, MPI_fs_results, &status);
    }
    MPI_File_close(&write_fh);
    MPI_Type_free(&MPI_fs_results);
}

void two_way_merge_based_on_distance(char * sorted_folder_path,
                                     char * sorted_file1,
                                     char * sorted_file2,
                                     int iteration,
                                     int pid,
                                     int phase_number)
{
    result_distance_t temp;
    MPI_Datatype MPI_fs_results;
    MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT, MPI_INT};
    int blocklen[10] = {1,1,1,200,1,1,1,1,1,1};
    MPI_Aint disp[10];
    disp[0] = offsetof(result_distance_t, iPatX);
    disp[1] = offsetof(result_distance_t, iPatY);
    disp[2] = offsetof(result_distance_t, tile_ID);
    disp[3] = offsetof(result_distance_t, file_name);
    disp[4] = offsetof(result_distance_t, dis);
    disp[5] = offsetof(result_distance_t, tile_x);
    disp[6] = offsetof(result_distance_t, tile_y);
    disp[7] = offsetof(result_distance_t, tile_width);
    disp[8] = offsetof(result_distance_t, tile_height);
    disp[9] = offsetof(result_distance_t, file_index);
    MPI_Type_create_struct(10,blocklen, disp, type, &MPI_fs_results);
    MPI_Type_commit(&MPI_fs_results);
    int data_type_size;
    MPI_Type_size(MPI_fs_results, &data_type_size);
    
    
    char *output_file_name = g_strdup_printf("%s/sorted-phase-%d-%d-%d.bin",sorted_folder_path,iteration, pid, phase_number);
    
    MPI_File read_fh1, read_fh2, write_fh;
    MPI_Status status;
    long long number_of_bytes_file1, number_of_bytes_file2;
    long long number_of_elements_file1, number_of_elements_file2;
    
    MPI_File_open(MPI_COMM_SELF, sorted_file1, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh1);
    MPI_File_open(MPI_COMM_SELF, sorted_file2, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh2);
    MPI_File_open(MPI_COMM_SELF, output_file_name, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &write_fh);
    
    MPI_File_get_size(read_fh1, &number_of_bytes_file1);
    MPI_File_get_size(read_fh2, &number_of_bytes_file2);
    
    number_of_elements_file1 = number_of_bytes_file1 / data_type_size;
    number_of_elements_file2 = number_of_bytes_file2 / data_type_size;
    //printf("%d: elements %lld %lld\n", pid, number_of_elements_file1, number_of_elements_file2);
    int flag1, flag2 ;
    flag1 = flag2 = 1;
    
    long long index1, index2;
    index1 = index2 = 0;
    result_distance_t elem1,elem2;
    while(index1 < number_of_elements_file1 && index2 < number_of_elements_file2)
    {
        if(flag1)
        {
            
            MPI_File_read(read_fh1, &elem1, 1, MPI_fs_results, &status);
            flag1 = 0;
            
        }
        if(flag2)
        {
            MPI_File_read(read_fh2, &elem2, 1, MPI_fs_results, &status);
            flag2 = 0;
            
        }
        if(elem1.dis < elem2.dis)
        {
            MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
            flag1 = 1;
            index1++;
        }
        else
        {
            MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
            flag2 = 1;
            index2++;        }
    }
    //printf("%d:INDICES %lld %lld\n ", pid, index1, index2);
    while(index1 < number_of_elements_file1)
    {
        if(flag1 == 0)
        {
            MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
            index1++;
            flag1 = 1;
            
        }else{
            MPI_File_read(read_fh1, &elem1, 1, MPI_fs_results, &status);
            MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
            index1++;
            
        }
        
    }
    while(index2 < number_of_elements_file2)
    {
        if(flag2 == 0)
        {
            MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
            index2++;
            flag2 = 1;
        }else{
            
            MPI_File_read(read_fh2, &elem2, 1, MPI_fs_results, &status);
            MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
            index2++;
        }
    }
    
    
    MPI_File_close(&write_fh);
    MPI_File_close(&read_fh1);
    MPI_File_close(&read_fh2);
    MPI_File_delete(sorted_file1, MPI_INFO_NULL);
    MPI_File_delete(sorted_file2, MPI_INFO_NULL);
    MPI_Type_free(&MPI_fs_results);
    
}


void two_way_merge_based_on_tile_ID(char * sorted_folder_path,
                                            char * sorted_file1,
                                            char * sorted_file2,
                                            int iteration,
                                            int pid,
                                            int phase_number)
{
    result_distance_t temp;
    MPI_Datatype MPI_fs_results;
    MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT, MPI_INT};
    int blocklen[10] = {1,1,1,200,1,1,1,1,1,1};
    MPI_Aint disp[10];
    disp[0] = offsetof(result_distance_t, iPatX);
    disp[1] = offsetof(result_distance_t, iPatY);
    disp[2] = offsetof(result_distance_t, tile_ID);
    disp[3] = offsetof(result_distance_t, file_name);
    disp[4] = offsetof(result_distance_t, dis);
    disp[5] = offsetof(result_distance_t, tile_x);
    disp[6] = offsetof(result_distance_t, tile_y);
    disp[7] = offsetof(result_distance_t, tile_width);
    disp[8] = offsetof(result_distance_t, tile_height);
    disp[9] = offsetof(result_distance_t, file_index);
    MPI_Type_create_struct(10,blocklen, disp, type, &MPI_fs_results);
    MPI_Type_commit(&MPI_fs_results);
    int data_type_size;
    MPI_Type_size(MPI_fs_results, &data_type_size);
    
    
    char *output_file_name = g_strdup_printf("%s/sorted-phase-%d-%d-%d.bin",sorted_folder_path,iteration, pid, phase_number);
    
    MPI_File read_fh1, read_fh2, write_fh;
    MPI_Status status;
    long long number_of_bytes_file1, number_of_bytes_file2;
    long long number_of_elements_file1, number_of_elements_file2;
    
    MPI_File_open(MPI_COMM_SELF, sorted_file1, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh1);
    MPI_File_open(MPI_COMM_SELF, sorted_file2, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh2);
    MPI_File_open(MPI_COMM_SELF, output_file_name, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &write_fh);
    
    MPI_File_get_size(read_fh1, &number_of_bytes_file1);
    MPI_File_get_size(read_fh2, &number_of_bytes_file2);
    
    number_of_elements_file1 = number_of_bytes_file1 / data_type_size;
    number_of_elements_file2 = number_of_bytes_file2 / data_type_size;
    //printf("%d: elements %lld %lld\n", pid, number_of_elements_file1, number_of_elements_file2);
    int flag1, flag2 ;
    flag1 = flag2 = 1;
    
    long long index1, index2;
    index1 = index2 = 0;
    result_distance_t elem1,elem2;
    while(index1 < number_of_elements_file1 && index2 < number_of_elements_file2)
    {
        if(flag1)
        {
            
            MPI_File_read(read_fh1, &elem1, 1, MPI_fs_results, &status);
            flag1 = 0;
            
        }
        if(flag2)
        {
            MPI_File_read(read_fh2, &elem2, 1, MPI_fs_results, &status);
            flag2 = 0;
            
        }
        if(elem1.tile_ID < elem2. tile_ID)
        {
            MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
            flag1 = 1;
            index1++;
        }
        else if(elem1.tile_ID > elem2. tile_ID)
        {
            MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
            flag2 = 1;
            index2++;
        }else
        {
            if(elem1.iPatX < elem2.iPatX)
            {
                MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
                flag1 = 1;
                index1++;
            }
            else if(elem1.iPatX> elem2.iPatX)
            {
                MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
                flag2 = 1;
                index2++;
            
            }
            else{
                
                if(elem1.iPatY < elem2.iPatY)
                {
                    MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
                    flag1 = 1;
                    index1++;
                }
                else
                {
                    MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
                    flag2 = 1;
                    index2++;
                }
            
            }
        
        
        
        
        }
    }
    //printf("%d:INDICES %lld %lld\n ", pid, index1, index2);
    while(index1 < number_of_elements_file1)
    {
        if(flag1 == 0)
        {
            MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
            index1++;
            flag1 = 1;
            
        }else{
                MPI_File_read(read_fh1, &elem1, 1, MPI_fs_results, &status);
                MPI_File_write(write_fh, &elem1, 1, MPI_fs_results, &status);
                index1++;
            
        }
    
    }
    while(index2 < number_of_elements_file2)
    {
        if(flag2 == 0)
        {
            MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
            index2++;
            flag2 = 1;
        }else{
            
            MPI_File_read(read_fh2, &elem2, 1, MPI_fs_results, &status);
            MPI_File_write(write_fh, &elem2, 1, MPI_fs_results, &status);
            index2++;
        }
    }
    
    
    MPI_File_close(&write_fh);
    MPI_File_close(&read_fh1);
    MPI_File_close(&read_fh2);
    MPI_File_delete(sorted_file1, MPI_INFO_NULL);
    MPI_File_delete(sorted_file2, MPI_INFO_NULL);
    MPI_Type_free(&MPI_fs_results);
}

int external_parallel_sort_MPIIO(int pid,
                                 int num_processes,
                                 char *file_name,
                                 char *sort_folder_path,
                                 char *dest_file_name,
                                 void (*merge_func)(char *, char *, char *, int, int, int),
                                 bool (*compare_func)(result_distance_t, result_distance_t)
                                 )
{   MPI_File read_fh, write_fh;
    MPI_Status status;
    MPI_Offset my_offset, my_current_offset;
    MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
    long long num_of_bytes;
    MPI_File_get_size(read_fh,&num_of_bytes);
    MPI_File_close(&read_fh);
    
    result_distance_t temp;
    MPI_Datatype MPI_fs_results;
    MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT, MPI_INT};
    int blocklen[10] = {1,1,1,200,1,1,1,1,1,1};
    MPI_Aint disp[10];
    disp[0] = offsetof(result_distance_t, iPatX);
    disp[1] = offsetof(result_distance_t, iPatY);
    disp[2] = offsetof(result_distance_t, tile_ID);
    disp[3] = offsetof(result_distance_t, file_name);
    disp[4] = offsetof(result_distance_t, dis);
    disp[5] = offsetof(result_distance_t, tile_x);
    disp[6] = offsetof(result_distance_t, tile_y);
    disp[7] = offsetof(result_distance_t, tile_width);
    disp[8] = offsetof(result_distance_t, tile_height);
    disp[9] = offsetof(result_distance_t, file_index);
    MPI_Type_create_struct(10,blocklen, disp, type, &MPI_fs_results);
    MPI_Type_commit(&MPI_fs_results);
    int data_type_size;
    MPI_Type_size(MPI_fs_results, &data_type_size);
    
    
    //printf("%d\n", data_type_size);
    long long num_of_elements = num_of_bytes/data_type_size;
    
    
    MPI_Group world_group, new_comm_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    
    long num_of_elements_per_p = num_of_elements/num_processes;
    long remainder = num_of_elements % num_processes;
    
    if(remainder>0 && num_of_elements_per_p == 0)
    {
        int *ranks = (int *)malloc((num_processes-remainder) * sizeof(int));
        for(int i = 0; i < num_processes-remainder ; i++)
        {
            ranks[i] = remainder + i;
          
        }
        MPI_Group_excl(world_group, num_processes-remainder,ranks, &new_comm_group);
    }
    else{
        
        //MPI_Group_excl(world_group, 0,{}, &new_comm_group);
        MPI_Comm_group(MPI_COMM_WORLD, &new_comm_group);
    }
    MPI_Comm prime_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, new_comm_group, 0, &prime_comm);
    
    
    if(MPI_COMM_NULL != prime_comm){
        //Calculate reading offset and number of elements based on pid and num of processes
        
        if(remainder > 0)
        {
            if((pid+1)<=remainder)
            {
                num_of_elements_per_p++;
                my_offset = num_of_elements_per_p * pid;
            }
            else my_offset = num_of_elements_per_p * pid + remainder;
        }else {
            my_offset = num_of_elements_per_p *pid;
            
        }
        
        
        
        my_offset *= data_type_size;
        ////////////////////////////////////////
        //Read portions of the file, sort and write to separate files
        
        MPI_File_open(prime_comm, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        MPI_File_seek(read_fh, my_offset, MPI_SEEK_SET);
        MPI_File_get_position(read_fh, &my_current_offset);
        
        // printf ("%3d: my current offset is %lld\n", pid, my_current_offset);
        long vector_element_size = MAX_MEMORY_SIZE/data_type_size;
        //printf("%ld %d\n",vector_element_size, num_of_elements_per_p);
        
        int j=0;
        
        int f = 0;
        while(j<num_of_elements_per_p)
        {
            int k =0;
            vector<result_distance_t> sort_vector;
            while(k < vector_element_size && j < num_of_elements_per_p )
            {
                result_distance_t temp;
                MPI_File_read(read_fh, &temp, 1, MPI_fs_results, &status);
                sort_vector.push_back(temp);
                MPI_File_get_position(read_fh, &my_current_offset);
                
                //printf ("%3d: my current offset is %lld\n", pid, my_current_offset);
                k++;
                j++;
            }
            sort(sort_vector.begin(), sort_vector.end(),compare_func);
            //printf("Sort vector size %lu\n", sort_vector.size());
            char *phase1_file_name = g_strdup_printf("%s/sorted-%d-%d", sort_folder_path, pid, f++);
            MPI_File_open(MPI_COMM_SELF, phase1_file_name, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &write_fh);
            for(int i = 0; i < sort_vector.size(); i++)
            {
                MPI_File_write(write_fh, &sort_vector[i], 1, MPI_fs_results, &status);
                // printf("I am writing\n");
            }
            MPI_File_close(&write_fh);
            sort_vector.clear();
        }
        MPI_File_close(&read_fh);
        MPI_Barrier(prime_comm);
        
        //PHASE 2 - multiway-merge
        char **file_list;
        int number_of_sorted_files;
        
        file_list = get_file_list(sort_folder_path,&number_of_sorted_files);
        MPI_Barrier(prime_comm);
        int phase_number = 0;
        while(number_of_sorted_files>1)
        {
            int file_index1, file_index2;
            int iteration = 0;
            
            do{
                file_index1 = iteration * num_processes * 2 + pid * 2;
                file_index2 = file_index1 +1;
                if(file_index2 < number_of_sorted_files)
                {
                    merge_func(sort_folder_path, file_list[file_index1], file_list[file_index2], iteration, pid, phase_number);
                    iteration++;
                }
                else break;
                //printf("%d:%d %d\n",pid,file_index1,file_index2);
            }while(file_index2 < number_of_sorted_files);
            
            MPI_Barrier(prime_comm);
            free(file_list);
            file_list = get_file_list(sort_folder_path,&number_of_sorted_files);
            phase_number++;
            MPI_Barrier(prime_comm);
        }
        MPI_Barrier(prime_comm);
        
        if(pid == 0)
        {
            rename(file_list[0], dest_file_name);
            free(file_list);
        }
        
        MPI_Barrier(prime_comm);
        
        if(pid == 0)
        {
            MPI_File read_fh;
            MPI_Status status;
            MPI_File_open(MPI_COMM_SELF, dest_file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
            for (int i = 0; i< num_of_elements; i++) {
                result_distance_t temp;
                MPI_File_read(read_fh, &temp, 1, MPI_fs_results, &status);
                printf("%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\t%d\n", temp.tile_ID, temp.iPatX, temp.iPatY, temp.dis, temp.tile_x, temp.tile_y, temp.tile_width, temp.tile_height, temp.file_name, temp.file_index);
            }
            printf("NUMBER OF RECORDS:%ld\n", num_of_elements);
            MPI_File_close(&read_fh);
        }
        
        MPI_Comm_free(&prime_comm);
    }
    MPI_Group_free(&world_group);
    MPI_Group_free(&new_comm_group);
    MPI_Type_free(&MPI_fs_results);
    
    return 0;
    
    
}

//combines multiple sorted files into  a single file
int external_parallel_sort_multifile_MPIIO(int pid,
                                           int num_processes,
                                           char *sort_folder_path,
                                           char *dest_file_name,
                                           void (*merge_func)(char *, char *, char *, int, int, int))
{
    result_distance_t temp;
    MPI_Datatype MPI_fs_results;
    MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT, MPI_INT};
    int blocklen[10] = {1,1,1,200,1,1,1,1,1,1};
    MPI_Aint disp[10];
    disp[0] = offsetof(result_distance_t, iPatX);
    disp[1] = offsetof(result_distance_t, iPatY);
    disp[2] = offsetof(result_distance_t, tile_ID);
    disp[3] = offsetof(result_distance_t, file_name);
    disp[4] = offsetof(result_distance_t, dis);
    disp[5] = offsetof(result_distance_t, tile_x);
    disp[6] = offsetof(result_distance_t, tile_y);
    disp[7] = offsetof(result_distance_t, tile_width);
    disp[8] = offsetof(result_distance_t, tile_height);
    disp[9] = offsetof(result_distance_t, file_index);
    MPI_Type_create_struct(10,blocklen, disp, type, &MPI_fs_results);
    MPI_Type_commit(&MPI_fs_results);
    


    //PHASE 2 - multiway-merge
    char **file_list;
    int number_of_sorted_files;
    
    file_list = get_file_list(sort_folder_path,&number_of_sorted_files);
    MPI_Barrier(MPI_COMM_WORLD);
    int phase_number = 0;
    while(number_of_sorted_files>1)
    {
        int file_index1, file_index2;
        int iteration = 0;
        
        do{
            file_index1 = iteration * num_processes * 2 + pid * 2;
            file_index2 = file_index1 +1;
            if(file_index2 < number_of_sorted_files)
            {
                merge_func(sort_folder_path, file_list[file_index1], file_list[file_index2], iteration, pid, phase_number);
                iteration++;
            }
            else break;
            //printf("%d:%d %d\n",pid,file_index1,file_index2);
        }while(file_index2 < number_of_sorted_files);
        
        MPI_Barrier(MPI_COMM_WORLD);
        free(file_list);
        file_list = get_file_list(sort_folder_path,&number_of_sorted_files);
        phase_number++;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(pid == 0)
    {
        rename(file_list[0], dest_file_name);
        free(file_list);
    }
    
    if(pid == 0)
    {
        MPI_File read_fh;
        MPI_Status status;
        
        MPI_File_open(MPI_COMM_SELF, dest_file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        long long num_of_bytes;
        MPI_File_get_size(read_fh, &num_of_bytes);
        
        int data_type_size;
        MPI_Type_size(MPI_fs_results, &data_type_size);
        
        long long num_of_elements = num_of_bytes /data_type_size;
        for (int i = 0; i<num_of_elements ; i++) {
            result_distance_t temp;
            MPI_File_read(read_fh, &temp, 1, MPI_fs_results, &status);
            printf("%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\t%d\n", temp.tile_ID, temp.iPatX, temp.iPatY, temp.dis, temp.tile_x, temp.tile_y, temp.tile_width, temp.tile_height, temp.file_name, temp.file_index);
        }
        printf("NUMBER OF RECORDS:%ld\n", num_of_elements);
        MPI_File_close(&read_fh);
    }
    MPI_Type_free(&MPI_fs_results);

    return 0;


}


void top_x_percent_sorted_fs_results_MPIIO(float percentage, int pid, int num_processes, char * sorted_file_path, char * dest_file_path)
{
    result_distance_t temp;
    MPI_Datatype MPI_fs_results;
    MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT, MPI_INT};
    int blocklen[10] = {1,1,1,200,1,1,1,1,1,1};
    MPI_Aint disp[10];
    disp[0] = offsetof(result_distance_t, iPatX);
    disp[1] = offsetof(result_distance_t, iPatY);
    disp[2] = offsetof(result_distance_t, tile_ID);
    disp[3] = offsetof(result_distance_t, file_name);
    disp[4] = offsetof(result_distance_t, dis);
    disp[5] = offsetof(result_distance_t, tile_x);
    disp[6] = offsetof(result_distance_t, tile_y);
    disp[7] = offsetof(result_distance_t, tile_width);
    disp[8] = offsetof(result_distance_t, tile_height);
    disp[9] = offsetof(result_distance_t, file_index);
    MPI_Type_create_struct(10,blocklen, disp, type, &MPI_fs_results);
    MPI_Type_commit(&MPI_fs_results);
    int data_type_size;
    MPI_Type_size(MPI_fs_results, &data_type_size);
    
    
    //printf("%d\n", data_type_size);
    MPI_File read_fh, write_fh;
    MPI_Status status;
    MPI_Offset my_offset, my_current_offset;
    MPI_File_open(MPI_COMM_WORLD, sorted_file_path, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
    long long num_of_bytes;
    MPI_File_get_size(read_fh,&num_of_bytes);
    
    long long num_of_elements = num_of_bytes/data_type_size;
    num_of_elements *= percentage;
    //Calculate reading offset and number of elements based on pid and num of processes
    
    long long num_of_elements_per_p = num_of_elements/num_processes;
    long remainder = num_of_elements % num_processes;
    
    
    if(remainder > 0)
    {
        if((pid+1)<=remainder)
        {
            num_of_elements_per_p++;
            my_offset = num_of_elements_per_p * pid;
        }
        else my_offset = num_of_elements_per_p * pid + remainder;
    }else {
        my_offset = num_of_elements_per_p *pid;
        
    }
    
    
    my_offset *= data_type_size;
    ////////////////////////////////////////
    //Read portions of the file, write only x percent
    
    
    MPI_File_seek(read_fh, my_offset, MPI_SEEK_SET);
    MPI_File_get_position(read_fh, &my_current_offset);
    
    MPI_File_open(MPI_COMM_WORLD, dest_file_path, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &write_fh);
    MPI_File_seek(write_fh, my_offset, MPI_SEEK_SET);
    
    
    //printf ("%3d: my current offset is %lld\n", pid, my_current_offset);
  //  long vector_element_size = MAX_MEMORY_SIZE/data_type_size;
    //printf("%ld %d\n",vector_element_size, num_of_elements_per_p);
    int j=0;
    
    while(j<num_of_elements_per_p)
    {
      
            result_distance_t temp;
            MPI_File_read(read_fh, &temp, 1, MPI_fs_results, &status);
            MPI_File_write(write_fh, &temp, 1, MPI_fs_results, &status);
        
            j++;
        
    }
    
    
    MPI_File_close(&read_fh);
    MPI_File_close(&write_fh);
    MPI_Barrier(MPI_COMM_WORLD);
    if(pid == 0)
    {
        MPI_File read_fh;
        MPI_Status status;
        MPI_File_open(MPI_COMM_SELF, dest_file_path, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        for (int i = 0; i< num_of_elements; i++) {
            result_distance_t temp;
            MPI_File_read(read_fh, &temp, 1, MPI_fs_results, &status);
            printf("%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\t%d\n", temp.tile_ID, temp.iPatX, temp.iPatY, temp.dis, temp.tile_x, temp.tile_y, temp.tile_width, temp.tile_height, temp.file_name, temp.file_index);
        }
        printf("NUMBER OF RECORDS:%ld\n", num_of_elements);
        MPI_File_close(&read_fh);
    }
    MPI_Type_free(&MPI_fs_results);


}



