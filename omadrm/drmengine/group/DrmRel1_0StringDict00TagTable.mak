BUILD_DIR=$(EPOCROOT)epoc32\build\generated\Xml
GENERATED_FILES= \
	$(BUILD_DIR)\DrmRel1_0StringDict00TagTable.cpp
SOURCE_DIR=..\RO\src\

$(BUILD_DIR)\DrmRel1_0StringDict00TagTable.cpp : $(SOURCE_DIR)\DrmRel1_0StringDict00TagTable.st
	perl -S emkdir.pl $(BUILD_DIR)
	perl -S ecopyfile.pl $(SOURCE_DIR)\DrmRel1_0StringDict00TagTable.st $(BUILD_DIR)\DrmRel1_0StringDict00TagTable.st
	perl -S stringtable.pl $(BUILD_DIR)\DrmRel1_0StringDict00TagTable.st

do_nothing:
	@rem do nothing

#
# The targets invoked by bld...
#

MAKMAKE : $(GENERATED_FILES)

BLD : MAKMAKE

SAVESPACE : MAKMAKE

CLEAN : 
	perl -S ermdir.pl $(BUILD_DIR)

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES : $(GENERATED_FILES)
