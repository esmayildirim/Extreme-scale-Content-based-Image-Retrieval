#include "write_image.h"

void write_image(char * file_name, int query_width, int query_height, int pid, int level_index)
{
    if(pid == 0)
    {
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
        
        MPI_File read_fh;
        MPI_Status status;
        MPI_File_open(MPI_COMM_SELF, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &read_fh);
        for (int i = 0; i< 5; i++) {
            result_distance_t temp;
            MPI_File_read(read_fh, &temp, 1, MPI_fs_results, &status);
            
            long long tile_x = temp.tile_x;
            long long tile_y = temp.tile_y;
            int tile_width = temp.tile_width;
            int tile_height = temp.tile_height;
            
            uint32_t * buffer = NULL;
            openslide_t *osr = openslide_open(temp.file_name);
            int64_t num_bytes = tile_width * tile_height * 4;
            buffer = (uint32_t *)malloc(num_bytes);
            int64_t image_width, image_height, image_width_0, image_height_0;
            openslide_get_level_dimensions(osr, level_index, &image_width, &image_height);
            openslide_get_level_dimensions(osr, 0, &image_width_0, &image_height_0);
            int level_factor = image_width_0/image_width;
            tile_x *= level_factor;
            tile_y *= level_factor;
            openslide_read_region(osr, buffer, tile_x, tile_y, level_index, tile_width, tile_height);
            assert(openslide_get_error(osr) == NULL);
            
            unsigned char *char_buffer = reinterpret_cast<unsigned char *>(buffer);
            Mat DataImg;
            DataImg = Mat(tile_height, tile_width, CV_8UC3);
            
            for( int j=0; j<tile_height; j++ ){
                for( int z=0; z<tile_width; z++ ){
                    for( int iChn=0; iChn<3; iChn++ ){
                        DataImg.at<unsigned char>(j,3*z+iChn) = char_buffer[j*tile_width*4+z*4+(iChn)];
                        //printf("%uc ",char_buffer[j*tile_width*4+z*4+(3-iChn)]);
                        //DataImg.at<Vec3b>(j,z)[iChn] = char_buffer[j*tile_width*4+z*4+(3-iChn)];
                    }//END_FOR_iCh
                    printf("\n");
                }//END_FOR_I
            }//END_FOR_J
            //imshow( "Display window",DataImg);                   // Show our image inside it.
            
            //waitKey(0);
            cv::Rect myROI(temp.iPatX, temp.iPatY, query_width, query_height);
            cv::Mat croppedImage = DataImg(myROI);
            char result_file_path[200];
            sprintf(result_file_path, "result-image-%d.jpg",i);
            imwrite(result_file_path, croppedImage);
            
        }//end for
        MPI_File_close(&read_fh);
        MPI_Type_free(&MPI_fs_results);
    }//end if
    
    
}//end function
