#ELF CLUSTER SETTINGS
#OPENCV_INC=-I/project1/parashar-001/esma/opencv-release/usr/local/include
#OPENCV_LIB=-L/project1/parashar-001/esma/opencv-release/usr/local/lib/ -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect  -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
#OMP_LIB=-fopenmp
#OPENSLIDE_LIB=-L/project1/parashar-001/esma/openslide-release/lib/ -lopenslide 
#OPENSLIDE_INC=-I/project1/parashar-001/esma/openslide-release/include/openslide
#GLIB_INC=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include

#LOCAL SETTINGS
OPENCV_INC=-I/usr/local/Cellar/opencv/2.4.12/include/opencv -I/usr/local/Cellar/opencv/2.4.12/include
OPENCV_LIB=-L/usr/local/Cellar/opencv/2.4.12/lib -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
OMP_LIB=-L/usr/local/Cellar/libiomp/20150701/lib/ -fopenmp
OMP_INC=-I/usr/local/Cellar/libiomp/20150701/include/libiomp/
#OMP_LIB=-fopenmp
OPENSLIDE_LIB=-L/usr/local/lib -lopenslide
OPENSLIDE_INC=-I/usr/local/include/openslide
GLIB_INC=-I/usr/local/Cellar/glib/2.44.1/include/glib-2.0 -I/usr/local/Cellar/glib/2.44.1/lib/glib-2.0/include -I/usr/local/opt/gettext/include





CC=mpicxx  




all:fea_HAH.o patchDis.o split_fun_c.o sort_fun.o mpi_utility.o  mpi_sort_fs_results.o glcm_f.o

fea_HAH.o:fea_HAH.cpp
	$(CC) -c fea_HAH.cpp $(OPENCV_INC)

patchDis.o:patchDis.cpp
	$(CC) -c patchDis.cpp $(OPENCV_INC)

split_fun_c.o:split_fun_c.cpp
	$(CC) -c split_fun_c.cpp $(OPENCV_INC) 

sort_fun.o:sort_fun.cpp
	$(CC) -c sort_fun.cpp

mpi_sort_fs_results.o:mpi_sort_fs_results.cpp
	$(CC) -c mpi_sort_fs_results.cpp $(GLIB_INC)

mpi_utility.o:mpi_utility.cpp
	$(CC) -c mpi_utility.cpp 

glcm_f.o:glcm_f.cpp
	$(CC) -c glcm_f.cpp 


.PHONY:clean

clean:
	rm -f *.o


