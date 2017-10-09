
#include "Meanshift_fun.h"


int g_fun(float x){
    int y=0;
    if(x<=1)
        y=1;
    
    return y;
}


float norm_fun(float x1, float x2){
    float y;
    y = sqrt( x1*x1 + x2*x2  );
    
    return y;
}



void UniqueIndex( vector<result_distance_t>& r2, vector<int>& tile_ID_unique, int can_num ){
    
    int flag_tile_ID_exist=0;
    
    
    for(int ie=0; ie < can_num; ie++ ){
        flag_tile_ID_exist=0;
        for(int iu=0; iu<tile_ID_unique.size(); iu++ ){
            if( r2[ie].tile_ID == tile_ID_unique[iu] ){
                flag_tile_ID_exist = 1;
                break;
            }
        }
        if(flag_tile_ID_exist==0)
            tile_ID_unique.push_back( r2[ie].tile_ID );
        
    }// END_FOR_ie
    
}// END_UniqueIndex()




void Meanshift_fun(vector<result_distance_t>& result2, vector<result_distance_t>&  pNew, int BandWidth ){
    
    // Mean Shift //i
    vector<result_distance_t>::iterator iter_result_2;
    vector<result_distance_t>::iterator iter_result_1;
    
    //    int BandWidth;
    float sumN_x, sumN_y, sumD;
    float pNew_x, pNew_y;
    int p_x, p_y;
    double dis_new;
    
    //    vector<result_distance_t> pNew;
    result_distance_t pNew_temp;
    vector<result_distance_t>::iterator pNew_iter;
    
    for( iter_result_1 = result2.begin(); iter_result_1 != result2.end(); iter_result_1++ ){
        p_x = iter_result_1 -> iPatX;  p_y = iter_result_1 -> iPatY;
        
        while(1)
        {
            sumN_x = 0; sumN_y = 0; sumD = 0;
            for( iter_result_2 = result2.begin(); iter_result_2 != result2.end(); iter_result_2++ ){
                sumN_x = sumN_x + iter_result_2->iPatX * g_fun(norm_fun( p_x - iter_result_2 -> iPatX,
                                                                        p_y - iter_result_2 -> iPatY )/BandWidth);
                
                sumN_y = sumN_y + iter_result_2->iPatY * g_fun(norm_fun( p_y - iter_result_2 -> iPatY,
                                                                        p_x - iter_result_2 -> iPatX )/BandWidth);
                
                sumD = sumD + g_fun(norm_fun( p_x - iter_result_2 -> iPatX, p_y - iter_result_2 -> iPatY )/BandWidth);
                
            }
            
            pNew_x = sumN_x/sumD;
            pNew_y = sumN_y/sumD;
            dis_new  = norm_fun( pNew_x - p_x, pNew_y - p_y );
            
            
            if( dis_new<2.0 )/*3.0*/{
                break;
            }
            else{
                p_x = pNew_x;  p_y = pNew_y;
            }
            
        }// END_For_while
        
        
        int flag_IsMember = 0;
        if( pNew.size() == 0 ){
            pNew_temp.iPatX = pNew_x;  pNew_temp.iPatY = pNew_y;  pNew_temp.tile_ID = iter_result_1 -> tile_ID;
            pNew.push_back( pNew_temp );
        }
        else{
            for( pNew_iter = pNew.begin(); pNew_iter != pNew.end(); pNew_iter++ )
            {
                
                if( sqrt( pow(pNew_x - pNew_iter->iPatX, 2) + pow( pNew_y - pNew_iter->iPatY, 2) ) < (BandWidth/1.0) ){
                    pNew_iter->iPatX = (pNew_x + pNew_iter->iPatX)/2.0;
                    pNew_iter->iPatY = (pNew_y + pNew_iter->iPatY)/2.0;
                    
                    flag_IsMember = 1;
                    break;
                }
                
            }
            if( flag_IsMember == 0 ){
                pNew_temp.iPatX = pNew_x;  pNew_temp.iPatY = pNew_y;  pNew_temp.tile_ID = iter_result_1 -> tile_ID;
                pNew.push_back( pNew_temp );
            }
        }
        
    }// END_For_iter1
    
} // END_Meanshift_fun()





