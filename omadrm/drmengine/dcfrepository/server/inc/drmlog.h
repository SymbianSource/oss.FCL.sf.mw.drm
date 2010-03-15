/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug log macro definitions
*
*/



#ifndef DRMLOG_H
#define DRMLOG_H

//  INCLUDES
#if (defined _DEBUG || defined _LOGGING)
#include <e32svr.h>
#include <flogger.h>

// CONSTANTS
// MACROS

#ifndef DRMFLOGFILE
#define DRMFLOGFILE _L("DcfRepSrv.log")
#endif

#define DRMLOG( a ) \
    RFileLogger::Write(_L("drm"), DRMFLOGFILE, EFileLoggingModeAppend, a); \
    RDebug::Print( a );

#define DRMLOG2( a, b ) \
    RFileLogger::WriteFormat(_L("drm"), DRMFLOGFILE, EFileLoggingModeAppend, a, b); \
    RDebug::Print( a, b );

#define DRMLOGHEX( a ) \
    RFileLogger::HexDump(_L("drm"), DRMFLOGFILE, EFileLoggingModeAppend, _S(""), _S(""), (a).Ptr(), (a).Size());

#else

#define DRMLOG( a )
#define DRMLOG2( a, b )
#define DRMLOGHEX( a )

#endif // _DEBUG

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

#endif      // DRMLOG_H

// End of File


