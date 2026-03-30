include $(PARALGO_STL_DIR)/Makefile.mk
include $(PARALGO_EXEC_DIR)/Makefile.mk

$(FINAL_PARALGO_PDF): $(PARALGO_PARTS) $(CONFIG_PARTS) | $(FINAL_DIR)
	$(LATEXMK) -jobname=$(subst /,-,$(<D)) $(PARALGO_DIR)/slides
