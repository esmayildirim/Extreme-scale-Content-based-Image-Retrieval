
#include "cbir.h"


#include "dataspaces.h"


#define FS_MEMORY_SIZE 200000000
using namespace cv;

double cbir_fs_after_rs_workflow(long long start_offset,
                              long long end_offset,
                              long long num_tiles,
                              char * file_path,
                              int level_index,
                              Mat QueryImg,
                              int query_image_height,
                              int query_image_width,
                              float OLP,
                              int KBINS,
                              int FEA_INDEX,
                              char * dest_folder,
                              int pid,
                              int64_t *** size_width_height
                              )
{
    vector<result_distance_t> fs_total_vector;
    double total_fs_read_time = 0;
    double read_start, read_end;
    if(num_tiles > 0)
    {
        
        MPI_File read_fh;
        MPI_Status status;
        MPI_Offset current_offset;
        MPI_File_open(MPI_COMM_SELF, file_path, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        MPI_File_seek(read_fh, start_offset, MPI_SEEK_SET);

        vector<result_distance_t> temp_vector;
        
        result_distance_t temp;
        MPI_Datatype MPI_fs_results;
        MPI_Datatype type[10] = {MPI_INT,MPI_INT,MPI_INT,MPI_CHAR,MPI_FLOAT,MPI_LONG, MPI_LONG,MPI_INT,MPI_INT,MPI_INT};
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
        
        result_distance_t temp_record;
        
        MPI_File_read(read_fh, &temp_record, 1, MPI_fs_results, &status);
        int current_tile_ID = temp_record.tile_ID;
        temp_vector.push_back(temp_record);
        MPI_File_get_position(read_fh, &current_offset);
        //printf("current offset: %lld\n", current_offset);
        long long vectors_size = 0;
        long iteration = 0;
        if(current_offset < end_offset+1){
            
            while(current_offset < end_offset+1)
            {
                MPI_File_read(read_fh, &temp_record, 1, MPI_fs_results, &status);
                MPI_File_get_position(read_fh, &current_offset);
                if(temp_record.tile_ID != current_tile_ID)
                {
                    
                    read_start = MPI_Wtime();
                    uint32_t *buffer = (uint32_t *)malloc(temp_vector[0].tile_height * temp_vector[0].tile_width * sizeof(uint32_t));
                    openslide_t *osr = openslide_open(temp_vector[0].file_name);
                    int level_factor = size_width_height[temp_vector[0].file_index][0][0]/ size_width_height[temp_vector[0].file_index][level_index][0];
                    
                    openslide_read_region(osr, buffer, temp_vector[0].tile_x * level_factor, temp_vector[0].tile_y * level_factor,level_index, temp_vector[0].tile_width, temp_vector[0].tile_height);
                    assert(openslide_get_error(osr) == NULL);
                    openslide_close(osr);
                    read_end = MPI_Wtime();
                    total_fs_read_time += read_end - read_start;
                     
                     
                     
                    unsigned char *char_buffer = (unsigned char *)buffer;
                    Mat DataImg = Mat( temp_vector[0].tile_height, temp_vector[0].tile_width, CV_8UC3 );
                    for( int j=0; j<temp_vector[0].tile_height; j++ ){
                        for( int z=0; z<temp_vector[0].tile_width; z++ ){
                            for( int iChn=0; iChn<3; iChn++ ){
                                //printf("BUFFER INDEX %lld value %lld\n",j*tile_width*4+z*4+(3-iChn),buffer[j*tile_width*4+z*4+(3-iChn)]);
                                DataImg.at<unsigned char>(j,3*z+iChn) = char_buffer[j*temp_vector[0].tile_width*4+z*4+(iChn)];
                            }//END_FOR_iCh
                        }//END_FOR_I
                    }//END_FOR_J
                    
                    free(buffer);
                    
                    result_distance_t * tile_fs_results = (result_distance_t *) malloc(sizeof(result_distance_t) * temp_vector.size());
                    for (int j = 0; j < temp_vector.size(); j++ ) {
                        tile_fs_results[j].tile_ID = temp_vector[j].tile_ID;
                        tile_fs_results[j].iPatX = temp_vector[j].iPatX;
                        tile_fs_results[j].iPatY = temp_vector[j].iPatY;
                        tile_fs_results[j].tile_x = temp_vector[j].tile_x;
                        tile_fs_results[j].tile_y = temp_vector[j].tile_y;
                        tile_fs_results[j].tile_height = temp_vector[j].tile_height;
                        tile_fs_results[j].tile_width = temp_vector[j].tile_width;
                        tile_fs_results[j].file_index = temp_vector[j].file_index;
                        memcpy(tile_fs_results[j].file_name,  temp_vector[j].file_name, 200);
                        
                    }
                    
                    vector<result_distance_t> final_fs_returned_results;
                    tileCBIR_fineS_omp(final_fs_returned_results,
                                       temp_vector.size(),
                                       tile_fs_results,
                                       DataImg,
                                       temp_vector[0].tile_height,
                                       temp_vector[0].tile_width,
                                       QueryImg,
                                       query_image_height,
                                       query_image_width,
                                       tile_fs_results[0].tile_ID,
                                       OLP,
                                       KBINS,
                                       FEA_INDEX );
                    temp_vector.clear();
                    
                    fs_total_vector.insert(fs_total_vector.end(), final_fs_returned_results.begin(), final_fs_returned_results.end());
                    
                    long long current_vector_size_in_bytes = fs_total_vector.size() * sizeof(result_distance_t);
                    if(current_vector_size_in_bytes > FS_MEMORY_SIZE)
                    {
                         sort_write_vector_to_disk(fs_total_vector, fs_total_vector[0].tile_ID, pid, dest_folder);
                         fs_total_vector.clear();
                    }
                    current_tile_ID = temp_record.tile_ID;
                    free(tile_fs_results);
                    final_fs_returned_results.clear();
                    
                    
                }//end of if
                
                
                temp_vector.push_back(temp_record);
                
                
                
            }//end of while
            if(temp_vector.size()> 0)
            {
                read_start = MPI_Wtime();
                uint32_t * buffer = (uint32_t *) malloc(sizeof(uint32_t) * temp_vector[0].tile_height * temp_vector[0].tile_width);
                openslide_t *osr = openslide_open(temp_vector[0].file_name);
                int level_factor = size_width_height[temp_vector[0].file_index][0][0]/ size_width_height[temp_vector[0].file_index][level_index][0];
                
                openslide_read_region(osr, buffer, temp_vector[0].tile_x * level_factor, temp_vector[0].tile_y * level_factor, level_index, temp_vector[0].tile_width, temp_vector[0].tile_height);
                assert(openslide_get_error(osr) == NULL);
                openslide_close(osr);
                read_end = MPI_Wtime();
                total_fs_read_time += read_end - read_start;
                
                unsigned char *char_buffer = (unsigned char *)buffer;
                Mat DataImg = Mat( temp_vector[0].tile_height, temp_vector[0].tile_width, CV_8UC3 );
                for( int j=0; j<temp_vector[0].tile_height; j++ ){
                    for( int z=0; z<temp_vector[0].tile_width; z++ ){
                        for( int iChn=0; iChn<3; iChn++ ){
                            //printf("BUFFER INDEX %lld value %lld\n",j*tile_width*4+z*4+(3-iChn),buffer[j*tile_width*4+z*4+(3-iChn)]);
                            DataImg.at<unsigned char>(j,3*z+iChn) = char_buffer[j*temp_vector[0].tile_width*4+z*4+(iChn)];
                        }//END_FOR_iCh
                    }//END_FOR_I
                }//END_FOR_J
                
                free(buffer);
                result_distance_t * tile_fs_results = (result_distance_t *) malloc(sizeof(result_distance_t) * temp_vector.size());
                for (int j = 0; j < temp_vector.size(); j++ ) {
                    tile_fs_results[j].tile_ID = temp_vector[j].tile_ID;
                    tile_fs_results[j].iPatX = temp_vector[j].iPatX;
                    tile_fs_results[j].iPatY = temp_vector[j].iPatY;
                    tile_fs_results[j].tile_x = temp_vector[j].tile_x;
                    tile_fs_results[j].tile_y = temp_vector[j].tile_y;
                    tile_fs_results[j].tile_height = temp_vector[j].tile_height;
                    tile_fs_results[j].tile_width = temp_vector[j].tile_width;
                    tile_fs_results[j].file_index = temp_vector[j].file_index;
                    memcpy(tile_fs_results[j].file_name,  temp_vector[j].file_name, 200);
                    
                }
                
                vector<result_distance_t> final_fs_returned_results;
                tileCBIR_fineS_omp(final_fs_returned_results,
                                   temp_vector.size(),
                                   tile_fs_results,
                                   DataImg,
                                   temp_vector[0].tile_height,
                                   temp_vector[0].tile_width,
                                   QueryImg,
                                   query_image_height,
                                   query_image_width,
                                   tile_fs_results[0].tile_ID,
                                   OLP,
                                   KBINS,
                                   FEA_INDEX );
                temp_vector.clear();
                free(tile_fs_results);
                fs_total_vector.insert(fs_total_vector.end(), final_fs_returned_results.begin(), final_fs_returned_results.end());
                
                sort_write_vector_to_disk(fs_total_vector, fs_total_vector[0].tile_ID, pid, dest_folder);
                fs_total_vector.clear();
             
                
                
            }
        }//end of if
        else{
           
            read_start = MPI_Wtime();
            uint32_t * buffer = (uint32_t *) malloc(sizeof(uint32_t) * temp_vector[0].tile_height * temp_vector[0].tile_width);
            openslide_t *osr = openslide_open(temp_vector[0].file_name);
            int level_factor = size_width_height[temp_vector[0].file_index][0][0]/ size_width_height[temp_vector[0].file_index][level_index][0];
            
            openslide_read_region(osr, buffer, temp_vector[0].tile_x * level_factor, temp_vector[0].tile_y * level_factor, level_index, temp_vector[0].tile_width, temp_vector[0].tile_height);
            assert(openslide_get_error(osr) == NULL);
            openslide_close(osr);
            read_end = MPI_Wtime();
            total_fs_read_time += read_end - read_start;
            
            
            unsigned char *char_buffer = (unsigned char *)buffer;
            Mat DataImg = Mat( temp_vector[0].tile_height, temp_vector[0].tile_width, CV_8UC3 );
            for( int j=0; j<temp_vector[0].tile_height; j++ ){
                for( int z=0; z<temp_vector[0].tile_width; z++ ){
                    for( int iChn=0; iChn<3; iChn++ ){
                        //printf("BUFFER INDEX %lld value %lld\n",j*tile_width*4+z*4+(3-iChn),buffer[j*tile_width*4+z*4+(3-iChn)]);
                        DataImg.at<unsigned char>(j,3*z+iChn) = char_buffer[j*temp_vector[0].tile_width*4+z*4+(iChn)];
                    }//END_FOR_iCh
                }//END_FOR_I
            }//END_FOR_J
            
            free(buffer);
            result_distance_t * tile_fs_results = (result_distance_t *) malloc(sizeof(result_distance_t) * temp_vector.size());
            for (int j = 0; j < temp_vector.size(); j++ ) {
                tile_fs_results[j].tile_ID = temp_vector[j].tile_ID;
                tile_fs_results[j].iPatX = temp_vector[j].iPatX;
                tile_fs_results[j].iPatY = temp_vector[j].iPatY;
                tile_fs_results[j].tile_x = temp_vector[j].tile_x;
                tile_fs_results[j].tile_y = temp_vector[j].tile_y;
                tile_fs_results[j].tile_height = temp_vector[j].tile_height;
                tile_fs_results[j].tile_width = temp_vector[j].tile_width;
                tile_fs_results[j].file_index = temp_vector[j].file_index;
                memcpy(tile_fs_results[j].file_name,  temp_vector[j].file_name, 200);
                
            }
            
            vector<result_distance_t> final_fs_returned_results;
            tileCBIR_fineS_omp(final_fs_returned_results,
                               temp_vector.size(),
                               tile_fs_results,
                               DataImg,
                               temp_vector[0].tile_height,
                               temp_vector[0].tile_width,
                               QueryImg,
                               query_image_height,
                               query_image_width,
                               tile_fs_results[0].tile_ID,
                               OLP,
                               KBINS,
                               FEA_INDEX);
            temp_vector.clear();
            free(tile_fs_results);
            fs_total_vector.insert(fs_total_vector.end(), final_fs_returned_results.begin(), final_fs_returned_results.end());
            
            sort_write_vector_to_disk(fs_total_vector, fs_total_vector[0].tile_ID, pid, dest_folder);
            fs_total_vector.clear();
            
        
            
        }
        
        
        MPI_File_close(&read_fh);
        MPI_Type_free(&MPI_fs_results);// not sure if it works
        
    }
    return total_fs_read_time;
    
    
    
    
    
}

