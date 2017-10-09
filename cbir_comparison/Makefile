ROOTDIR=/Users/eyildirim/Documents/cbir_comparison

subsystem:
	$(MAKE) -C $(ROOTDIR)/cbir_common/
	$(MAKE) -C $(ROOTDIR)/rough_searching_omp/
	$(MAKE) -C $(ROOTDIR)/fine_searching_omp/
	$(MAKE) -C $(ROOTDIR)/clustering/
	$(MAKE) -C $(ROOTDIR)/final_fine_searching/
	$(MAKE) -C $(ROOTDIR)/write_image/
	$(MAKE) -C $(ROOTDIR)/cbir_main/


.PHONY: clean

clean:
	$(MAKE) -C $(ROOTDIR)/cbir_common/ clean
	$(MAKE) -C $(ROOTDIR)/rough_searching_omp/ clean
	$(MAKE) -C $(ROOTDIR)/fine_searching_omp/ clean
	$(MAKE) -C $(ROOTDIR)/clustering/ clean
	$(MAKE) -C $(ROOTDIR)/final_fine_searching/ clean
	$(MAKE) -C $(ROOTDIR)/write_image/ clean
	$(MAKE) -C $(ROOTDIR)/cbir_main/ clean

