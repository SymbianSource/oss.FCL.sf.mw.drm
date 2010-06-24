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
TARGET = drmbrowserlauncher 	

symbian:
{
TARGET.UID1 = 0x1000008D
TARGET.UID3 = 0x200315BC

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.VID = VID_DEFAULT
TARGET.EPOCALLOWDLLDATA = 1

defBlock = \
	"$${LITERAL_HASH}if defined(ARMCC)" \
	"DEFFILE ./eabi/" \
	"$${LITERAL_HASH}elif defined( WINSCW )" \
	"DEFFILE ./bwinscw/" \
	"$${LITERAL_HASH}elif defined( WINS )" \
	"DEFFILE ./bwins/" \
	"$${LITERAL_HASH}else" \
	"DEFFILE ./bmarm/" \
	"$${LITERAL_HASH}endif"

	MMP_RULES += defBlock
	
	MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl // don't use --export_all_vtbl"
}

DEPENDPATH += .
DEPENDPATH += ./src

INCLUDEPATH += ../../inc

SOURCES += drmbrowserlauncher.cpp

