/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef LOG_H
#define LOG_H

#ifdef _DEBUG
#define _LOGGING
#endif

#ifdef _LOGGING

#include <e32std.h>
#include <f32file.h>
#include <flogger.h>

#ifndef _LOGGING_DIR
#define _LOGGING_DIR L"wmdrm"
#endif

#ifndef _LOGGING_FILE
#define _LOGGING_FILE L"wmdrm.txt"
#endif

const static TLitC<sizeof(_LOGGING_DIR) / 2> KLogDir =
    {
    sizeof(_LOGGING_DIR) / 2 - 1, _LOGGING_DIR
    };
const static TLitC<sizeof(_LOGGING_FILE) / 2> KLogFile =
    {
    sizeof(_LOGGING_FILE) / 2 - 1, _LOGGING_FILE
    };

#define LOG( des ) \
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, des )
    
#define LOG1( AAA ) \
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, _L( AAA ) )

#define LOG2( FMT, BBB ) \
    RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, _L( FMT ), BBB )
    
#define LOG3( FMT, BBB, CCC ) \
    RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, _L( FMT ), BBB, CCC )
    
#define LOG4( FMT, BBB, CCC, DDD ) \
    RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, _L( FMT ), BBB, CCC, DDD )
    
#define LOG5( FMT, BBB, CCC, DDD, EEE ) \
    RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, _L( FMT ), BBB, CCC, DDD, EEE )
    
#define LOGHEX( ptr, len ) \
    RFileLogger::HexDump( KLogDir, KLogFile, EFileLoggingModeAppend, _S(""), _S(""), ptr, len )
    
#define LOGFN( AAA ) TLogFn ___tmp( _L( AAA ) )
#define LOGFNR( AAA, BBB ) TLogFn ___tmp( _L( AAA ), (TInt* ) &( BBB ) )

#define CLEARLOG() {\
    RFs fs;\
    TBuf<sizeof( _LOGGING_DIR ) + sizeof( _LOGGING_DIR ) + 32> path;\
    path.Copy( _L( "c:\\logs\\" ) ); path.Append( KLogDir ); path.Append( '\\' ); path.Append( KLogFile ); \
    fs.Connect();\
    fs.Delete( path );\
    fs.Close();}

/**
 *  Class for logging funtion calls and exits
 *
 *  The constructor logs the entry of the function, the destructor the exit
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TLogFn )
    {
public:
    TLogFn( const TDesC& aName, TInt* aResult = NULL ):
        iResult( aResult)
        {
    	RBuf8 data;
    	
    	iName.Set( aName );
    	if ( data.Create( iName.Size() + 8 ) == KErrNone )
    	    {
    	    data.Copy( _L( ">> " ) );
    	    data.Append( iName );
    	    LOG( data );
    	    data.Close();
    	    }
    	}

    ~TLogFn()
        {
    	RBuf8 data;
    	
    	if ( data.Create( iName.Size() + 50 ) == KErrNone )
    	    {
        	data.Copy( _L( "<< " ) );
        	data.Append( iName );
        	if ( iResult )
        	    {
        	    data.Append( ' ' );
        	    data.AppendNum( *iResult );
        	    data.Append( _L( " 0x" ) );
        	    data.AppendNum( *iResult, EHex );
        	    }
        	LOG( data );
        	data.Close();
        	}
        }
    
private: // data

    /**
     * Name of the function
     */
    TPtrC iName;
    TInt* iResult;
    };

#else // _LOGGING

#define LOG( des )
#define LOG1( AAA )
#define LOG2( FMT, BBB )
#define LOG3( FMT, BBB, CCC )
#define LOG4( FMT, BBB, CCC, DDD )
#define LOG5( FMT, BBB, CCC, DDD, EEE )
#define LOGHEX( ptr, len )
#define LOGFN( AAA )	
#define LOGFNR( AAA, BBB )	
#define CLEARLOG()

#endif // _LOGGING
    
#endif // LOG_H
