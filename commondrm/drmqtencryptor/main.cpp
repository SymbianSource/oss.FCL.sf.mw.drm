/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  DRM Encryption tool.
*
*/

#include <QtGui/QApplication>
#include "drmencryptor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DRMEncryptor w;
    w.showMaximized();
    return a.exec();
}
