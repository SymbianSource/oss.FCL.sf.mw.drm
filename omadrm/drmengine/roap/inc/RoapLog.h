/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logger utilities for ROAP
*
*/



#ifndef ROAP_LOG_H
#define ROAP_LOG_H

#ifdef _DEBUG
#include <e32debug.h>
#define ROAPDEBUGCODE( a ) a 
#define ROAPDEBUG( a ) RDebug::Print( a );
#define ROAPDEBUG2( a, b ) RDebug::Print( a, b );
#define ROAPDEBUGLIT( a ) ROAPDEBUG( _L ( a ) )
#else
#define ROAPDEBUGCODE( a )
#define ROAPDEBUG( a )
#define ROAPDEBUG2( a, b )
#define ROAPDEBUGLIT( a )
#endif

#ifndef _ROAP_TESTING

#define LOG( a )
#define LOGLIT( a )
#define LOGHEX( ptr, len )
#define LOG2( a, b )
#define PERFORMANCE_LOG( a )
#define PERFORMANCE_LOG2( a, b )
#define PERFORMANCE_LOGLIT( a )

#else // _ROAP_TESTING

#include <flogger.h>

// #define PERFORMANCE_LOGGING_ONLY // Disables normal logging. Enables only minimal logging for performance testing
#define _ROAP_DETAILED_LOGGING // Normal logging + more detailed (signature) logging

_LIT(KRoapLogDir, "DRM");
_LIT(KRoapLogFile, "ROAP.log");

#ifdef PERFORMANCE_LOGGING_ONLY

// Performance logging only

#define LOG( a )
#define LOGLIT( a )
#define LOGHEX( ptr, len )
#define LOG2( a, b )
#define PERFORMANCE_LOG( a ) RFileLogger::Write( KRoapLogDir(), KRoapLogFile(), EFileLoggingModeAppend, a );
#define PERFORMANCE_LOG2( a, b ) RFileLogger::WriteFormat( KRoapLogDir(), KRoapLogFile(), EFileLoggingModeAppend, a, b );
#define PERFORMANCE_LOGLIT( a ) PERFORMANCE_LOG( _L ( a ) )
#else // PERFORMANCE_LOGGING_ONLY

// Normal logging

#define LOG( a ) RFileLogger::Write( KRoapLogDir(), KRoapLogFile(), EFileLoggingModeAppend, a );
#define LOGLIT( a ) LOG( _L ( a ) )
#define LOGHEX( ptr, len ) RFileLogger::HexDump( KRoapLogDir(), KRoapLogFile(), EFileLoggingModeAppend, _S(""), _S(""), ptr, len );
#define LOG2( a, b ) RFileLogger::WriteFormat( KRoapLogDir(), KRoapLogFile(), EFileLoggingModeAppend, a, b );
#define PERFORMANCE_LOG( a )
#define PERFORMANCE_LOG2( a, b )
#define PERFORMANCE_LOGLIT( a )
#endif // PERFORMANCE_LOGGING_ONLY
#endif // _ROAP_TESTING
#ifdef _ROAP_DETAILED_LOGGING
#define DETAILLOG( a ) LOG( a )
#define DETAILLOGLIT( a ) LOGLIT( a )
#define DETAILLOGHEX( ptr, len ) LOGHEX( ptr, len )
#define DETAILLOG2( a, b ) LOG2( a, b )
#else
#define DETAILLOG( a )
#define DETAILLOGLIT( a )
#define DETAILLOGHEX( ptr, len )
#define DETAILLOG2( a, b )
#endif

#endif // ROAP_LOG_H
