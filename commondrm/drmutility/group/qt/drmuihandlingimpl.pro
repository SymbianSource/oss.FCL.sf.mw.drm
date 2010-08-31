
#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

include ( drmuihandlingimpl.pri )

IncludeBlock = \
	"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
	"$${LITERAL_HASH}include <platform_paths.hrh>" 
MMP_RULES += IncludeBlock

TEMPLATE = lib
TARGET = DRMUiHandlingImpl

SrcIfdefBlock = \
	"$${LITERAL_HASH}ifdef __DRM" \
	"SOURCE ../../src/DrmUiHandlingImpl.cpp" \
	"SOURCE ../../src/drmuihandlingdata.cpp" \
	"SOURCE ../../src/DrmUtilityDownloadManager.cpp" \
	"$${LITERAL_HASH}endif"

MMP_RULES += SrcIfdefBlock

symbian: { 
	TARGET.CAPABILITY = CAP_GENERAL_DLL DRM
	TARGET.VID = VID_DEFAULT
	TARGET.UID3 = 0x10205CBE
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	INCLUDEPATH += ../../inc
	INCLUDEPATH += ../../../../omadrm/drmengine/roap/inc
	INCLUDEPATH += ../../../../omadrm/drmengine/roapstorage/inc
	INCLUDEPATH += ../../../../omadrm/drmengine/server/inc
	INCLUDEPATH += ../../../../inc   // ADo level inc dir
	INCLUDEPATH += /epoc32/include/platform/mw/cwrt
	INCLUDEPATH += /epoc32/include/ecom

	LIBS += -leuser
	LIBS += -lapparc // for launching rights mgr ui embedded
	LIBS += -lavkon
	LIBS += -lefsrv // for launching rights mgr ui embedded
	LIBS += -lapgrfx
	LIBS += -lws32
	LIBS += -lcentralrepository
	LIBS += -lservicehandler
	LIBS += -lcaf
	LIBS += -lcafutils
	LIBS += -lcharconv
	LIBS += -lecom // for scheme handling
	LIBS += -lcmmanager
	LIBS += -ldrmbrowserlauncher

	IfdefLibBlock = \
	"$${LITERAL_HASH}ifdef __DRM" \
	"LIBRARY  drmutilitycommon.lib" \
	"LIBRARY  drmutility.lib" \
	"LIBRARY  roaphandler.lib" \
	"LIBRARY  drmparsers.lib" \
	"LIBRARY  drmdcf.lib" \
	"LIBRARY  dcfrep.lib" \
	"LIBRARY  drmrights.lib" \
	"LIBRARY  drmserverinterfaces.lib" \
	"LIBRARY  drmcommon.lib" \
	"$${LITERAL_HASH}endif"
	
	MMP_RULES += IfdefLibBlock

	defBlock = \
	"$${LITERAL_HASH}if defined(ARMCC)" \
	"DEFFILE ../../eabi/" \
	"$${LITERAL_HASH}elif defined( WINSCW )" \
	"DEFFILE ../../bwinscw/" \
	"$${LITERAL_HASH}elif defined( WINS )" \
	"DEFFILE ../../bwins/" \
	"$${LITERAL_HASH}else" \
	"DEFFILE ../../bmarm/" \
	"$${LITERAL_HASH}endif"

	MMP_RULES += defBlock
	
	}

