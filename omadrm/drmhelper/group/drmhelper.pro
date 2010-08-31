
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

TEMPLATE = lib
TARGET = DRMHelper

IncludeBlock = \
	"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
	"$${LITERAL_HASH}include <platform_paths.hrh>"

MMP_RULES += IncludeBlock

SrcIfdefBlock = \
	"$${LITERAL_HASH}ifdef __DRM" \
	"SOURCE          ../src/drmhelper.cpp" \
	"SOURCE          ../src/ConsumeData.cpp" \
	"SOURCE          ../src/RDRMHelper.cpp" \
	"SOURCE          ../src/DRMHelperGlobalNoteWrapper.cpp" \
	"SOURCE          ../src/DRMHelperDownloadManager.cpp" \
	"SOURCE          ../src/DRMHelperInfoNoteWrapper.cpp" \
	"$${LITERAL_HASH}else" \
	"SOURCE          ../src/DRMHelperStub.cpp" \
	"SOURCE          ../src/RDRMHelperStub.cpp" \
	"SOURCE          ../src/DRMHelperGlobalNoteWrapper.cpp" \
	"$${LITERAL_HASH}endif" \
	"SOURCE          ../src/DRMHelperDll.cpp"

MMP_RULES += SrcIfdefBlock

symbian: { 
	TARGET.CAPABILITY = CAP_GENERAL_DLL DRM
	TARGET.VID = VID_DEFAULT
	TARGET.UID3 = 0x101F8647
	TARGET.EPOCALLOWDLLDATA = 1
	
	INCLUDEPATH += ../inc
	INCLUDEPATH += ../../drmengine/server/inc
	INCLUDEPATH += ../../drmengine/roap/inc
	INCLUDEPATH += ../../drmengine/roapstorage/inc
	INCLUDEPATH += ../../../inc   // ADo level inc dir
	INCLUDEPATH += /epoc32/include/platform/mw/cwrt

	#// Default system include paths for middleware layer modules.
	INCLUDEPATH += APP_LAYER_SYSTEMINCLUDE

	# needed because of SchemeHandler
	INCLUDEPATH += /epoc32/include/ecom    

	# needed for drmhelpersecondarydisplay.h

	LIBS += -leuser
	LIBS += -lcommonengine
	LIBS += -lcone
	LIBS += -lflogger
	LIBS += -laknnotify
	LIBS += -lapparc // for launching rights mgr ui embedded
	LIBS += -lavkon
	LIBS += -ldrmbrowserlauncher

	IfdefLibBlock = \
	"$${LITERAL_HASH}ifdef __DRM" \
	"LIBRARY         DrmParsers.lib" \
	"LIBRARY         DrmDcf.lib" \
	"LIBRARY         DrmRights.lib" \
	"LIBRARY         DrmServerInterfaces.lib" \
	"LIBRARY         DRMCommon.lib" \
	"LIBRARY         eikdlg.lib" \
	"LIBRARY         StarterClient.lib" \
	"LIBRARY         efsrv.lib // for launching rights mgr ui embedded" \
	"LIBRARY         APGRFX.lib" \
	"LIBRARY         WS32.lib" \
	"LIBRARY         dcfrep.lib" \
	"LIBRARY         centralrepository.lib" \
	"LIBRARY         ServiceHandler.lib" \
	"LIBRARY         caf.lib" \
	"LIBRARY         cafutils.lib" \
	"LIBRARY         charconv.lib" \
	"LIBRARY         ecom.lib       // for scheme handling" \
	"LIBRARY         cmmanager.lib" \
	"LIBRARY         estor.lib   // Cover Display" \
	"LIBRARY         featmgr.lib" \
	"LIBRARY         RoapHandler.lib" \
	"LIBRARY         mediatorclient.lib" \
	"$${LITERAL_HASH}ifdef RD_MULTIPLE_DRIVE" \
	"LIBRARY         platformenv.lib" \
	"$${LITERAL_HASH}endif" \
	"$${LITERAL_HASH}endif"
	
	MMP_RULES += IfdefLibBlock


	ResourceBlock = \
	"START RESOURCE Drmhelper.rss" \
	"HEADER" \
	"TARGETPATH RESOURCE_FILES_DIR" \
	"LANGUAGE_IDS" \
	"END"

	MMP_RULES += ResourceBlock

	defBlock = \
	"$${LITERAL_HASH}if defined(ARMCC)" \
	"DEFFILE ../eabi/" \
	"$${LITERAL_HASH}elif defined( WINSCW )" \
	"DEFFILE ../bwinscw/" \
	"$${LITERAL_HASH}elif defined( WINS )" \
	"DEFFILE ../bwins/" \
	"$${LITERAL_HASH}else" \
	"DEFFILE ../bmarm/" \
	"$${LITERAL_HASH}endif"

	MMP_RULES += defBlock
	
	MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl // don't use --export_all_vtbl"
	}

