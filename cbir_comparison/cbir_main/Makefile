ROOTDIR=/Users/eyildirim/Documents/cbir_comparison/
RS_DIR=$(ROOTDIR)/rough_searching_omp
FS_DIR=$(ROOTDIR)/fine_searching_omp
COMMON_DIR=$(ROOTDIR)/cbir_common
CLUSTER_DIR=$(ROOTDIR)/clustering
FINAL_FS_DIR=$(ROOTDIR)/final_fine_searching
WRITE_DIR=$(ROOTDIR)/write_image

#DATASPACES_DIR=/project1/parashar-001/esma/ds_install/bin
#DATASPACES_INC=-I/project1/parashar-001/esma/ds_install/include
#DATASPACES_LIB=-L/project1/parashar-001/esma/ds_install/lib -ldspaces -ldscommon -ldart -lm

#OPENJPEG_INC=-I/project1/parashar-001/esma/openjpeg/usr/local/include/openjpeg-2.1
#OPENJPEG_LIB=-L/project1/parashar-001/esma/openjpeg/usr/local/lib -lopenjp2
#OPENCV_INC=-I/project1/parashar-001/esma/opencv-release/usr/local/include
#OPENCV_LIB=-L/project1/parashar-001/esma/opencv-release/usr/local/lib/ -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect  -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_videoio -lopencv_video -lopencv_videostab
#OMP_INC=-fopenmp
#OMP_LIB=-fopenmp
#OPENSLIDE_LIB=-L/project1/parashar-001/esma/openslide-release/lib/ -lopenslide
#OPENSLIDE_INC=-I/project1/parashar-001/esma/openslide-release/include/openslide
#GLIB_INC=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include
#LIBTIFF_LIB=-L/project1/parashar-001/esma/tiff-release2/usr/local/lib -ltiff

OPENCV_INC=-I/usr/local/Cellar/opencv/2.4.12/include/opencv -I/usr/local/Cellar/opencv/2.4.12/include
OPENCV_LIB=-L/usr/local/Cellar/opencv/2.4.12/lib -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
OMP_LIB=-L/usr/local/Cellar/libiomp/20150701/lib/ -liomp5
OMP_INC=-I/usr/local/Cellar/libiomp/20150701/include/libiomp/
#OMP_LIB=-fopenmp
OPENSLIDE_LIB=-L/usr/local/lib -lopenslide
OPENSLIDE_INC=-I/usr/local/include/openslide
GLIB_INC=-I/usr/local/Cellar/glib/2.44.1/include/glib-2.0 -I/usr/local/Cellar/glib/2.44.1/lib/glib-2.0/include -I/usr/local/opt/gettext/include

DATASPACES_LIB=-L/Users/eyildirim/Documents/ds_install/lib -ldspaces -ldscommon -ldart -lm
DATASPACES_INC=-I/Users/eyildirim/Documents/ds_install/include

CC=mpicxx

all: executable  





executable:cbir_fs_after_rs.o cbir_clustering_fs.o svs_reader_stage1_read_rs_fs_sort.o $(WRITE_DIR)/write_image.o $(FINAL_FS_DIR)/final_fs.o $(COMMON_DIR)/glcm_f.o $(COMMON_DIR)/mpi_utility.o $(COMMON_DIR)/mpi_sort_fs_results.o $(COMMON_DIR)/fea_HAH.o $(COMMON_DIR)/patchDis.o $(COMMON_DIR)/split_fun_c.o $(COMMON_DIR)/sort_fun.o $(RS_DIR)/hah_module_parallel.o $(RS_DIR)/module_roughSearching_2.o $(RS_DIR)/tileCBIR_roughS_omp_parallel.o $(FS_DIR)/segDis2.o $(FS_DIR)/para_feaDis2.o $(FS_DIR)/tileCBIR_fineS_omp.o $(CLUSTER_DIR)/tileCBIR_cluster_omp.o $(CLUSTER_DIR)/Meanshift_fun.o
	$(CC) -o $@ $^ $(DATASPACES_LIB) $(OPENCV_LIB) $(OMP_LIB) $(OPENSLIDE_LIB) $(OPENJPEG_LIB) $(LIBTIFF_LIB) 

#executable_cl_fs_sort:cbir_clustering_fs.o svs_reader_cl_fs_sort.o $(WRITE_DIR)/write_image.o $(FINAL_FS_DIR)/final_fs.o $(COMMON_DIR)/mpi_utility.o $(COMMON_DIR)/mpi_sort_fs_results.o $(COMMON_DIR)/fea_HAH.o $(COMMON_DIR)/patchDis.o $(COMMON_DIR)/split_fun_c.o $(COMMON_DIR)/sort_fun.o $(RS_DIR)/hah_module_parallel.o $(RS_DIR)/module_roughSearching_2.o $(RS_DIR)/tileCBIR_roughS_omp_parallel.o $(FS_DIR)/segDis2.o $(FS_DIR)/para_feaDis2.o $(FS_DIR)/tileCBIR_fineS_omp.o $(CLUSTER_DIR)/tileCBIR_cluster_omp.o $(CLUSTER_DIR)/Meanshift_fun.o
#	$(CC) -o $@ $^ $(DATASPACES_LIB) $(OPENCV_LIB) $(OMP_LIB) $(OPENSLIDE_LIB) $(OPENJPEG_LIB) $(LIBTIFF_LIB)


svs_reader_solo.o:svs_reader_solo.cpp
	$(CC) -c svs_reader_solo.cpp $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_v2.o:svs_reader_v2.cpp 
	$(CC) -c svs_reader_v2.cpp $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_stage1_read.o:svs_reader_stage1_read.cpp 
	$(CC) -c svs_reader_stage1_read.cpp $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_stage1_read_rs.o:svs_reader_stage1_read_rs.cpp
	$(CC) -c svs_reader_stage1_read_rs.cpp $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_stage1_read_rs_sort.o:svs_reader_stage1_read_rs_sort.cpp 
	$(CC) -c svs_reader_stage1_read_rs_sort.cpp $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_stage1_read_rs_fs_sort.o:svs_reader_stage1_read_rs_fs_sort.cpp 
	$(CC) -c svs_reader_stage1_read_rs_fs_sort.cpp $(DATASPACES_INC) $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_stage1_read_rs_fs_sort_cl_fs_sort.o:svs_reader_stage1_read_rs_fs_sort_cl_fs_sort.cpp 
	$(CC) -c svs_reader_stage1_read_rs_fs_sort_cl_fs_sort.cpp $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)

svs_reader_cl_fs_sort.o:svs_reader_cl_fs_sort.cpp 
	$(CC) -c svs_reader_cl_fs_sort.cpp $(DATASPACES_INC) $(GLIB_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC)




cbir_fs_after_rs.o:cbir_fs_after_rs.cpp	
	$(CC) -c cbir_fs_after_rs.cpp $(DATASPACES_INC) $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC) $(GLIB_INC)

cbir_clustering_fs.o:cbir_clustering_fs.cpp	
	$(CC) -c cbir_clustering_fs.cpp $(OMP_INC) $(OPENCV_INC) $(OPENSLIDE_INC) $(GLIB_INC)


.PHONY:clean

clean:
	rm -f *.o


