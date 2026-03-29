# include $(CLASSICSTL_ITERATOR_DIR)/Makefile.mk

$(FINAL_CLASSICSTL_PDF): $(CLASSICSTL_PARTS) $(CONFIG_PARTS)
	$(LATEXMK) -jobname=$(subst /,-,$(<D)) $(CLASSICSTL_DIR)/slides
