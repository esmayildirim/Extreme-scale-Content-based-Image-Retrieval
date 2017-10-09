ROOTDIR=/home1/esma/cbir_12_07_2015

#OPENCV_INC=-I/project1/parashar-001/esma/opencv-release/usr/local/include
#OPENCV_LIB=-L/project1/parashar-001/esma/opencv-release/usr/local/lib/ -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect  -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
#OMP_INC=-fopenmp
#OMP_LIB=-fopenmp
#OPENSLIDE_LIB=-L/project1/parashar-001/esma/openslide-release/lib/ -lopenslide
#OPENSLIDE_INC=-I/project1/parashar-001/esma/openslide-release/include/openslide
#GLIB_INC=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include

OPENCV_INC=-I/usr/local/Cellar/opencv/2.4.12/include/opencv -I/usr/local/Cellar/opencv/2.4.12/include
OPENCV_LIB=-L/usr/local/Cellar/opencv/2.4.12/lib -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
OMP_LIB=-L/usr/local/Cellar/libiomp/20150701/lib/ -fopenmp
OMP_INC=-I/usr/local/Cellar/libiomp/20150701/include/libiomp/
#OMP_LIB=-fopenmp
OPENSLIDE_LIB=-L/usr/local/lib -lopenslide
OPENSLIDE_INC=-I/usr/local/include/openslide
GLIB_INC=-I/usr/local/Cellar/glib/2.44.1/include/glib-2.0 -I/usr/local/Cellar/glib/2.44.1/lib/glib-2.0/include -I/usr/local/opt/gettext/include
DATASPACES_LIB=-L/Users/eyildirim/Documents/ds_install/lib -ldspaces -ldscommon -ldart -lm
DATASPACES_INC=-I/Users/eyildirim/Documents/ds_install/include



CC=mpicxx


all:final_fs.o 


final_fs.o:final_fs.cpp
	$(CC) -c final_fs.cpp $(DATASPACES_INC) $(OPENCV_INC) $(OMP_INC) $(OPENSLIDE_INC)


.PHONY:clean

clean:
	rm -f *.o
	rm -f executable_serial

