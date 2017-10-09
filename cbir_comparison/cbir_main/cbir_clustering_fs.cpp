
#include "cbir.h"

using namespace cv;

double cbir_cluster_fs_workflow(long long start_offset,
                             long long end_offset,
                             long long num_tiles,
                             char * file_path,
                             float Beta,
                             int level_index,
                             unsigned char *query_image_buffer,
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
    double total_final_fs_read_time = 0;
    double read_time = 0;
    
    if(num_tiles > 0)
    {
        
        MPI_File read_fh;
        MPI_Status status;
        MPI_Offset current_offset;
        MPI_File_open(MPI_COMM_SELF, file_path, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        MPI_File_seek(read_fh, start_offset, MPI_SEEK_SET);
        
        vector<vector<result_distance_t> > vectors;
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
                    vectors.push_back(temp_vector);
                    vectors_size += temp_vector.size() * data_type_size;
                    temp_vector.clear();
                    if(vectors_size > CLUSTERING_MEMORY_SIZE)
                    {
                        
                        ///start clustering and fine searching
                        vector<result_distance_t> clustering_returned_results;
                        tileCBIR_cluster_omp(vectors, query_image_height, query_image_width, Beta, clustering_returned_results);
                        for(int i = 0;  i< vectors.size(); i++)
                            vectors[i].clear();
                        vectors.clear();
                        vectors_size = 0;
                        //printf("MEmory is full\n");
                        
                        std::sort(clustering_returned_results.begin(),clustering_returned_results.end(),compare_patch_tile_ID_asc);  // this sort operation can use insertion sort
                        for(int i = 0 ; i < clustering_returned_results.size(); i++)
                            printf("CL:%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\t%d\n", clustering_returned_results[i].tile_ID,  clustering_returned_results[i].iPatX,  clustering_returned_results[i].iPatY,  clustering_returned_results[i].dis,  clustering_returned_results[i].tile_x,  clustering_returned_results[i].tile_y,  clustering_returned_results[i].tile_width,  clustering_returned_results[i].tile_height,  clustering_returned_results[i].file_name, clustering_returned_results[i].file_index);
                        
                        vector<result_distance_t> final_fs_returned_results;
                        
                        
                        read_time = fine_searching_after_clustering(clustering_returned_results,
                                                        level_index,
                                                        query_image_buffer,
                                                        query_image_height,
                                                        query_image_width,
                                                        OLP,
                                                        KBINS,
                                                        FEA_INDEX,
                                                        final_fs_returned_results,
                                                        size_width_height);
                        total_final_fs_read_time += read_time;
                       
                       /* fine_searching_after_clustering_dataspaces(clustering_returned_results,
                                                                   level_index,
                                                                   query_image_buffer,
                                                                   query_image_height,
                                                                   query_image_width,
                                                                   OLP,
                                                                   KBINS,
                                                                   FEA_INDEX,
                                                                   final_fs_returned_results,
                                                                   size_width_height);
                        */
                        
                        clustering_returned_results.clear();
                        sort_write_vector_to_disk(final_fs_returned_results, iteration++, pid, dest_folder);
                        //for(int i = 0 ; i < final_fs_returned_results.size(); i++)
                        //    printf("FINE: %d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\n", final_fs_returned_results[i].tile_ID,  final_fs_returned_results[i].iPatX,  final_fs_returned_results[i].iPatY,  final_fs_returned_results[i].dis,  final_fs_returned_results[i].tile_x,  final_fs_returned_results[i].tile_y,  final_fs_returned_results[i].tile_width,  final_fs_returned_results[i].tile_height,  final_fs_returned_results[i].file_name);
                        final_fs_returned_results.clear();
                    }
                    current_tile_ID = temp_record.tile_ID;
                }
                
                
                temp_vector.push_back(temp_record);
                
                
                
            }
            if(temp_vector.size()>0)//meaning there is only 1 tile and it needs to be clustered
            {
              
                    vectors.push_back(temp_vector);
                    temp_vector.clear();
              
            }
            if(vectors.size()>0)//memory limit has never been breached
            {
                vector<result_distance_t> clustering_returned_results;
                tileCBIR_cluster_omp(vectors, query_image_height, query_image_width, Beta, clustering_returned_results);
                for(int i = 0;  i< vectors.size(); i++)
                    vectors[i].clear();
                vectors.clear();
                
                
                std::sort(clustering_returned_results.begin(),clustering_returned_results.end(),compare_patch_tile_ID_asc);
                for(int i = 0 ; i < clustering_returned_results.size(); i++)
                    printf("CL:%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\t%d\n", clustering_returned_results[i].tile_ID,  clustering_returned_results[i].iPatX,  clustering_returned_results[i].iPatY,  clustering_returned_results[i].dis,  clustering_returned_results[i].tile_x,  clustering_returned_results[i].tile_y,  clustering_returned_results[i].tile_width,  clustering_returned_results[i].tile_height,  clustering_returned_results[i].file_name, clustering_returned_results[i].file_index);
                
                vector<result_distance_t> final_fs_returned_results;
                read_time = fine_searching_after_clustering(clustering_returned_results,
                                                level_index,
                                                query_image_buffer,
                                                query_image_height,
                                                query_image_width,
                                                OLP,
                                                KBINS,
                                                FEA_INDEX,
                                                final_fs_returned_results, size_width_height);
                total_final_fs_read_time += read_time;
                 /*
                fine_searching_after_clustering_dataspaces(clustering_returned_results,
                                                           level_index,
                                                           query_image_buffer,
                                                           query_image_height,
                                                           query_image_width,
                                                           OLP,
                                                           KBINS,
                                                           FEA_INDEX,
                                                           final_fs_returned_results,
                                                           size_width_height);
                  
                  */
                
                 clustering_returned_results.clear();
                sort_write_vector_to_disk(final_fs_returned_results, iteration++, pid, dest_folder);
               // for(int i = 0 ; i < final_fs_returned_results.size() ; i++)
               //     printf("FINE: %d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\n", final_fs_returned_results[i].tile_ID,  final_fs_returned_results[i].iPatX,  final_fs_returned_results[i].iPatY,  final_fs_returned_results[i].dis,  final_fs_returned_results[i].tile_x,  final_fs_returned_results[i].tile_y,  final_fs_returned_results[i].tile_width,  final_fs_returned_results[i].tile_height,  final_fs_returned_results[i].file_name);
                final_fs_returned_results.clear();
                
                
            }
            
            
        }else{
            vectors.push_back(temp_vector);
            temp_vector.clear();
            //start clustering and fine searching
            vector<result_distance_t> clustering_returned_results;
            tileCBIR_cluster_omp(vectors, query_image_height, query_image_width, Beta, clustering_returned_results);
            for(int i = 0;  i< vectors.size(); i++)
                vectors[i].clear();
            vectors.clear();
            
            std::sort(clustering_returned_results.begin(),clustering_returned_results.end(),compare_patch_tile_ID_asc);
            for(int i = 0 ; i < clustering_returned_results.size(); i++)
               printf("CL:%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\t%d\n", clustering_returned_results[i].tile_ID,  clustering_returned_results[i].iPatX,  clustering_returned_results[i].iPatY,  clustering_returned_results[i].dis,  clustering_returned_results[i].tile_x,  clustering_returned_results[i].tile_y,  clustering_returned_results[i].tile_width,  clustering_returned_results[i].tile_height,  clustering_returned_results[i].file_name, clustering_returned_results[i].file_index);
            
            vector<result_distance_t> final_fs_returned_results;
            read_time = fine_searching_after_clustering(clustering_returned_results,
                                            level_index,
                                            query_image_buffer,
                                            query_image_height,
                                            query_image_width,
                                            OLP,
                                            KBINS,
                                            FEA_INDEX,
                                            final_fs_returned_results,
                                            size_width_height);
            total_final_fs_read_time += read_time;
            /*
            fine_searching_after_clustering_dataspaces(clustering_returned_results,
                                                       level_index,
                                                       query_image_buffer,
                                                       query_image_height,
                                                       query_image_width,
                                                       OLP,
                                                       KBINS,
                                                       FEA_INDEX,
                                                       final_fs_returned_results,
                                                       size_width_height);

            */
             clustering_returned_results.clear();
            sort_write_vector_to_disk(final_fs_returned_results, iteration++, pid, dest_folder);
            //for(int i = 0 ; i < final_fs_returned_results.size(); i++)
            //    printf("FINE:%d\t%d\t%d\t%f\t%ld\t%ld\t%d\t%d\t%s\n", final_fs_returned_results[i].tile_ID,  final_fs_returned_results[i].iPatX,  final_fs_returned_results[i].iPatY,  final_fs_returned_results[i].dis,  final_fs_returned_results[i].tile_x,  final_fs_returned_results[i].tile_y,  final_fs_returned_results[i].tile_width,  final_fs_returned_results[i].tile_height,  final_fs_returned_results[i].file_name);
            final_fs_returned_results.clear();
            
            
        }
        MPI_File_close(&read_fh);
        MPI_Type_free(&MPI_fs_results);// not sure if it works
    }
    return total_final_fs_read_time;
    
    
    
    
    
}