void Meanshift_fun_detail(vector<result_distance_t>& result2, int BandWidth, vector<result_distance_t>& pNew, vector<int>& pNew_num, vector<int>& pNew_index ){
    // Mean Shift //i
    vector<result_distance_t>::iterator iter_result_2;
    vector<result_distance_t>::iterator iter_result_1;
    
    //    int BandWidth;
    float sumN_x, sumN_y, sumD;
    float pNew_x, pNew_y;
    int p_x, p_y;
    double dis_new;
    
    //    vector<result_distance_t> pNew;
    result_distance_t pNew_temp;
    vector<result_distance_t>::iterator pNew_iter;
    
    
    for( iter_result_1 = result2.begin(); iter_result_1 != result2.end(); iter_result_1++ ){
        p_x = iter_result_1 -> iPatX;  p_y = iter_result_1 -> iPatY;
        
        
        while(1)
        {
            sumN_x = 0; sumN_y = 0; sumD = 0;
            for( iter_result_2 = result2.begin(); iter_result_2 != result2.end(); iter_result_2++ )
            {
                sumN_x = sumN_x + iter_result_2->iPatX * g_fun(norm_fun( p_x - iter_result_2 -> iPatX, p_y - iter_result_2 -> iPatY )/BandWidth);
                
                sumN_y = sumN_y + iter_result_2->iPatY * g_fun(norm_fun( p_y - iter_result_2 -> iPatY, p_x - iter_result_2 -> iPatX )/BandWidth);
                
                sumD = sumD + g_fun(norm_fun( p_x - iter_result_2 -> iPatX, p_y - iter_result_2 -> iPatY )/BandWidth);
                
            }
            
            pNew_x = sumN_x/sumD;
            pNew_y = sumN_y/sumD;
            dis_new  = norm_fun( pNew_x - p_x, pNew_y - p_y );
            
            
            if( dis_new<2.0 )/*3.0*/{
                
                break;
            }
            else{
                p_x = pNew_x;  p_y = pNew_y;
            }
            
        }// END_For_while
        
        
        
        int flag_IsMember = 0;
        if( pNew.size() == 0 ){
            pNew_temp.iPatX = pNew_x;  pNew_temp.iPatY = pNew_y;  pNew_temp.tile_ID = iter_result_1 -> tile_ID;
            pNew.push_back( pNew_temp );
        }
        else{
            
            for( int ipt = 0; ipt<pNew.size(); ipt++ ){
                
                if( sqrt( pow(pNew_x - pNew[ipt].iPatX, 2) + pow( pNew_y - pNew[ipt].iPatY, 2) ) < (BandWidth/1.0) ){
                    pNew[ipt].iPatX = (pNew_x + pNew[ipt].iPatX)/2.0;
                    pNew[ipt].iPatY = (pNew_y + pNew[ipt].iPatY)/2.0;
                    
                    flag_IsMember = 1;
                    
                    break;
                }
            }
            
            if( flag_IsMember == 0 ){
                pNew_temp.iPatX = pNew_x;  pNew_temp.iPatY = pNew_y;  pNew_temp.tile_ID = iter_result_1 -> tile_ID;
                pNew.push_back( pNew_temp );
                
                pNew_num.push_back(1);
            }
        }
        
        
    }// END_For_iter1
    
} // END_Meanshift_fun()






void Meanshift_cluster( vector<result_distance_t>& pNew, vector<result_distance_t>& r2, vector<int>& tile_ID_unique, int can_num, int BandWidth ){
    
    
    pNew.clear();
    
    vector<result_distance_t> result2_temp;
    vector<result_distance_t> pNew_temp;
    
    
    int tile_ID_temp = r2[0].tile_ID;
    
    vector<int> pNew_num;
    vector<int> pNew_index;
    for( int iu=0; iu<tile_ID_unique.size(); iu++ ){
        result2_temp.clear();
        pNew_temp.clear();
        
        for( int ie=0; ie < can_num; ie++ ){
            if( r2[ie].tile_ID == tile_ID_unique[iu] ){
                result2_temp.push_back( r2[ie] );
            }
        }//END_For_ie
        
        
        Meanshift_fun_detail( result2_temp, BandWidth, pNew_temp, pNew_num, pNew_index );
        
        for(int ipn=0; ipn<pNew_temp.size(); ipn++ ){
            pNew.push_back( pNew_temp[ipn] );
        }
        
    }//END_For_iu
    
}// END_Meanshift_cluster()









