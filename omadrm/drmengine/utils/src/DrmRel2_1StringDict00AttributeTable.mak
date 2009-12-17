BUILD_DIR=$(EPOCROOT)epoc32\build\generated\Xml
GENERATED_FILES= \
	$(BUILD_DIR)\DrmRel2_1StringDict00AttributeTable.cpp
SOURCE_DIR=.

$(BUILD_DIR)\DrmRel2_1StringDict00AttributeTable.cpp : $(SOURCE_DIR)\DrmRel2_1StringDict00AttributeTable.st
	perl -S emkdir.pl $(BUILD_DIR)
	perl -S ecopyfile.pl $(SOURCE_DIR)\DrmRel2_1StringDict00AttributeTable.st $(BUILD_DIR)\DrmRel2_1StringDict00AttributeTable.st
	perl -S stringtable.pl $(BUILD_DIR)\DrmRel2_1StringDict00AttributeTable.st

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
