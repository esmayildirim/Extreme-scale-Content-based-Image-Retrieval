# Extreme-scale-Content-based-Image-Retrieval

A hybrid MPI-OpenMP Workflow for searching cancerous patterns in large tissue slide images (Whole Slide Images)

Dependant image processing libs: Openslide, OpenCV

It consists of 6 modules each with its own Makefile
Update the makefiles according to your system library and application paths 
Run make from the parent directory
Update ./run_script.sh 

Example :

mpiexec -n 2 /Users/eyildirim/Documents/cbir_comparison/cbir_main/executable --tilex 1024 --tiley 1024 --level 1 /Users/eyildirim/Documents/input/ /Users/eyildirim/Documents/cbir_comparison/query_images/level1/roi_1_level_1_TCGA-EJ-A46I-01Z-00-DX1.tif

Parameters :

--tilex : width of tile

--tiley : height of tile

--level : resolution level of image data

dataset directory of WSI images

dataset directory of query images
