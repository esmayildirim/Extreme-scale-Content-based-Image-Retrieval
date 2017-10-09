#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>
#include <glib.h>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <string.h>
#ifndef __PDIS_DEF__
#define __PDIS_DEF__
#include "PointDis_def.h"
#endif
#include "sort_fun.h"
using namespace std;


int external_parallel_sort_MPIIO(int pid,
                                 int num_processes,
                                 char *file_name,
                                 char *sort_folder_path,
                                 char *dest_file_name,
                                 void (*merge_func)(char *, char *, char *, int, int, int),
                                 bool (*compare_func)(result_distance_t, result_distance_t)
                                 );

void two_way_merge_based_on_distance(char * sorted_folder_path,
                                     char * sorted_file1,
                                     char * sorted_file2,
                                     int iteration,
                                     int pid,
                                     int phase_number);

void two_way_merge_based_on_tile_ID(char * sorted_folder_path,
                                    char * sorted_file1,
                                    char * sorted_file2,
                                    int iteration,
                                    int pid,
                                    int phase_number);

void top_x_percent_sorted_fs_results_MPIIO(float percentage,
                                           int pid,
                                           int num_processes,
                                           char * sorted_file_path,
                                           char * dest_file_path);

int external_parallel_sort_multifile_MPIIO(int pid,
                                           int num_processes,
                                           char *sort_folder_path,
                                           char *dest_file_name,
                                           void (*merge_func)(char *, char *, char *, int, int, int));

void sort_write_vector_to_disk(vector<result_distance_t> fs_results,
                               int iteration_no,
                               int pid,
                               char * write_folder);


