
#ROOTDIR=/home1/esma/cbir_12_07_2015

#OPENCV_INC=-I/project1/parashar-001/esma/opencv-release/usr/local/include
#OPENCV_LIB=-L/project1/parashar-001/esma/opencv-release/usr/local/lib/ -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect  -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
#OMP_INC=-fopenmp
#OMP_LIB=-fopenmp
#OPENSLIDE_LIB=-L/project1/parashar-001/esma/openslide/lib/ -lopenslide
#OPENSLIDE_INC=-I/project1/parashar-001/esma/openslide/include/openslide
#GLIB_INC=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include

OPENCV_INC=-I/usr/local/Cellar/opencv/2.4.12/include/opencv -I/usr/local/Cellar/opencv/2.4.12/include
OPENCV_LIB=-L/usr/local/Cellar/opencv/2.4.12/lib -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
OMP_LIB=-L/usr/local/Cellar/libiomp/20150701/lib/ -liomp5
OMP_INC=-I/usr/local/Cellar/libiomp/20150701/include/libiomp/
#OMP_LIB=-fopenmp
OPENSLIDE_LIB=-L/usr/local/lib -lopenslide
OPENSLIDE_INC=-I/usr/local/include/openslide
GLIB_INC=-I/usr/local/Cellar/glib/2.44.1/include/glib-2.0 -I/usr/local/Cellar/glib/2.44.1/lib/glib-2.0/include -I/usr/local/opt/gettext/include



CC=mpicxx
all:hah_module_parallel.o module_roughSearching_2.o tileCBIR_roughS_omp_parallel.o 

hah_module_parallel.o:hah_module_parallel.cpp
	$(CC) -c hah_module_parallel.cpp $(OPENCV_INC) $(OMP_INC)

module_roughSearching_2.o:module_roughSearching_2.cpp
	$(CC) -c module_roughSearching_2.cpp $(OMP_INC) $(OPENCV_INC)

tileCBIR_roughS_omp_parallel.o:tileCBIR_roughS_omp_parallel.cpp	
	$(CC) -c tileCBIR_roughS_omp_parallel.cpp $(OMP_INC) $(OPENCV_INC)


.PHONY:clean

clean:
	rm -f *.o


