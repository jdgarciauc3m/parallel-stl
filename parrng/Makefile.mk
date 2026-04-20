include $(PARRNG_ALGO_DIR)/Makefile.mk

$(FINAL_PARRNG_PDF): $(PARRNG_PARTS) $(CONFIG_PARTS) | $(FINAL_DIR)
	$(LATEXMK) -jobname=$(subst /,-,$(<D)) $(PARRNG_DIR)/slides
