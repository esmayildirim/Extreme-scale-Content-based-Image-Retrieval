// tileCBIR_cluster_omp.cpp
// ... 


#include "tileCBIR_cluster_omp.h"

void tileCBIR_cluster_omp( vector<vector<result_distance_t> > vectors,
                          int HeightImg,
                          int WidthImg,
                          float Beta,
                          vector<result_distance_t> &clustering_returned_results)
{
    
    int BandWidth = round( Beta * norm_fun( WidthImg/2, HeightImg/2 ) );
    //decide OpenMP thread number
    int max_node_thread_num = 24;
    int thread_num = vectors.size();
    if(thread_num > max_node_thread_num)
        thread_num = max_node_thread_num;
    omp_set_dynamic(0);
#pragma omp parallel for num_threads(thread_num)
    for(int i=0; i<vectors.size(); i++)
    {// 2. cluster
        vector<result_distance_t> pNew_temp;
        vector<int> pNew_num;
        vector<int> pNew_index;
        Meanshift_fun_detail( vectors[i], BandWidth, pNew_temp, pNew_num, pNew_index );
        
        //cout <<"[MS] pNew_temp SIZE: "<< pNew_temp.size() << endl;
        
        for(int ipn=0; ipn<pNew_temp.size(); ipn++){
            result_distance_t  r2t_temp;
            r2t_temp.tile_ID = pNew_temp[ipn].tile_ID;
            r2t_temp.iPatX = pNew_temp[ipn].iPatX;
            r2t_temp.iPatY = pNew_temp[ipn].iPatY;
            r2t_temp.tile_x = vectors[i][0].tile_x;
            r2t_temp.tile_y = vectors[i][0].tile_y;
            r2t_temp.tile_width = vectors[i][0].tile_width;
            r2t_temp.tile_height = vectors[i][0].tile_height;
            memcpy( r2t_temp.file_name, vectors[i][0].file_name, 200);
            r2t_temp.file_index = vectors[i][0].file_index;
#pragma omp critical
            {
                clustering_returned_results.push_back( r2t_temp );
            }
            
        }
        pNew_temp.clear();
        pNew_num.clear();
        pNew_index.clear();
       // vectors[i].clear();
    }
    //vectors.clear();
    
}//END tileCBIR_cluster_omp()





