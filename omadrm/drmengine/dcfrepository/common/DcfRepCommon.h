/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client side header
*
*/



#ifndef DCFREPCOMMON_H
#define DCFREPCOMMON_H

//  INCLUDES

// CONSTANTS
//the server version. A version must be specified when
//creating a session with the server
const TUint KDcfRepMajorVersionNumber=0;
const TUint KDcfRepMinorVersionNumber=1;
const TUint KDcfRepBuildVersionNumber=1;
const TUint KTtidLen = 16;
const TUint KOrdFilename = 1;
const TUint KOrdPosition = 2;
const TUint KOrdCid = 3;
const TUint KOrdGroupId = 4;
const TUint KOrdTtid = 5;
const TUint KMaxLength =65536;
const TUint KStartPosition = 0;

_LIT( KDbFileLocation , "c:\\system\\data\\DcfRepDb" );
_LIT( KDcfRepSrvName ,"CDcfRepSrv" ); // server name
_LIT( KDcfRepSrvFileName ,"DcfRepSrv.exe" ); // for client to search for server
_LIT( KDcfRepSrv , "CDCFREPSRV" ); //panic info
_LIT( KDcfRepSemaphoreName , "DcfRepSemaphore" ); //Semaphore
_LIT(KDb , "DcfRepDb");
_LIT(KTable , "dcf");
_LIT(KColCid , "cid");
_LIT(KColGroupId , "group");
_LIT(KColFilename , "filename");
_LIT(KColPosition , "position");
_LIT(KColTtid , "ttid");


//opcodes used in message passing between client and server
enum TDcfRepSrvRqst
    {
    EScanStart = 201,
    EScanAdd,
    EScanEnd,
    EFullScan,
    ESetTtidStart,
    ESetTtidAdd,
    ESetTtidEnd,
    EGetFileHandle,
    EAddFile,
    ENotifyIdle,
    EStopWatching
    };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS



// CLASS DECLARATION



#endif      // DCFREPCOMMON_H

// End of File
