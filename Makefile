FINAL_DIR=final

LATEXMK=latexmk -pdf -silent -file-line-error -halt-on-error -outdir=$(FINAL_DIR) 

CONFIG_PARTS= \
	metadata.tex \
	license-cc.tex \
	config/* \

LOGOS=logos/*

VERSION_PARTS= \
	$(FINAL_DIR)/version.tex

include paralgo/Makefile.def

include guide/Makefile.def

.PHONY: all clean slides guide

all:	slides guide

clean:
	rm -Rf $(FINAL_DIR)

$(FINAL_DIR):
	mkdir -p $(FINAL_DIR)

$(FINAL_DIR)/version.tex:	.git/HEAD .git/index | $(FINAL_DIR)
	LC_ALL=C git log -1 --format=%ad --date=format:'\newcommand{\versiondate}{%B %Y}' > $(FINAL_DIR)/version.tex
	git log -1 --format='\newcommand{\versionid}{%h}' >> $(FINAL_DIR)/version.tex

.PHONY: updatevers
updatevers:	$(FINAL_DIR)/version.tex

slides: $(ALL_SLIDES_FINAL_PDF)

guide: $(GUIDE_FINAL_PDF)

include $(PARALGO_DIR)/Makefile.mk

include $(GUIDE_DIR)/Makefile.mk
