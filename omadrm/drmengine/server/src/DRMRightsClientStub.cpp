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
* Description:  Client side class implementation
*
*/


// INCLUDE FILES
#include <s32file.h>
#include "DRMRightsClient.h"
#include "DRMClientServer.h"
#ifdef _DRM_TESTING
#include "logfile.h"
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
#ifdef CLIENT_STARTS_SERVER
extern TInt DRMServerStarter();
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RDRMRightsClient::RDRMRightsClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMRightsClient::RDRMRightsClient() :
    iPtr( NULL )
    {
    }
    
// -----------------------------------------------------------------------------
// RDRMRightsClient::~RDRMRightsClient
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RDRMRightsClient::~RDRMRightsClient()
    {
    }    

// -----------------------------------------------------------------------------
// RDRMRightsClient::Connect
// Opens connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Connect()
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Close
// Closes the connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::Close() 
    {
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::AddRecord
// Add a new entry to the rights database.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::AddRecord( const TDesC8& aCEK, // Content encryption key
                                  // The rights object which is to be added
                                  const CDRMPermission& aRightsObject, 
                                  const TDesC8& aCID, // Content-ID 
                                  TDRMUniqueID& aID ) // Unique ID, out-parameter
    {
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDBEntriesL
// Get a file name from the server. The file contains the rights objects,
// which are then converted to RPointerArray.
// -----------------------------------------------------------------------------
//
EXPORT_C void RDRMRightsClient::GetDBEntriesL( const TDesC8& aId,
                                      RPointerArray< CDRMPermission >& aRightsList )
    {
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDbEntryL
// Get a single RO from the server.
// -----------------------------------------------------------------------------
//
EXPORT_C CDRMPermission* RDRMRightsClient::GetDbEntryL( const TDesC8& aContentID,
                                                 const TDRMUniqueID& aUniqueID )
    {
    CDRMPermission* object = new (ELeave) CDRMPermission;
    object->iUniqueID = aUniqueID;
    return object;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteDbEntry
// Deletes all rights objects associated with the given UID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteDbEntry( const TDesC8& aContentID )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteDbEntry
// Delete a single rights object identified by given parameters.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteDbEntry( const TDesC8& aContentID,
                                      const TDRMUniqueID& aUniqueID )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::ExportContentIDList
// Overloaded method: requests all content IDs to be put to a file. 
// Assumes that the given descriptor represents a buffer large enough to
// contain the file name.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ExportContentIDList( TDes& aFileName )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::ExportContentIDList
// Overloaded method: requests all content IDs to be put to a file, 
// and then converts the file into RPointerArray.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::ExportContentIDList( RPointerArray< HBufC8 >& aCIDList )
    {
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RDRMRightsClient::GetDecryptionKey
// Fetches the decryption key from the server.
// Uses TR mechanisms. Uses asynchronous C/S call.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::GetDecryptionKey( const TInt aIntent,
                                        const TDesC8& aContentID,
                                        const TBool aUpdate,
                                        TDes8& aKey )
    {
    aKey.Copy(_L("0000000000000000"));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::CheckRights
// Checks if appropriate rights exist for a certain content ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::CheckRights( const TInt aIntent,
                                    const TDesC8& aContentID )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::Count
// Returns the amount of unique content IDs in the database.
// If an error occurs, a negative value is returned (Symbian OS / DRM 3 specific
// error code).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Count()
    {
    return 1;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::DeleteAll
// Empties the database.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::DeleteAll()
	{
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// RDRMRightsClient::Consume()
// Consume the right with specific intent and contentID
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::Consume( const TInt aIntent, const TDesC8& aContentID, const TInt aActionIntent )
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// RDRMRightsClient::CalculatePadding
// Calculate the padding from a data block and a certain content ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RDRMRightsClient::CalculatePadding(
    const TDesC8& aContentID,
    const TDesC8& aLastTwoDataBlocks)
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::FileToListL
// Converts the given file into an array.
// -----------------------------------------------------------------------------
//
void RDRMRightsClient::FileToListL( RFs& aFs,
                                   const TDesC& aFileName,
                                   RPointerArray< CDRMPermission >& aList )
    {
    }

// -----------------------------------------------------------------------------
// RDRMRightsClient::URIFileToArrayL
// Converts the given file into an array.
// -----------------------------------------------------------------------------
//
void RDRMRightsClient::URIFileToArrayL( RFs& aFs,
                                       const TDesC& aFile,
                                       RPointerArray< HBufC8 >& aList )
    {
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
