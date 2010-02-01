# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: DRM Crypto Library build configuration
#
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

crypto_files = $(wildcard ../DrmCrypto/CMLA/*.cpp)
do_nothing:
	

#
# The targets invoked by bld...
#
MAKMAKE :
	$(call createdir,"../drmcrypto/build")
ifeq ($(crypto_files),)
	perl $(EPOCROOT)epoc32/tools/ecopyfile.pl ../DrmCrypto/stub/CmlaCryptoStub.cpp ../drmcrypto/build/cmlacrypto.cpp
else
	perl $(EPOCROOT)epoc32/tools/ecopyfile.pl ../DrmCrypto/CMLA/CmlaCrypto.cpp ../drmcrypto/build/cmlacrypto.cpp
endif
	perl $(EPOCROOT)epoc32/tools/ecopyfile.pl ../DrmCrypto/src/OmaCrypto.cpp ../drmcrypto/build/omacrypto.cpp
BLD : MAKMAKE

SAVESPACE : do_nothing

CLEAN :
	perl $(EPOCROOT)epoc32/tools/ermdir.pl ../drmcrypto/build/omacrypto.cpp
	perl $(EPOCROOT)epoc32/tools/ermdir.pl ../drmcrypto/build/cmlacrypto.cpp

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES : do_nothing
