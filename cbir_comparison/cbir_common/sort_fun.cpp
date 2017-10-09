// sort_fun.cpp

#include "sort_fun.h"
using namespace std;
bool compare_patch_distance_asc(result_distance_t A, result_distance_t B){
    if( A.dis < B.dis)
        return true;
    else return false;
}

bool compare_patch_tile_ID_asc(result_distance_t A, result_distance_t B){
    if(A.tile_ID < B.tile_ID)
        return true;
    else if(B.tile_ID < A.tile_ID)
        return false;
    else
    {
        if(A.iPatX < B.iPatX)
            return true;
        else if (B.iPatX < A.iPatX)
            return false;
        else
        {
            if( A.iPatY < B.iPatY)
                return true;
            else return false;
        
        }
    
    
    }
    
}

static void swap(result_distance_t &A, result_distance_t &B)
{
    result_distance_t temp;
    temp.dis = A.dis;
    temp.tile_ID = A.tile_ID;
    temp.iPatX = A.iPatX;
    temp.iPatY = A.iPatY;
    temp.tile_x = A.tile_x;
    temp.tile_y = A.tile_y;
    temp.tile_width = A.tile_width;
    temp.tile_height = A.tile_height;
    temp.file_index = A.file_index;
    memcpy(temp.file_name, A.file_name, 200);
    
    A.dis = B.dis ;
    A.tile_ID = B.tile_ID;
    A.iPatX = B.iPatX;
    A.iPatY = B.iPatY;
    A.tile_x = B.tile_x;
    A.tile_y = B.tile_y;
    A.tile_width = B.tile_width;
    A.tile_height = B.tile_height;
    A.file_index = B.file_index;
    memcpy(A.file_name, B.file_name ,200);
    
    B.dis = temp.dis ;
    B.tile_ID = temp.tile_ID;
    B.iPatX = temp.iPatX;
    B.iPatY = temp.iPatY;
    B.tile_x = temp.tile_x;
    B.tile_y = temp.tile_y;
    B.tile_width = temp.tile_width;
    B.tile_height = temp.tile_height;
    B.file_index = temp.file_index;
    memcpy(B.file_name, temp.file_name ,200);
    
    
    
}

void bubblesort_patch_distance_asc(vector<result_distance_t> &list)
{
    bool swapped = true;
    long long n = list.size();
    while(swapped)
    {
        swapped = false;
        for(int i = 1; i < n; i++)
        {
            if(list[i-1].dis > list[i].dis)
            {
                swap(list[i-1].dis, list[i].dis);
                swapped = true;
            }
        }
        n--;
    }
}

void insertionsort_patch_distance_asc(vector<result_distance_t> &list)
{
    long long n = list.size();
    for(int i = 1; i < n ; i++)
    {
        int j = i;
        while(j  > 0 && list[j-1].dis > list[j].dis )
        {
            swap(list[j],list[j-1]);
            j--;
        }
    
    }

}

void bubblesort_tile_ID_asc(vector<result_distance_t> &list)
{
    bool swapped = true;
    long long n = list.size();
    while(swapped)
    {
        swapped = false;
        for(int i = 1; i < n; i++)
        {
            if(list[i-1].dis > list[i].dis)
            {
                swap(list[i-1], list[i]);
                swapped = true;
            }
            else if(list[i-1].tile_ID == list[i].tile_ID)
            {
                if(list[i-1].iPatX > list[i].iPatX)
                {
                    swap(list[i-1], list[i]);
                    swapped = true;
                }else if(list[i-1].iPatX == list[i].iPatX)
                {
                    if(list[i-1].iPatY > list[i].iPatY)
                    {
                        swap(list[i-1], list[i]);
                        swapped = true;
                    }
                }
            }
        }
       
    }
}