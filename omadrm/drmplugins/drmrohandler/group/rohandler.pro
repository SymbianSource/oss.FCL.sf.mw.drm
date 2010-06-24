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
# Description: This is project specification file for the rohandler.dll
#

include ( rohandler.pri )

IncludeBlock = \
	"$${LITERAL_HASH}include <platform_paths.hrh>" \
	"$${LITERAL_HASH}include <data_caging_paths.hrh>" 

MMP_RULES += IncludeBlock

TEMPLATE = lib
TARGET = RoHandler
MMP_RULES += "TARGETTYPE plugin"

symbian: { 
	TARGET.CAPABILITY = CAP_ECOM_PLUGIN DRM
	TARGET.VID = VID_DEFAULT
	TARGET.UID2 = 0x10009D8D
	TARGET.UID3 = 0x101F7B92
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += ../inc
	INCLUDEPATH += ../../../drmengine/roap/inc
	INCLUDEPATH += ../../drmromtm/client/inc
	INCLUDEPATH += ../../../../inc   // ADo level inc dir
	INCLUDEPATH += /epoc32/include/platform/mw/cwrt

	LIBS += -lDrmParsers
	LIBS += -lDrmDcf 
	LIBS += -lDrmRights 
	LIBS += -lDrmServerInterfaces
	LIBS += -ldrmroapwbxmlparser // ROAP Wbxml to XML parsers
	LIBS += -lcentralrepository // Browser default AP
	LIBS += -lcmmanager
	LIBS += -lWrtDownloadMgr
	LIBS += -leuser
	LIBS += -lecom
	LIBS += -lRoapHandler
	LIBS += -lWapPushUtils
	LIBS += -lmsgs // for MMsvSessionObserver

	LIBS += -lefsrv
	LIBS += -linetprotutil // TUri16 and so on
	LIBS += -lbafl // RResourceFile
	LIBS += -lflogger
	LIBS += -lsysutil // SysUtil
	LIBS += -lesock

	ResourceBlock1 = \
	"START RESOURCE 101F7B92.rss" \
	"TARGET RoHandler" \
	"END"

	ResourceBlock2 = \
	"START RESOURCE RoHandler.rss" \
	"HEADER" \
	"TARGETPATH RESOURCE_FILES_DIR" \
	"LANGUAGE_IDS" \
	"END"

	MMP_RULES += ResourceBlock1
	MMP_RULES += ResourceBlock2

	IfdefLibBlock = \
	"$${LITERAL_HASH}ifdef RD_MULTIPLE_DRIVE" \
	"LIBRARY platformenv.lib" \
	"$${LITERAL_HASH}endif"

	MMP_RULES += IfdefLibBlock

	MWIncPath = \
		"// Default system include paths for middleware layer modules." \
		"MW_LAYER_SYSTEMINCLUDE" \

	MMP_RULES += MWIncPath

	SMPSafeBlock = "SMPSAFE"

	MMP_RULES += SMPSafeBlock

	BLD_INF_RULES.prj_exports += "../loc/rohandler.loc MW_LAYER_LOC_EXPORT_PATH( rohandler.loc )"

	defBlock = " "

	MMP_RULES += defBlock

	
	}

