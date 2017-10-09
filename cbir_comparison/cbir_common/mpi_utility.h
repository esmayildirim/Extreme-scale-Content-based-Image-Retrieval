#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>

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

using namespace std;


void get_tile_record_positions(long long &start_offset,
                               long long &end_offset,
                               long long &my_number_of_tiles,
                               int pid,
                               char * file_path,
                               int num_of_procs);