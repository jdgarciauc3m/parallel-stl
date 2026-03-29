# include $(CLASSICSTL_ITERATOR_DIR)/Makefile.mk

$(FINAL_CLASSICSTL_PDF): $(CLASSICSTL_PARTS) $(CONFIG_PARTS) | $(FINAL_DIR)
	$(LATEXMK) -jobname=$(subst /,-,$(<D)) $(CLASSICSTL_DIR)/slides

$(FINAL_DIR):
	mkdir -p $(FINAL_DIR)
