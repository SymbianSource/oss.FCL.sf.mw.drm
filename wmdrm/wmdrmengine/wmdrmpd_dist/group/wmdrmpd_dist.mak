#
# Check if the binaries exist
#
armv5_files = $(wildcard ../armv5/urel/*.dll)
winscw_files = $(wildcard ../winscw/urel/*.dll)
key_files = $(wildcard ../data/*.dat)

.PHONY: create_dirs do_nothing clean_armv5 clean_winscw clean_keys MAKMAKE \
	BLD SAVESPACE CLEAN FREEZE LIB CLEANLIB RESOURCE FINAL RELEASABLES
#
# empty action
#
do_nothing:
	@rem do nothing

create_dirs:
	@perl -S emkdir.pl /epoc32/release/armv5/urel
	@perl -S emkdir.pl /epoc32/release/armv5/udeb
	@perl -S emkdir.pl /epoc32/release/winscw/urel
	@perl -S emkdir.pl /epoc32/release/winscw/udeb
	@perl -S emkdir.pl /epoc32/data/z/data
	@perl -S emkdir.pl /epoc32/release/winscw/udeb/z/data

clean_armv5:
	@perl -S ermdir.pl /epoc32/release/armv5/urel/wmdrmpd.dll \
	/epoc32/release/armv5/udeb/wmdrmpd.dll \
	/epoc32/release/armv5/urel/wmdrmkeystorage.dll \
	/epoc32/release/armv5/udeb/wmdrmkeystorage.dll

clean_winscw:
	@perl -S ermdir.pl /epoc32/release/winscw/urel/wmdrmpd.dll \
	 /epoc32/release/winscw/udeb/wmdrmpd.dll

clean_keys:
	@perl -S ermdir.pl /epoc32/data/z/data/devcert.dat \
	/epoc32/data/z/data/key.dat \
	/epoc32/release/winscw/data/z/data/devcert.dat \
	/epoc32/release/winscw/udeb/z/data/key.dat

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

#
# during abld bld, copy the binaries to the release dir, if they exist
#
BLD : create_dirs
ifeq ($(armv5_files),)
	@echo *** Using WMDRM ARMV5 stub
else
	@perl -S ecopyfile.pl ../armv5/urel/wmdrmpd.dll /epoc32/release/armv5/urel/wmdrmpd.dll
	@perl -S ecopyfile.pl ../armv5/udeb/wmdrmpd.dll /epoc32/release/armv5/udeb/wmdrmpd.dll
	@perl -S ecopyfile.pl ../armv5/urel/wmdrmkeystorage.dll /epoc32/release/armv5/urel/wmdrmkeystorage.dll
	@perl -S ecopyfile.pl ../armv5/udeb/wmdrmkeystorage.dll /epoc32/release/armv5/udeb/wmdrmkeystorage.dll
endif

ifeq ($(winscw_files),)
	@echo *** Using WMDRM WINSCW stub
else
	@perl -S ecopyfile.pl ../winscw/urel/wmdrmpd.dll /epoc32/release/winscw/urel/wmdrmpd.dll
	@perl -S ecopyfile.pl ../winscw/udeb/wmdrmpd.dll /epoc32/release/winscw/udeb/wmdrmpd.dll
endif

ifeq ($(key_files),)
	@echo *** Not using any device keys
else
	@perl -S ecopyfile.pl ../data/devcert.dat /epoc32/data/z/data/devcert.dat
	@perl -S ecopyfile.pl ../data/priv.dat /epoc32/data/z/data/priv.dat
	@perl -S ecopyfile.pl ../data/devcert.dat /epoc32/release/winscw/udeb/z/data/devcert.dat
	@perl -S ecopyfile.pl ../data/priv.dat /epoc32/release/winscw/udeb/z/data/priv.dat
endif

SAVESPACE : BLD

CLEAN : clean_armv5 clean_winscw clean_keys

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES : do_nothing
