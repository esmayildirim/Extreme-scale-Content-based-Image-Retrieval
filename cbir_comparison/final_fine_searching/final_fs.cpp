
#include "final_fs.h"
#include <assert.h>
#include <mpi.h>
#include "dataspaces.h"


double fine_searching_after_clustering(vector<result_distance_t> &clustered_results,
                                     int level_index,
                                     unsigned char * queryBuf_ptr,
                                     int query_imH,
                                     int query_imW,
                                     float OLP,
                                     int KBINS,
                                     int Index_FEA,
                                     vector<result_distance_t>&final_vector,
                                     int64_t ***size_width_height)
{
    double total_final_fs_read_time = 0;
    double read_start, read_end;
    
    int current_tile_ID = clustered_results[0].tile_ID;
    int number_of_tiles = 1;
    for(int i= 1; i< clustered_results.size(); i++)
        if(clustered_results[i].tile_ID != current_tile_ID)
        {
            number_of_tiles++;
            current_tile_ID = clustered_results[i].tile_ID;
        }
    //divide the vector based on tile numbers
    
    vector< vector<result_distance_t> > vectors;
    int k = 0;
    for(int i = 0; i< number_of_tiles; i++ )
    {
        vector<result_distance_t> temp_vector;
        do{
            temp_vector.push_back(clustered_results[k++]);
            
        }while(k < clustered_results.size() && clustered_results[k].tile_ID == clustered_results[k-1].tile_ID);
        vectors.push_back(temp_vector);
        //printf("TEMP VeCTOR sizes %lu\n", temp_vector.size());
    }
    //printf("MY number of tiles is %d\n", number_of_tiles);
    for (int i = 0;  i < number_of_tiles; i++) {
   
     
        //bring the tile into memory and convert to Mat object
        int tile_x = vectors[i][0].tile_x;
        int tile_y = vectors[i][0].tile_y;
        int tile_width = vectors[i][0].tile_width;
        int tile_height = vectors[i][0].tile_height;
    
        read_start = MPI_Wtime();
        uint32_t * buffer = NULL;
        openslide_t *osr = openslide_open(vectors[i][0].file_name);
        int64_t num_bytes = tile_width * tile_height * 4;
        buffer = (uint32_t *)malloc(num_bytes);
        int level_factor = size_width_height[vectors[i][0].file_index][0][0]/size_width_height[vectors[i][0].file_index][level_index][0];
        
        tile_x *= level_factor;
        tile_y *= level_factor;
        openslide_read_region(osr, buffer, tile_x, tile_y, level_index, tile_width, tile_height);
        assert(openslide_get_error(osr) == NULL);
        openslide_close(osr);
        
        read_end = MPI_Wtime();
        total_final_fs_read_time += read_end - read_start;
        //printf("size of buffer %ld\n",sizeof(buffer));
        unsigned char *char_buffer = (unsigned char *)buffer;
        Mat QueryImg, DataImg;
        DataImg = Mat( tile_height, tile_width, CV_8UC3 );
        QueryImg = Mat( query_imH, query_imW, CV_8UC3 );
 
        //printf("TILE WIDTH %d TILE HEIGHT %d\n", tile_width, tile_height);
        for( int j=0; j<tile_height; j++ ){
            for( int z=0; z<tile_width; z++ ){
                for( int iChn=0; iChn<3; iChn++ ){
                    //printf("BUFFER INDEX %lld value %lld\n",j*tile_width*4+z*4+(3-iChn),buffer[j*tile_width*4+z*4+(3-iChn)]);
                    DataImg.at<unsigned char>(j,3*z+iChn) = char_buffer[j*tile_width*4+z*4+(iChn)];
                }//END_FOR_iCh
            }//END_FOR_I
        }//END_FOR_J
  
        for( int j=0; j<query_imH; j++ ){
            for( int i=0; i<query_imW; i++ ){
                for( int iChn=0; iChn<3; iChn++ ){
                    QueryImg.at<unsigned char>(j,3*i+iChn) = queryBuf_ptr[j*query_imW*4+i*4+(iChn)];
                }//END_FOR_iCh
            }//END_FOR_I
        }//END_FOR_J
 
        //imshow("DW1", DataImg);
        //imshow("DW2", QueryImg);
        //waitKey(0);
        
        //convert the vector into array
        result_distance_t * tile_fs_results = (result_distance_t *) malloc(sizeof(result_distance_t) * vectors[i].size());
        for (int j = 0; j < vectors[i].size(); j++ ) {
            tile_fs_results[j].tile_ID = vectors[i][j].tile_ID;
            tile_fs_results[j].iPatX = vectors[i][j].iPatX;
            tile_fs_results[j].iPatY = vectors[i][j].iPatY;
            tile_fs_results[j].tile_x = vectors[i][j].tile_x;
            tile_fs_results[j].tile_y = vectors[i][j].tile_y;
            tile_fs_results[j].tile_height = vectors[i][j].tile_height;
            tile_fs_results[j].tile_width = vectors[i][j].tile_width;
            tile_fs_results[j].file_index = vectors[i][j].file_index;
            memcpy(tile_fs_results[j].file_name,  vectors[i][j].file_name, 200);
            
        }
        
    
        //apply fine searching
        
        tileCBIR_fineS_omp(final_vector,
                           vectors[i].size(),
                           tile_fs_results,
                           DataImg,
                           tile_height,
                           tile_width,
                           QueryImg,
                           query_imH,
                           query_imW,
                           tile_fs_results[0].tile_ID,
                           OLP,
                           KBINS,
                           Index_FEA );
        DataImg.release();
        QueryImg.release();
        free(tile_fs_results);
        
        free(buffer);
    }

    return total_final_fs_read_time;
    
}
