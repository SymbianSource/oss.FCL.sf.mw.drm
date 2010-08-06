#
# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  DRM Encryption tool project file.
#
#

TEMPLATE = app
TARGET = drmencryptor
DEPENDPATH += .
INCLUDEPATH += . 
INCLUDEPATH += ../../inc

symbian:
{
TARGET.CAPABILITY += DRM
}

# Input
HEADERS += drmencryptor.h
FORMS += drmencryptor.ui
SOURCES += drmencyptor.cpp main.cpp
LIBS += -lplatformenv -ldrmserviceapi -ldcfrep -ldrmparsers -lfbscli -lcone -lestor -lcaf -lefsrv -ldrmserverinterfaces
