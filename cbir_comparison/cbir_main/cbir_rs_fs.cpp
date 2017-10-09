
#include "cbir.h"

using namespace cv;

int cbir_rs_fs_workflow(vector<result_distance_t>& disPatch_fine_vector,
                  unsigned char* tileBuf_ptr,
                  int tile_imH,
                  int tile_imW,
                  unsigned char* queryBuf_ptr,
                  int query_imH,
                  int query_imW,
                  int tile_ID,
                  float OLP,
                  int KBINS,
                  int Index_FEA,
                  char * file_name,
                  long tile_x,
                  long tile_y,
                  int pid,
                  int num_processes)
{

    // 1. Transfer Query and Data Image Tile to Mat objects
    
    Mat QueryImg, DataImg;
    DataImg = Mat( tile_imH, tile_imW, CV_8UC3 );
    QueryImg = Mat( query_imH, query_imW, CV_8UC3 );
    
   
    for( int j=0; j<tile_imH; j++ ){
        for( int i=0; i<tile_imW; i++ ){
            for( int iChn=0; iChn<3; iChn++ ){
                DataImg.at<unsigned char>(j,3*i+iChn) = tileBuf_ptr[j*tile_imW*4+i*4+(iChn)]; //a change was made
            }//END_FOR_iCh
        }//END_FOR_I
    }//END_FOR_J
   
    for( int j=0; j<query_imH; j++ ){
        for( int i=0; i<query_imW; i++ ){
            for( int iChn=0; iChn<3; iChn++ ){
                QueryImg.at<unsigned char>(j,3*i+iChn) = queryBuf_ptr[j*query_imW*4+i*4+(iChn)]; //a change was made
            }//END_FOR_iCh
        }//END_FOR_I
    }//END_FOR_J
 
    
    //2. rough searching
    vector<result_distance_t> disPatch_rough_vector;
    tileCBIR_roughS_omp(disPatch_rough_vector,
                        DataImg,
                        tile_imH,
                        tile_imW,
                        QueryImg,
                        query_imH,
                        query_imW,
                        tile_ID,
                        OLP,
                        KBINS,
                        Index_FEA,
                        file_name );
    
    //3. sort and pick top 30 percent of the resultant distances
    double XPERCENT = 0.3;
    
    std::sort(disPatch_rough_vector.begin(), disPatch_rough_vector.end(), compare_patch_distance_asc);// since the results of only a single is sorted, it is ok to use merge sort. 
    
    long num_elements_x_percent = disPatch_rough_vector.size() * XPERCENT;
    
    result_distance_t * top_x_percent_distance_array = (result_distance_t *) malloc(num_elements_x_percent * sizeof(result_distance_t));
    for(long c = 0; c < num_elements_x_percent; c++)
    {
        
        result_distance_t temp = disPatch_rough_vector[c];
        top_x_percent_distance_array[c].iPatX = temp.iPatX;
        top_x_percent_distance_array[c].iPatY = temp.iPatY;
        top_x_percent_distance_array[c].tile_ID = temp.tile_ID;
        top_x_percent_distance_array[c].dis = temp.dis;
        memcpy(top_x_percent_distance_array[c].file_name, temp.file_name, 200);
        
    }
    
    
    //4. fine searching

    tileCBIR_fineS_omp(disPatch_fine_vector,
                       num_elements_x_percent,
                       top_x_percent_distance_array,
                       DataImg,
                       tile_imH,
                       tile_imW,
                       QueryImg,
                       query_imH,
                       query_imW,
                       tile_ID,
                       OLP,
                       KBINS,
                       Index_FEA );

    //Gather the fine searching results at the master
    for(int i=0; i< disPatch_fine_vector.size(); i++)
    {
        disPatch_fine_vector[i].tile_x = tile_x;
        disPatch_fine_vector[i].tile_y = tile_y;
        disPatch_fine_vector[i].tile_width = tile_imW;
        disPatch_fine_vector[i].tile_height = tile_imH;
    }
    free(top_x_percent_distance_array);
    return 0;





}


