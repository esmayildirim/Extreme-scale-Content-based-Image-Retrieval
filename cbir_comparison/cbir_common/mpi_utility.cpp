
#include "mpi_utility.h"


void get_tile_record_positions(long long &start_offset, long long &end_offset, long long &my_number_of_tiles, int pid, char * file_path, int num_of_procs)
{
    long long * offset_start_array;
    long long * offset_end_array;
    long long * number_of_tiles;
    if(pid == 0)
    {
        long long num_of_total_tiles = 0;
        vector<long long> position_start, position_end;
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
        
        
        long long current_offset = 0;
        long long my_current_offset = 0;
        long long file_size;
        
        MPI_File read_fh;
        MPI_Status status;
        MPI_File_open(MPI_COMM_SELF, file_path, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        MPI_File_get_size(read_fh, &file_size);
        long long number_of_records = file_size/data_type_size;
        
        
        int current_tile_ID;
        MPI_File_seek(read_fh,0 + disp[2], MPI_SEEK_SET);
        MPI_File_read(read_fh, &current_tile_ID, 1, MPI_INT, &status);
        printf("CURRENT TILEID:%d\n", current_tile_ID);
        num_of_total_tiles = 1;
        position_start.push_back(0);
        
        for(int i = 1; i< number_of_records; i++)
        {   int temp_tile_ID;
            MPI_File_seek(read_fh, i *data_type_size + disp[2], MPI_SEEK_SET);
            MPI_File_read(read_fh, &temp_tile_ID, 1, MPI_INT, &status);
            if(temp_tile_ID !=current_tile_ID)
            {
                position_end.push_back(i * data_type_size -1);
                position_start.push_back(i*data_type_size);
                num_of_total_tiles ++;
                current_tile_ID = temp_tile_ID;
                printf("CURRENT TILEID:%d\n", current_tile_ID);
            }
        }
        position_end.push_back(number_of_records*data_type_size -1);
        printf("%lld %lld %lld\n", position_start.size(), position_end.size(), num_of_total_tiles);
        MPI_File_close(&read_fh);
        MPI_Type_free(&MPI_fs_results);
        
        offset_start_array = (long long *)malloc(num_of_procs * sizeof(long long));
        offset_end_array = (long long *)malloc(num_of_procs * sizeof(long long));
        number_of_tiles = (long long *)malloc(num_of_procs * sizeof(long long));
        int proc_no = 0;
        long records_per_p = number_of_records / num_of_procs;
        int remainder = number_of_records % num_of_procs;
        if(remainder>0) records_per_p++;
        
        for(int i = 0 ; i < num_of_total_tiles;)
        {
            offset_start_array[proc_no] = position_start[i];
            number_of_tiles[proc_no] = 0;
            do
            {
                offset_end_array[proc_no] = position_end[i];
                number_of_tiles[proc_no]++;
                i++;
                
            }while(offset_end_array[proc_no] - offset_start_array[proc_no] < records_per_p*data_type_size && i < num_of_total_tiles);
            proc_no++;
            
        }
        for(int i = proc_no; i < num_of_procs;i++)
        {
            offset_start_array[i] =0;
            offset_end_array[i] = 0;
            number_of_tiles[i] = 0;
        }
        
        //Send each process their corresponding numbers
        start_offset = offset_start_array[0];
        end_offset = offset_end_array[0];
        my_number_of_tiles = number_of_tiles[0];
        for(int i = 1; i < num_of_procs; i++)
        {
          
            MPI_Send(&offset_start_array[i], 1, MPI_LONG_LONG,i,0,MPI_COMM_WORLD);
            MPI_Send(&offset_end_array[i], 1, MPI_LONG_LONG,i,0,MPI_COMM_WORLD);
            MPI_Send(&number_of_tiles[i], 1, MPI_LONG_LONG,i,0,MPI_COMM_WORLD);
    
        }
        position_start.clear();
        position_end.clear();
        
        
    }else
    {
        long long temp_var;
        MPI_Recv(&temp_var,1,MPI_LONG_LONG,0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        start_offset = temp_var;
        MPI_Recv(&temp_var,1,MPI_LONG_LONG,0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        end_offset = temp_var;
        MPI_Recv(&temp_var,1,MPI_LONG_LONG,0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        my_number_of_tiles = temp_var;
    
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(pid ==0)
    {
        free(offset_start_array);
        free(offset_end_array);
        free(number_of_tiles);
        
    
    }
    
}







