/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logger utility.
*
*/


#ifndef DRMUILOGGER_H
#define DRMUILOGGER_H


// INCLUDES

#include <e32std.h>
#include <e32def.h>
#include <eikenv.h>
#include <flogger.h>


#ifdef _DEBUG
#define __TEST_DRM_LOG__
#endif // _DEBUG


#ifdef __TEST_DRM_LOG__

// CONSTANTS

// DRMUI logging directory.
_LIT( KDRMUILogDir,                   "DRMUI" );
// DRMUI log file name.
_LIT( KDRMUILogFile,                  "DRMUI.log" );
// Format string: enter function.
_LIT( KDRMUILogEnterFn,               "-> %S" );
// Format string: leave function.
_LIT( KDRMUILogLeaveFn,               "<- %S" );
// Format string: time.
_LIT( KDRMUILogTimeFormatString,      "%H:%T:%S:%*C2" );
// Format string: timestamp.
_LIT( KDRMUILogTimeStampFormatString, "%S %S" );
//Banner message
_LIT( KDRMUILogBanner,                "DRMUI 3.0");
// Message of exit from app
_LIT( KDRMUILogExit,                  "DRMUI: Application exit");

// DEFINES

// Create the log
#define CLOG_CREATE                 { FCreate(); }

// Close the log
#define CLOG_CLOSE                  \
    {                               \
    RFileLogger::Write              \
        (                           \
        KDRMUILogDir,               \
        KDRMUILogFile,              \
        EFileLoggingModeAppend,     \
        KDRMUILogExit               \
        );                          \
    }

// Write log: enter function.
#define CLOG_ENTERFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KDRMUILogDir,               \
        KDRMUILogFile,              \
        EFileLoggingModeAppend,     \
        KDRMUILogEnterFn,           \
        &temp                       \
        );                          \
    }

// Write log: leave function.
#define CLOG_LEAVEFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KDRMUILogDir,               \
        KDRMUILogFile,              \
        EFileLoggingModeAppend,     \
        KDRMUILogLeaveFn,           \
        &temp                       \
        );                          \
    }

// Write log: string 'a'.
#define CLOG_WRITE( a )             \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::Write              \
        (                           \
        KDRMUILogDir,               \
        KDRMUILogFile,              \
        EFileLoggingModeAppend,     \
        temp                        \
        );                          \
    }

// Write log: formatted.
#define CLOG_WRITE_FORMAT( a, b )   \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KDRMUILogDir,               \
        KDRMUILogFile,              \
        EFileLoggingModeAppend,     \
        temp,                       \
        b                           \
        );                          \
    }

// Write log: timestamp.
#define CLOG_WRITE_TIMESTAMP( a )                                   \
    {                                                               \
    _LIT( temp, a );                                                \
    TTime time;                                                     \
    time.HomeTime();                                                \
    TBuf<32> timeBuf;                                               \
    TRAPD( err, time.FormatL( timeBuf, KDRMUILogTimeFormatString ) ); \
    if ( !err )                                                     \
        {                                                           \
        RFileLogger::WriteFormat                                    \
            (                                                       \
            KDRMUILogDir,                                           \
            KDRMUILogFile,                                          \
            EFileLoggingModeAppend,                                 \
            KDRMUILogTimeStampFormatString,                         \
            &temp,                                                  \
            &timeBuf                                                \
            );                                                      \
        }                                                           \
    }

// Write formatted
#define CLOG_WRITEF                 FPrint


inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KDRMUILogDir, KDRMUILogFile, 
                              EFileLoggingModeAppend, aFmt, list );
    }


inline void FPrint( const TDesC& aDes )
    {
    RFileLogger::WriteFormat( KDRMUILogDir, KDRMUILogFile, 
                              EFileLoggingModeAppend, aDes );
    }


inline void FCreate()
    {
    TFileName path( _L( "c:\\logs\\" ) );
    path.Append( KDRMUILogDir );
    path.Append( _L( "\\" ) );
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll( path );
    RFileLogger::WriteFormat( KDRMUILogDir, KDRMUILogFile, 
                              EFileLoggingModeOverwrite, KDRMUILogBanner );
    }


#else // not defined __TEST_DRM_LOG__

inline void FPrint( const TRefByValue<const TDesC> /*aFmt*/, ... ) {};


// DEFINES

// Empty definition (disable log).
#define CLOG_CREATE

// Empty definition (disable log).
#define CLOG_CLOSE

// Empty definition (disable log).
#define CLOG_ENTERFN( a )

// Empty definition (disable log).
#define CLOG_LEAVEFN( a )

// Empty definition (disable log).
#define CLOG_WRITE( a )

// Empty definition (disable log).
#define CLOG_WRITE_FORMAT( a, b )

// Empty definition (disable log).
#define CLOG_WRITE_TIMESTAMP( a )

// Empty definition (disable log).
#define CCDLGLOGGER_WRITEF   1 ? ((void)0) : FPrint

#endif // __TEST_DRM_LOG__

#endif // DRMUILOGGER_H
