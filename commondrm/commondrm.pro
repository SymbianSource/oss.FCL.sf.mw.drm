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
# Description: Commondrm main level project file.
#

TEMPLATE = subdirs

SYMBIAN_PLATFORMS = WINSCW ARMV5

SUBDIRS += drmbrowserlauncher/drmbrowserlauncher.pro
SUBDIRS += drmqtencryptor/qt_drmencryptor.pro
SUBDIRS += drmutility/group/qt/drmutilitydmgrwrapper.pro
SUBDIRS += drmutility/group/qt/drmuihandlingimpl.pro
SUBDIRS += drmui/drmuinotifications/drmuinotifications.pro
