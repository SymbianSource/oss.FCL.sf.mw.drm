
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

include ( drmutilitydmgrwrapper.pri )

TEMPLATE = lib
TARGET = DrmUtilityDmgrWrapper

IncludeBlock = \
	"$${LITERAL_HASH}include <data_caging_paths.hrh>" \
	"$${LITERAL_HASH}include <platform_paths.hrh>"

MMP_RULES += IncludeBlock


SrcIfdefBlock = \
	"$${LITERAL_HASH}ifdef __DRM" \
	"SOURCE ../../src/DrmUtilityDmgrWrapper.cpp" \
	"SOURCE ../../src/drmutilityconnection.cpp" \
	"SOURCE ../../src/qdrmutilitydmgreventhandler.cpp" \
	"$${LITERAL_HASH}endif"

MMP_RULES += SrcIfdefBlock

symbian: {
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.VID = VID_DEFAULT
	TARGET.UID3 = 0x102830FE
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
	INCLUDEPATH += ../../inc
	INCLUDEPATH += ../../../../omadrm/drmengine/roap/inc
	INCLUDEPATH += ../../../../inc   // ADo level inc dir
	INCLUDEPATH += ../../traces      // OST definitions
	INCLUDEPATH += /epoc32/include/platform/mw/cwrt

	LIBS += -leuser
	LIBS += -lcmmanager // RCmManager
	LIBS += -lesock // RConnection, RSocketServ
	LIBS += -lextendedconnpref
	LIBS += -lnetmeta // TExtendedConnPref
	LIBS += -lWrtDownloadMgr
	LIBS += -lroaphandler
	LIBS += -lefsrv
	LIBS += -lavkon
	LIBS += -lcommonengine
	LIBS += -leikctl
	LIBS += -lplatformenv
	LIBS += -lflogger

	ResourceBlock = \
	"START RESOURCE ../../data/DrmUtilityDmgrWrapper.rss" \
	"HEADER" \
	"TARGETPATH RESOURCE_FILES_DIR" \
	"LANGUAGE_IDS" \
	"END"

	MMP_RULES += ResourceBlock

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

