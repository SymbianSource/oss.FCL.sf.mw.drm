/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file defines RightsServer.exe's client/server interface
*
*/


#ifndef DRMENGINECLIENTSERVER_H
#define DRMENGINECLIENTSERVER_H

//  INCLUDES
#include <e32def.h>
// #include <e32uid.h>

// Added namespace to prevent namespace pollution (TDRMCommand).
namespace DRMEngine
    {
    // CONSTANTS
    const TUint8 KServerMajorVersion =  3;
    const TUint8 KServerMinorVersion =  0;
    const TUint16 KServerBuildVersion =  0;
    
    const TInt KServerUID = 0x101F51F2;
    const TInt KNullUID = 0x0;
    const TInt KMaxOmaV1CIDLength = 256;
        
    _LIT( KDRMSemaphore, "RightsServerSem" );
   
    _LIT( KServerName, "!RightsServer" );   
    
    // MACROS
    // DATA TYPES
       
    enum TCommand 
        {
        /*
        Command: failsafe
        */
        ENone,       /* 0 */
        
        /*
        Command: Add a new rights object into database.
        Params: 
        TDesC8( CID ),
        TPtr8( TRightsObject ),
        TDesC8( CEK ),
        TDes8( Unique ID )
        */ 
        EAddRecord,
        
        /*
        Command: Get single rights object.
        Params:
        TPckg8<TInt>,
        NULL,
        TPtrC8( TDRMUniqueID ),
        TDesC8( CID )
        */
        EGetDbEntry,
        
        /*
        Command: Get list of rights objects and store the list 
        into a temporary file.
        Params: 
        TFileName( temp file ),
        TDesC8( CID ),
        */
        EGetEntryList,
        
        /*
        Command: Delete all ROs with certain CID.
        Params:
        NULL,
        NULL,
        TDesC8( CID )
        */ 
        EDeleteWithCID,
        
        /*
        Command: Delete one RO, identified with UID & CID.
        Params:
        TPtr8( TDRMUniqueID )
        NULL,
        NULL,
        TDesC8( Content-ID )
        */
        EDeleteRO,  /* 5 */
        
        /*
        Command: Export Content-ID list into a file.
        Params:
        TDes( File name )
        */
        EExportCIDs,
        
        /* 
        Command: Get decryption key, update rights if said to do so.
        Params: 
        TInt( usage intention ),
        TDesC8( content ID ),
        TDRMKey( key ),
        */
        EGetKey,
        
        /*
        Command: Check whether there is available rights.
        Params:
        TInt( usage intention ),
        TDesC8( content ID )
        */
        ECheckRights,
        /*
        Command: Return how many rights object there is in the DB.
        Params:
        TPtr8( integer )
        */
        ECount,
        
        /*
        Command: Delete every RO from the database.
        Params:
        -
        */
        EDeleteAll, /* 10 */

        /*
          Command: Consume the right with database
          Params: 
          TDesC8( content ID )
        */
        EConsume,

        /*
        Command: Calculate the padding from file ending
        Params:
        TDesC8( content ID )
        TDesC8( last two data blocks )
        */
        ECalculatePadding,

        /*
         Command: Get the secure time and trust level.
         Params:
         TPckg< TInt64 > time
         TPckg< TBool > trust level
        */
        ESecureTime,

        /*
        Command: Add a protected rights object into database.
        Params: 
        TDesC8( CID ),
        TPtr8( TRightsObject ),
        TDesC8( REK + CEK + MAC),
        TDes8( Unique ID )
        */ 
        EAddProtectedRecord,
        
        /*
        Command: Add a domain rights object into database.
        Params: 
        TDesC8( CID ),
        TPtr8( TRightsObject ),
        TDesC8( wrapped CEK ),
        TDes8( Unique ID )
        */ 
        EAddDomainRecord,   /* 15 */
        
        
        /*
        Command: Gets the prepared data from the server size
        TPtr8( data )
        */
        EGetPreparedData,
        
        /*
        Command: Adds the domain ro xml representation to the server
        TDesC8( RoId )
        TDesC8( XmlData )
        */
        EAddDomainRO,
        
        /*
        Command: Gets the domain ro xml representation from the server
        TDesC8( RoId )
        */
        EGetDomainRO,
        
        /*
        Command: Deletes the domain ro xml representation from the server
        TDesC8( RoId )
        */
        EDeleteDomainRO,
        
        /*
        Command: Checks whether the item is in Replay Cache.
        TDesC8( ID )
        TPckgC< TTime >
        TPckg< TBool return value >
        */
        EIsInCache,     /* 20 */
        
        /*
        Command: Add an entry to Replay Cache.
        TDesC8( ID )
        TPckgC< TTime >
        */
        EAddToCache,

        /*
        Command: Initialize the session key
        TDesC8( content ID )
        */
        EInitializeKey,

        /*
        Command: Initialize the session key from a group key
        TDesC8( group ID )
        TDesC8( group Key )
        TInt( encryption method )
        */
        EInitializeGroupKey,

        /*
        Command: Decrypt data using the session key
        TDesC8( initialization vector )
        TBool( remove padding? )
        TPtr8( data )
        */
        EDecrypt,

        /*
        Command: Encrypt data using the session key
        TDesC8( initialization vector )
        TBool( add padding? )
        TPtr8( data )
        */
        EEncrypt,   /* 25 */

        /*
        Command: Get the domain RO as XML for the given content ID
        TDesC8( content ID )
        TPtr8( RO as XML )
        */
        EGetDomainRoForCid,

        /*
        Command: Check if consume is possible
        Params: 
        TInt( intent )
        TDesC8( content ID )
        */
        ECheckConsume,
    
        /*
        Command: Delete expired permissions.
        */
        EDeleteExpired,
        
        /*
        Command: Set the estimated arrival time
        Params:
        TDesC8( content ID )
        TTimeIntervalSeconds( time until arrival )
        */
        ESetEstimatedArrival,
        
        /*
        Command: Get the estimated arrival time
        Params:
        TDesC8( content ID )
        TTimeIntervalSeconds( time until arrival )
        */
        EGetEstimatedArrival,   /* 30 */
        
        /*
        Command: Set name
        Params:
        TDesC8( content-ID)
        TDesC( name )
        */
        ESetName,
        
        /*
        Command: Get name
        Params:
        TDesC8( content-ID)
        TPckgBuf< TInt >( data lengt to be returned )
        */
        EGetName,
        
        /*
        Command: Gets the prepared data from the server size
        TPtr( data )
        */
        EGetWideData,
        
        /*
        Command: Cancel
        */
        ECancel,

        /*
	    Command: Get Udt Data
	    TDesC8( UDT Data );
        */
        EGetUdtData,    /* 35 */

        /*
        Command: Initiate Udt
        TDesC8( key )
        */
        EInitiateUdt,

        /*
        Command: Export Orphaned Content-ID list into a file.
        Params:
        TBool( perform file scan )
        */
        EInitOrphanedList,

        /*
        Command: Export Orphaned Content-ID list into a file.
        Params:
        TDes( File name )
        */
        EGetOrphanedList,
        
        /*
        Command: Get FL URI.
        Params:
        TBuf8< 256 >( URI )
        */
        EGetFLUri,
        
        /*
        Command: Encode rights issuer url.
        Params:
        TPtr8( original ), room for result also.        
        */
        EEncodeRightsIssuerField,   /* 40 */
        
        /*
        Command: Decode rights issuer url.
        Params:
        TDesC8( original ).
        TPtr8( decoded )
        */
        EDecodeRightsIssuerField,

        /*
        Command: Set the authentication seed for a content ID
        Params:
        TDesC8( content ID)
        TDesC8( authentication seed )
        */
        ESetAuthenticationSeed,

        /*
        Command: Get the authentication seed for a content ID
        Params:
        TDesC8( content ID)
        TPtr8( authentication seed )
        */
        EGetAuthenticationSeed,
        
        /*
        Command: Verify the MAC
        Params:
        TDesC8( RO element)
        TDesC8( MAC value )
        */
        EVerifyMac,
        
        /*
        Command: Get the supported individuals list
        Params:
        RPointerArray<HBufC8>
        */
        EGetSupportedIndividuals,   /* 45 */
        
        /*
        Command: Stop watching the DCF repository server
        */
        EStopWatching,
        
        /*
        Command: Unwraps public key encrypted MAC and REK keys.
        Params:
        TDesC8 aMacAndRek,
        TKeyTransportScheme& aTransScheme,
        TDesC8 aRiID
        */
        EUnwrapDeviceMacAndRek,
        
        /*
        Command: Unwraps domain key encrypted MAC and REK keys.
        Params:
        TDesC8 aMacAndRek,
        TKeyTransportScheme& aTransScheme,
        TDesC8 aRiID
        */
        EUnwrapDomainMacAndRek,
        
        /*
        Command: Fills the given buffer with random data
        Params:
        TDesC8 aRandomData,
        */
        EGetRandomData,
        
        /*
        Command: Wrap metering data per RiId
        Params:
        TDesC8 aRiId,
        */
        EGetMeteringData,   /* 50 */

        /*
        Command: Delete metering data per RiId
        Params:
        TDesC8 aRiId,
        */        
        EDeleteMeteringData
        };
    }

#endif      // DRMENGINECLIENTSERVER_H

// End of File
