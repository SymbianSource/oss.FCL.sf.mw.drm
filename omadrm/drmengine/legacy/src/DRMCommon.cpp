/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of common DRM operations
*
*/


// INCLUDE FILES

#include <f32file.h>
#include <s32file.h>
#include <apmstd.h>
#include <wspdecoder.h>
#include <wspencoder.h>

#include <barsc.h>
#include <barsread.h>

#include <DRMCommon.rsg>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "DRMCommon.h"
#include "DRMRightsClient.h"
#include "DcfCommon.h"
#include "Oma1Dcf.h"
#include "oma2dcf.h"
#include "DrmPermission.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TInt KBufferSize = 1024;
_LIT8(KRiUrl, "Rights-Issuer");

// MACROS

// LOCAL CONSTANTS AND MACROS

_LIT8(KFLPrefix, "flk:");
_LIT8(KLDPrefix, "ldf:");
_LIT8(KHeaderNameEnding, ":");
_LIT8(KHeaderEnding, "\r\n");

 #ifdef RD_MULTIPLE_DRIVE
_LIT(KDataTypesFile, "%c:\\system\\data\\DataType.dat");
_LIT(KResourceFile, "%c:\\resource\\drmcommon.rsc");
_LIT(KTempFile, "%c:\\system\\temp\\DataType.tmp");
#else
_LIT(KDataTypesFile, "c:\\system\\data\\DataType.dat");
_LIT(KResourceFile, "z:\\resource\\drmcommon.rsc");
_LIT(KTempFile, "c:\\system\\temp\\DataType.tmp");
#endif


// Version of the client

const TUint8 KClientVersionMajor = 1;
const TUint8 KClientVersionMinor = 1;
const TUint16 KClientVersionBuild = 2;

// Which lowest server version is supported

const TUint8 KServerVersionMajor = 1;
const TUint8 KServerVersionMinor = 2;
const TUint16 KServerVersionBuild = 1;

//_LIT(KNullDate,"00000000:000000.000000");


// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

/*
LOCAL_C TInt CheckContentRightsL(
    const TDesC8& aContentURI,
    TUint32 aRightsSpec);

LOCAL_C TInt IsProtectedContentL(
    const TDesC8& aContent,
    TBool& aProtection);

LOCAL_C TInt GetNewHeaderBuffer(
    HBufC8*& aOldHeaderBuf,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue,
    HBufC8*& aNewHeaderBuf
    );

LOCAL_C TInt WriteNewFile(
    TFileName aFileName,
    HBufC8*& aHeaderBuf,
    TUint32& aFirstPartLength,
    TUint32& aDataLength,
    TUint32& aDataPartPos
    );

LOCAL_C TInt GetContentURIListL(
    RPointerArray<HBufC8>*& aURIList);

LOCAL_C TInt DataTypesCountL(
    TInt& aCount);

LOCAL_C TInt SupportedDataTypeL(
    const TInt aIndex, TDataType& aDataType);

LOCAL_C TInt StaticDataTypesCountL(
    TInt& aCount);

LOCAL_C TInt RegisterDataTypeL(
    const TDataType& aDataType);

LOCAL_C TInt UnRegisterDataTypeL(
    const TInt aIndex);

*/

LOCAL_C void GetActiveRightsL(
    const TDesC8& aContentURI,
    TUint32 aRightsSpec,
    CDRMRights*& aRightsObject);

LOCAL_C TInt CheckFileRightsL(
    const TDesC& aFileName,
    TUint32 aRightsSpec);

LOCAL_C TInt CheckFileRightsL(
    RFile& aFileHandle,
    TUint32 aRightsSpec);

LOCAL_C TInt IsProtectedFileL(
    const TDesC& aFileName,
    TBool& aProtection);

LOCAL_C TInt IsProtectedFileL(
    RFile& aFileHandle,
    TBool& aProtection);

LOCAL_C TInt GetContentInfoL(
    const TDesC8& aContent,
    DRMCommon::TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength);

LOCAL_C TInt GetFileInfoL(
    const TDesC& aFileName,
    DRMCommon::TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength);

LOCAL_C TInt GetFileInfoL(
    RFile& aFileHandle,
    DRMCommon::TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength);

LOCAL_C void GetContentHeaderL(
    const TDesC8& aContent,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue);

LOCAL_C void GetFileHeaderL(
    const TFileName& aFileName,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue);

LOCAL_C void GetFileHeaderL(
    RFile& aFileHandle,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue);

LOCAL_C void SetContentHeaderL(
    HBufC8*& aContent,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue);

LOCAL_C void SetFileHeaderL(
    const TDesC16& aFileName,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue);

LOCAL_C void SetFileHeaderL(
    RFile& aFileHandle,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue);

LOCAL_C void GetSingleRightsObjectL(
    const TDesC8& aContentURI,
    TUint32 aLocalID,
    CDRMRights*& aRightsObject);

LOCAL_C void GetDetailedContentRightsL(
    const TDesC8& aContentURI,
    RPointerArray<CDRMRights>*& aRightsList);

LOCAL_C void GetDetailedFileRightsL(
    const TDesC& aFileName,
    RPointerArray<CDRMRights>*& aRightsList);

LOCAL_C void GetDetailedFileRightsL(
    RFile& aFileHandle,
    RPointerArray<CDRMRights>*& aRightsList);

LOCAL_C TInt RegisterDynamicDataTypeL(
    const TDataType& aDataType);

LOCAL_C TInt UnRegisterDynamicDataTypeL(
    const TInt aIndex);

LOCAL_C TInt CalculatePaddingL(
    COma1Dcf* dcf);

LOCAL_C void DoResetAndDestroy( TAny* aPtr );


LOCAL_C TInt GetFileHandleRead(
    RFs& aFileServer,
    RFile& aFile,
    const TDesC& aFileName );

LOCAL_C void AddParents(
    DRMCommon* aDrmCommon,
    RPointerArray<CDRMRights>& aRights );

// FORWARD DECLARATIONS

using namespace ContentAccess;

// ============================= LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// DoResetAndDestroy
// Does RPointerArray< >->ResetAndDestroy() for the given array aPtr.
// ----------------------------------------------------------------------------
//
LOCAL_C void DoResetAndDestroy( TAny* aPtr )
    {
    ( reinterpret_cast< RPointerArray< CDRMPermission >* >( aPtr ) )->
        ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// GetPermission
// -----------------------------------------------------------------------------
LOCAL_C void GetPermission(
    RDRMRightsClient& aClient,
    const TDesC8& aUri,
    TIntent aIntent,
    CDRMPermission*& aPermission )
    {
    TInt r = KErrNone;
    CDRMPermission* permission = NULL;
    TUint32 reason = 0;

    TRAP( r, permission = aClient.GetActiveRightsL( aIntent, aUri, reason ) );
    if ( permission != NULL )
        {
        if ( aPermission == NULL )
            {
            TRAP_IGNORE( aPermission = CDRMPermission::NewL() );
            }
        aPermission->Merge( *permission );
        delete permission;
        }
    }

// -----------------------------------------------------------------------------
// GetActiveRightsL
// Returns a possible active rights object. Adopts to broken callers who still
// use a bitmask for intent.
// -----------------------------------------------------------------------------
void GetActiveRightsL(
    const TDesC8& aContentURI,
    TUint32 aConstraints,
    CDRMRights*& aRightsObject)
    {
    CDRMPermission* permission = NULL;
    RDRMRightsClient client;
    HBufC8* uri = NULL;

    User::LeaveIfError( client.Connect() );
    CleanupClosePushL( client );

    if ( aConstraints == 0 )
        {
        GetPermission( client, aContentURI, EView, permission );
        GetPermission( client, aContentURI, EPlay, permission );
        GetPermission( client, aContentURI, EExecute, permission );
        GetPermission( client, aContentURI, EPrint, permission );
        }
    else
        {
        GetPermission( client, aContentURI, static_cast<TIntent>( aConstraints ), permission );
        }

    if ( permission != NULL )
        {
        CleanupStack::PushL( permission );
        aRightsObject = CDRMRights::NewL();
        CleanupStack::PushL( aRightsObject );
        uri = aContentURI.AllocL();
        CleanupStack::PushL( uri );
        aRightsObject->SetPermissionL( *permission );
        aRightsObject->SetContentURI( uri );
        CleanupStack::Pop( uri );
        CleanupStack::Pop( aRightsObject );
        CleanupStack::PopAndDestroy( permission );
        }
    else
        {
        aRightsObject = NULL;
        User::Leave( KErrCANoRights );
        }
    CleanupStack::PopAndDestroy(); // client
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C TInt IsProtectedFileL(
    const TDesC& aFileName,
    TBool& aProtection)
    {
    RFs fs;
    RFile file;
    TInt r = KErrNone;
    CDcfCommon* dcf = NULL;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);


    r = GetFileHandleRead( fs, file, aFileName );
    User::LeaveIfError(r);

    CleanupClosePushL(file);
    dcf = CDcfCommon::NewL(file);
    if (dcf != NULL)
        {
        aProtection = ETrue;
        delete dcf;
        }
    else
        {
        aProtection = EFalse;
        }
    CleanupStack::PopAndDestroy(2); // file, fs
    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C TInt IsProtectedFileL(
    RFile& aFileHandle,
    TBool& aProtection)
    {
    TInt r = KErrNone;
    CDcfCommon* dcf = NULL;

    dcf = CDcfCommon::NewL(aFileHandle);
    if (dcf != NULL)
        {
        aProtection = ETrue;
        delete dcf;
        }
    else
        {
        aProtection = EFalse;
        }
    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
TInt GetContentInfoL(
    const TDesC8& aContent,
    DRMCommon::TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength)
    {
    TInt r = KErrNone;
    COma1Dcf* dcf = NULL;
    TRAP(r, dcf = COma1Dcf::NewL(aContent));
    if (dcf != NULL)
        {
        CleanupStack::PushL( dcf );

        if (dcf->iContentID->Left(4).Compare(KLDPrefix) == 0 ||
            dcf->iContentID->Left(4).Compare(KFLPrefix) == 0 ||
            dcf->iRightsIssuerURL == NULL)
            {
            aProtection = DRMCommon::EForwardLocked;
            }
        else
            {
            aProtection = DRMCommon::ESuperDistributable;
            }
        if (dcf->iContentID != NULL)
            {
            aContentURI = dcf->iContentID->AllocL();
            }
        else
            {
            aContentURI = NULL;
            }
        if (dcf->iMimeType != NULL)
            {
            aMIMEType = dcf->iMimeType->AllocL();
            }
        else
            {
            aMIMEType = NULL;
            }

        CalculatePaddingL(dcf);
        aDataLength = dcf->iPlainTextLength;
        CleanupStack::PopAndDestroy();
        }
    else
        {
        aProtection = DRMCommon::ENoDCFFile;
        if (r == KErrArgument)
            {
            r = KErrNone;
            }
        User::LeaveIfError(r);
        }

    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
TInt GetFileInfoL(
    const TDesC& aFileName,
    DRMCommon::TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength)
    {
    RFs fs;
    RFile file;
    TInt r = KErrNone;
    CDcfCommon* dcf = NULL;
    COma2Dcf* dcf2 = NULL;
    CData* data = NULL;

    aProtection = DRMCommon::ENoDCFFile;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    r = GetFileHandleRead( fs, file, aFileName );
    User::LeaveIfError(r);
    CleanupClosePushL(file);

    dcf = CDcfCommon::NewL(file);
    if (dcf != NULL)
        {
        CleanupStack::PushL(dcf);
        if (dcf->iVersion == EOma2Dcf)
            {
            dcf2 = static_cast<COma2Dcf*>(dcf);
            }

        if (dcf->iContentID->Left(4).Compare(KLDPrefix) == 0 ||
            dcf->iContentID->Left(4).Compare(KFLPrefix) == 0 ||
            dcf->iRightsIssuerURL == NULL)
            {
            aProtection = DRMCommon::EForwardLocked;
            }
        else
            {
            aProtection = DRMCommon::ESuperDistributable;
            }
        if (dcf->iContentID != NULL)
            {
            aContentURI = dcf->iContentID->AllocL();
            }
        else
            {
            aContentURI = NULL;
            }
        if (dcf->iMimeType != NULL)
            {
            aMIMEType = dcf->iMimeType->AllocL();
            }
        else
            {
            aMIMEType = NULL;
            }

        // Insert domain RO if it exists
        if (dcf2 != NULL && dcf2->iRightsObjects.Count() > 0)
            {
            // Creating a CData object will insert the domain RO into the RDB
            data = CData::NewL(file, KDefaultContentObject);
            delete data;
            }

        if (dcf->iVersion == EOma1Dcf && !dcf->iPlainTextLengthValid)
            {
            CalculatePaddingL(static_cast<COma1Dcf*>(dcf));
            }

        aDataLength = dcf->iPlainTextLength;
        CleanupStack::PopAndDestroy(); // dcf
        }
    CleanupStack::PopAndDestroy(2); // file, fs
    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
TInt GetFileInfoL(
    RFile& aFileHandle,
    DRMCommon::TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength)
    {
    TInt r = KErrNone;
    CDcfCommon* dcf = NULL;
    COma2Dcf* dcf2 = NULL;
    CData* data = NULL;
    TInt initialPos = 0;

    // Store the initial filePos
    User::LeaveIfError(aFileHandle.Seek(ESeekCurrent, initialPos));

    aProtection = DRMCommon::ENoDCFFile;

    dcf = CDcfCommon::NewL(aFileHandle);
    if (dcf != NULL)
        {
        CleanupStack::PushL(dcf);
        if (dcf->iVersion == EOma2Dcf)
            {
            dcf2 = static_cast<COma2Dcf*>(dcf);
            }

        if (dcf->iContentID->Left(4).Compare(KLDPrefix) == 0 ||
            dcf->iContentID->Left(4).Compare(KFLPrefix) == 0 ||
            dcf->iRightsIssuerURL == NULL)
            {
            aProtection = DRMCommon::EForwardLocked;
            }
        else
            {
            aProtection = DRMCommon::ESuperDistributable;
            }
        if (dcf->iContentID != NULL)
            {
            aContentURI = dcf->iContentID->AllocL();
            }
        else
            {
            aContentURI = NULL;
            }
        if (dcf->iMimeType != NULL)
            {
            aMIMEType = dcf->iMimeType->AllocL();
            }
        else
            {
            aMIMEType = NULL;
            }

        // Insert domain RO if it exists
        if (dcf2 != NULL && dcf2->iRightsObjects.Count() > 0)
            {
            // Creating a CData object will insert the domain RO into the RDB
            data = CData::NewL(aFileHandle, KDefaultContentObject);
            delete data;
            }

        if (dcf->iVersion == EOma1Dcf && !dcf->iPlainTextLengthValid)
            {
            CalculatePaddingL(static_cast<COma1Dcf*>(dcf));
            }

        aDataLength = dcf->iPlainTextLength;
        CleanupStack::PopAndDestroy(); // dcf
        }
    // Restore filePos
    User::LeaveIfError(aFileHandle.Seek(ESeekStart, initialPos));
    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
TInt CheckFileRightsL(
    const TDesC& aFileName,
    TUint32 aRightsSpec)
    {
    RFs fs;
    RFile file;
    CDcfCommon* dcf = NULL;
    TInt r = DRMCommon::ENoRights;
    RDRMRightsClient client;
    TUint32 reason = 0;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    r = GetFileHandleRead(fs, file, aFileName);
    User::LeaveIfError(r);
    CleanupClosePushL(file);

    dcf = CDcfCommon::NewL(file);
    if ( dcf == NULL )
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    r = client.CheckRights(aRightsSpec, *dcf->iContentID, reason);
    CleanupStack::PopAndDestroy(4); // dcf, file, client, fs
    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
TInt CheckFileRightsL(
    RFile& aFileHandle,
    TUint32 aRightsSpec)
    {
    CDcfCommon* dcf = NULL;
    TInt r = DRMCommon::ENoRights;
    RDRMRightsClient client;
    TUint32 reason = 0;

    dcf = CDcfCommon::NewL(aFileHandle);
    if ( dcf == NULL )
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    r = client.CheckRights(aRightsSpec, *dcf->iContentID, reason);
    CleanupStack::PopAndDestroy(2); // dcf, file
    return r;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetSingleRightsObjectL(
    const TDesC8& aContentURI,
    TUint32 aLocalID,
    CDRMRights*& aRightsObject)
    {
    RDRMRightsClient client;
    CDRMPermission* p = NULL;
    CDRMAsset* a = NULL;

    aRightsObject = NULL;
    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);

    aRightsObject = CDRMRights::NewL();
    CleanupStack::PushL(aRightsObject);

    p = client.GetDbEntryL(aContentURI, aLocalID);
    CleanupStack::PushL(p);
    aRightsObject->SetPermissionL(*p);
    CleanupStack::PopAndDestroy(); // p

    a = CDRMAsset::NewLC();
    aRightsObject->SetAssetL(*a);
    CleanupStack::PopAndDestroy();

    aRightsObject->SetContentURIAndLocalID(aContentURI.AllocL(), aLocalID);
    CleanupStack::Pop(); // aRightsObject
    CleanupStack::PopAndDestroy(); // client
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetDetailedContentRightsL(
    const TDesC8& aContentURI,
    RPointerArray<CDRMRights>*& aRightsList)
    {
    RDRMRightsClient client;
    RPointerArray<CDRMPermission> rights;
    CDRMRights* ro = NULL;
    CDRMAsset* a  = NULL;
    TInt i;
    // Need a temporary pointer
    HBufC8* contentId = NULL;

    aRightsList = NULL;
    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);

    client.GetDBEntriesL(aContentURI, rights);

    TCleanupItem cleanup( DoResetAndDestroy, &rights );
    CleanupStack::PushL( cleanup );

    aRightsList = new(ELeave) RPointerArray<CDRMRights>(rights.Count());

    // Push the actual pointer reserved with NewL in
    CleanupStack::PushL( aRightsList );

    // Push the cleanup of the items as well into the cleanup stack
    TCleanupItem cleanup2( DoResetAndDestroy, aRightsList );
    CleanupStack::PushL( cleanup2 );

    for (i = 0; i < rights.Count(); i++)
        {
        // Needs to be pushed to the cleanup stack
        ro = CDRMRights::NewL();
        CleanupStack::PushL(ro);

        // Push to cleanp stack
        a = CDRMAsset::NewLC();

        // Set asset
        ro->SetAssetL(*a);

        // Destroy asset
        CleanupStack::PopAndDestroy(); // Asset

        // Set the permission, copies the object
        ro->SetPermissionL(*rights[i]);

        // take a copy of the content id
        contentId = aContentURI.AllocL();

        // set content uri and local id
        ro->SetContentURIAndLocalID(contentId, rights[i]->iUniqueID);

        // Append the ro to the rights list
        aRightsList->AppendL(ro);

        // Take the ro out of the cleanup stack
        CleanupStack::Pop(ro);
        }

    CleanupStack::Pop(2); // aRightsList ( two items )
    CleanupStack::PopAndDestroy(2); // rights, client
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetDetailedFileRightsL(
    const TDesC& aFileName,
    RPointerArray<CDRMRights>*& aRightsList)
    {

    CDcfCommon* dcf = NULL;
    RFile file;
    RFs fs;
    TInt r = KErrNone;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    r = GetFileHandleRead( fs, file, aFileName );
    User::LeaveIfError(r);
    CleanupClosePushL(file);


    dcf = CDcfCommon::NewL(file);
    if (dcf == NULL)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    GetDetailedContentRightsL(*dcf->iContentID, aRightsList);
    CleanupStack::PopAndDestroy(3); // dcf, file, client

    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetDetailedFileRightsL(
    RFile& aFileHandle,
    RPointerArray<CDRMRights>*& aRightsList)
    {
    CDcfCommon* dcf = NULL;

    dcf = CDcfCommon::NewL(aFileHandle);
    if (dcf == NULL)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    GetDetailedContentRightsL(*dcf->iContentID, aRightsList);
    CleanupStack::PopAndDestroy(); // dcf
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C void  GetNewHeaderBufferL(
    HBufC8*& aOldHeaderBuf,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue,
    HBufC8*& aNewHeaderBuf
    )
    {
    TInt oldHeaderValueLength = 0;
    TInt offset = 0;
    TPtr8 headerPtr(aOldHeaderBuf->Des());
    TPtr8 headerNamePtr(NULL, 0, 0);
    TPtr8 newHeaderPtr(NULL, 0, 0);

    HBufC8* headerName = HBufC8::NewL(aHeaderName.Length() + 1);
    // Find the position of the text in the header
    headerNamePtr.Set(headerName->Des());
    headerNamePtr.Copy(aHeaderName);
    headerNamePtr.Append(KHeaderNameEnding);
    offset = headerPtr.FindF(headerNamePtr);
    delete headerName;
    headerName = NULL;
    if (offset != KErrNotFound)
        {
        // All fields end with CRLF, search for the end of the field and if it is not found
        // return KErrArgument
        oldHeaderValueLength = headerPtr.Right(headerPtr.Length() -
            (offset + aHeaderName.Length() + 1)).Find(KHeaderEnding);
        User::LeaveIfError(oldHeaderValueLength);
        // reserve new buffer for the header, might be useless if the lengths are the same
        aNewHeaderBuf = HBufC8::NewL(headerPtr.Length() - oldHeaderValueLength +
            aHeaderValue.Length());
        newHeaderPtr.Set(aNewHeaderBuf->Des());
        newHeaderPtr.Copy(headerPtr.Left(offset + aHeaderName.Length() + 1));
        newHeaderPtr.Append(aHeaderValue);
        newHeaderPtr.Append(headerPtr.Right(headerPtr.Length() -
            (offset + aHeaderName.Length() + 1 + oldHeaderValueLength)));
        }
    else
        {
        aNewHeaderBuf = HBufC8::NewL(headerPtr.Length() + aHeaderName.Length() +
            aHeaderValue.Length() + 3);
        newHeaderPtr.Set(aNewHeaderBuf->Des());
        newHeaderPtr.Copy(headerPtr);
        newHeaderPtr.Append(aHeaderName);
        newHeaderPtr.Append(KHeaderNameEnding);
        newHeaderPtr.Append(aHeaderValue);
        newHeaderPtr.Append(KHeaderEnding);
        }
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C void WriteNewFileL(
    RFs& aFs,
    RFile& aOriginalFile,
    const TDesC& aOriginalFileName,
    COma1Dcf& aDcf,
    TFileName& aTempFileName )
    {
    TPtrC tempFilePath;
    HBufC8* buffer = NULL;
    RFileWriteStream temp;
    TPtr8 ptr(NULL, 0, 0);
    TInt offset = 0;

    // Find the path of the file to be modified and put the tempfile
    // into that directory
    tempFilePath.Set(aOriginalFileName.Left(
        aOriginalFileName.LocateReverse('\\') + 1));
    User::LeaveIfError(temp.Temp(aFs, tempFilePath, aTempFileName, EFileWrite));
    CleanupClosePushL( temp );

    temp.WriteUint8L(1);
    temp.WriteUint8L(aDcf.iMimeType->Length());
    temp.WriteUint8L(aDcf.iContentID->Length());
    temp.WriteL(*aDcf.iMimeType);
    temp.WriteL(*aDcf.iContentID);
    buffer = TWspPrimitiveEncoder::UintVarL(aDcf.iHeaders->Length());
    CleanupStack::PushL(buffer);
    temp.WriteL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    buffer = TWspPrimitiveEncoder::UintVarL(aDcf.iDataLength);
    CleanupStack::PushL(buffer);
    temp.WriteL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    temp.WriteL(*aDcf.iHeaders);

    offset = aDcf.iOffset;
    User::LeaveIfError(aOriginalFile.Seek(ESeekStart, offset));
    buffer = HBufC8::NewLC(KBufferSize);
    do
        {
        ptr.Set(buffer->Des());
        ptr.SetLength(0);
        aOriginalFile.Read(ptr);
        if (ptr.Length() > 0)
            {
            temp.WriteL(ptr);
            }
        }
    while (ptr.Length() > 0);
    temp.CommitL();
    CleanupStack::PopAndDestroy(); // temp
    CleanupStack::PopAndDestroy(); //buffer
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C void WriteNewFileL(
    RFs& aFs,
    RFile& aOriginalFile,
    COma1Dcf& aDcf)
    {
    TPtrC tempFilePath;
    HBufC8* buffer = NULL;
    RFile tempFileHandle;
    RFileWriteStream temp;
    RFileWriteStream orig;
    TFileName tempFile;
    TPtr8 ptr(NULL, 0, 0);
    TInt offset = 0;
    TInt fileSize = 0;

    // Find the path of the file to be modified and put the tempfile
    // into that directory
    tempFilePath.Set(_L("C:\\"));
    User::LeaveIfError( temp.Temp( aFs, tempFilePath, tempFile,
                        EFileWrite));
    CleanupClosePushL( temp );

    temp.WriteUint8L(1);
    temp.WriteUint8L(aDcf.iMimeType->Length());
    temp.WriteUint8L(aDcf.iContentID->Length());
    temp.WriteL(*aDcf.iMimeType);
    temp.WriteL(*aDcf.iContentID);
    buffer = TWspPrimitiveEncoder::UintVarL(aDcf.iHeaders->Length());
    CleanupStack::PushL(buffer);
    temp.WriteL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    buffer = TWspPrimitiveEncoder::UintVarL(aDcf.iDataLength);
    CleanupStack::PushL(buffer);
    temp.WriteL(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    temp.WriteL(*aDcf.iHeaders);

    offset = aDcf.iOffset;
    User::LeaveIfError(aOriginalFile.Seek(ESeekStart, offset));
    buffer = HBufC8::NewLC(KBufferSize);
    do
        {
        ptr.Set(buffer->Des());
        ptr.SetLength(0);
        aOriginalFile.Read(ptr);
        if (ptr.Length() > 0)
            {
            temp.WriteL(ptr);
            }
        }
    while (ptr.Length() > 0);
    temp.CommitL();

    CleanupStack::PopAndDestroy(2); //buffer, temp
    // Get the size of the temp file


    User::LeaveIfError( tempFileHandle.Open( aFs, tempFile, EFileRead ));
    CleanupClosePushL( tempFileHandle );
    tempFileHandle.Size( fileSize );


    // Set the fileSize of the original file
    User::LeaveIfError( aOriginalFile.SetSize( fileSize ) );
    offset = 0;
    User::LeaveIfError( tempFileHandle.Seek( ESeekStart, offset ) );
    orig.Attach( aOriginalFile );
    CleanupClosePushL( orig );

    buffer = HBufC8::NewLC(KBufferSize);
    // Copy the file over because we can't use other copy things:
    do
        {
        ptr.Set(buffer->Des());
        ptr.SetLength(0);
        tempFileHandle.Read(ptr);
        if (ptr.Length() > 0)
            {
            orig.WriteL(ptr);
            }
        }
    while (ptr.Length() > 0);

    // Write the changes
    orig.CommitL();

    // close the tempfile
    CleanupStack::PopAndDestroy(3); // buffer, tempFileHandle, orig

    // Delete the temp file
    User::LeaveIfError( aFs.Delete( tempFile ) );
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C void SetFileHeaderL(
    const TDesC16& aFileName,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue)
    {
    __UHEAP_MARK;
    TFileName tempFileName;
    HBufC8* newHeaderBuf = NULL;
    COma1Dcf* dcf = NULL;
    RFs fs;
    RFile file;

    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    User::LeaveIfError( GetFileHandleRead( fs, file, aFileName ) );

    CleanupClosePushL(file);
    dcf = COma1Dcf::NewL(file);

    CleanupStack::PushL(dcf);
    GetNewHeaderBufferL(dcf->iHeaders, aHeaderName, aHeaderValue, newHeaderBuf);
    delete dcf->iHeaders;
    dcf->iHeaders = newHeaderBuf;
    dcf->iHeaderLength = newHeaderBuf->Length();
    WriteNewFileL( fs, file, aFileName, *dcf, tempFileName );
    CleanupStack::PopAndDestroy(2); // file, dcf
    User::LeaveIfError( fs.Replace( tempFileName, aFileName ) );
    CleanupStack::PopAndDestroy(); // fs

    __UHEAP_MARKEND;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
LOCAL_C void SetFileHeaderL(
    RFile& aFileHandle,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue)
    {
    HBufC8* newHeaderBuf = NULL;
    COma1Dcf* dcf = NULL;
    RFs fs;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL( fs );

    dcf = COma1Dcf::NewL(aFileHandle);
    if (dcf == NULL)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    GetNewHeaderBufferL(dcf->iHeaders, aHeaderName, aHeaderValue, newHeaderBuf);
    delete dcf->iHeaders;
    dcf->iHeaders = newHeaderBuf;
    dcf->iHeaderLength = newHeaderBuf->Length();
    WriteNewFileL(fs, aFileHandle, *dcf);
    CleanupStack::PopAndDestroy(2); // dcf, fs
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void SetContentHeaderL(
    HBufC8*& aContent,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue)
    {
    HBufC8* newHeaderBuf = NULL;
    HBufC8* newContent = NULL;
    HBufC8* buffer;
    COma1Dcf* dcf = NULL;
    TPtr8 ptr(0, 0);

    dcf = COma1Dcf::NewL(*aContent);
    if (dcf == NULL)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    GetNewHeaderBufferL(dcf->iHeaders, aHeaderName, aHeaderValue, newHeaderBuf);
    delete dcf->iHeaders;
    dcf->iHeaders = newHeaderBuf;
    dcf->iHeaderLength = newHeaderBuf->Length();
    newContent = HBufC8::NewLC(3 +
        dcf->iContentID->Length() + dcf->iMimeType->Length() + 2 * 5 +
        dcf->iHeaderLength + dcf->iDataLength);
    ptr.Set(newContent->Des());
    ptr.Append(1);
    ptr.Append(dcf->iMimeType->Length());
    ptr.Append(dcf->iContentID->Length());
    ptr.Append(*dcf->iMimeType);
    ptr.Append(*dcf->iContentID);
    buffer = TWspPrimitiveEncoder::UintVarL(dcf->iHeaders->Length());
    CleanupStack::PushL(buffer);
    ptr.Append(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    buffer = TWspPrimitiveEncoder::UintVarL(dcf->iDataLength);
    CleanupStack::PushL(buffer);
    ptr.Append(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    ptr.Append(*dcf->iHeaders);
    ptr.Append(aContent->Right(dcf->iDataLength));
    CleanupStack::Pop(newContent);
    CleanupStack::PopAndDestroy(dcf);
    delete aContent;
    aContent = newContent;
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetHeaderL(
    const TDesC8& aContent,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {

    TInt i;
    TInt j;
    TPtrC8 ptr( aContent );

    // Add Room for CRLF and Semicolon:
    HBufC8* buffer = HBufC8::NewMaxLC( aHeaderName.Length() + 3 );
    TPtr8 searchBuf( const_cast<TUint8*>(buffer->Ptr()), 0, buffer->Des().MaxSize() );

    searchBuf.Copy(aHeaderName);
    searchBuf.Append(KHeaderNameEnding);

    // First see if the
    i = ptr.Find(searchBuf);
    User::LeaveIfError( i );

    if( i > 0 )
        {
        // if it's not the first one, use the search buffer:
        // Create the search buffer
        searchBuf.Copy(KHeaderEnding);
        searchBuf.Append(aHeaderName);
        searchBuf.Append(KHeaderNameEnding);

        // First see if the
        i = ptr.Find(searchBuf);
        User::LeaveIfError( i );
        }
    // Move search buffer
    i += searchBuf.Length();

    j = ptr.Mid(i).Find(KHeaderEnding);
    User::LeaveIfError( j );

    aHeaderValue = ptr.Mid(i, j).AllocL();
    CleanupStack::PopAndDestroy(); // buffer
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetContentHeaderL(
    const TDesC8& aContent,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {
    COma1Dcf* dcf = NULL;

    dcf = COma1Dcf::NewL(aContent);
    CleanupStack::PushL(dcf);
    GetHeaderL(*dcf->iHeaders, aHeaderName, aHeaderValue);
    CleanupStack::PopAndDestroy(); // dcf
    }
// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetFileHeaderL(
    const TFileName& aFileName,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {
    RFs fs;
    RFile file;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    User::LeaveIfError( GetFileHandleRead( fs, file, aFileName ) );

    CleanupClosePushL(file);
    GetFileHeaderL(file, aHeaderName, aHeaderValue);
    CleanupStack::PopAndDestroy(2); // fs, file
    }

// -----------------------------------------------------------------------------
//
//
// Parameters:
//
// Returns:
// -----------------------------------------------------------------------------
void GetFileHeaderL(
    RFile& aFileHandle,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {
    CDcfCommon* dcf = NULL;

    aHeaderValue = NULL;
    dcf = CDcfCommon::NewL(aFileHandle);
    if (dcf == NULL)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PushL(dcf);
    if (dcf->iVersion == EOma1Dcf)
        {
        GetHeaderL(*static_cast<COma1Dcf*>(dcf)->iHeaders, aHeaderName,
            aHeaderValue);
        }
    else
        {
        if (aHeaderName.Compare(KRiUrl) == 0 && dcf->iRightsIssuerURL != NULL)
            {
            aHeaderValue = dcf->iRightsIssuerURL->AllocL();
            }
        }
    CleanupStack::PopAndDestroy(); // dcf
    }

// -----------------------------------------------------------------------------
// DynamicDataTypesCount: Returns the number of supported dynamic datatypes
//
// Parameters:
//      aCount: out parameter for the number of dynamically registered data
//          types
//
// Returns:
//      DRMCommon::EOk: Count returned successfully
// -----------------------------------------------------------------------------
LOCAL_C TInt DynamicDataTypesCount(
    TInt& aCount)
    {
    TInt err = 0;
    TDataType type;
    RFs fs;
    RFileReadStream reader;

    aCount = 0;
    err = fs.Connect();
    if (!err)
        {

#ifndef RD_MULTIPLE_DRIVE

    err = reader.Open(fs, KDataTypesFile, EFileRead);

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    TFileName dataTypesFile;
    dataTypesFile.Format( KDataTypesFile, (TUint)driveLetter );

    err = reader.Open(fs, dataTypesFile, EFileRead);

#endif

        if (err == KErrNotFound)
            {
            err = DRMCommon::EOk;
            }
        else
            {
            while (err == KErrNone)
                {
                TRAP(err, (reader >> type));
                if (err == KErrNone) aCount++;
                }

            if (err == KErrEof)
                {
                err = DRMCommon::EOk;
                }

            reader.Release();
            reader.Close();
            }
        }

    fs.Close();

    return err;
    }

// -----------------------------------------------------------------------------
// SupportedDynamicDataType: Returns a registered dynamic datatype
//
// Parameters:
//      aIndex: index of the dynamic data type
//      aDataType: out parameter for the data type
//
// Returns:
//      DRMCommon::EOk: Data type returned successfully
//      KErrEof: Data type not found
// -----------------------------------------------------------------------------
LOCAL_C TInt SupportedDynamicDataType(
    const TInt aIndex,
    TDataType& aDataType)
    {
    TInt err = KErrNone;
    TInt i;
    RFs fs;
    TDataType type;
    RFileReadStream reader;

    err = fs.Connect();
    if (!err)
        {

#ifndef RD_MULTIPLE_DRIVE

        err = reader.Open(fs, KDataTypesFile, EFileRead);

#else //RD_MULTIPLE_DRIVE

        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
        fs.DriveToChar( driveNumber, driveLetter );

        TFileName dataTypesFile;
        dataTypesFile.Format( KDataTypesFile, (TUint)driveLetter );

        err = reader.Open(fs, dataTypesFile, EFileRead);

#endif

        if (!err)
            {
            i = -1;
            while (err == 0 && i != aIndex)
                {
                TRAP(err, (reader >> type));
                i++;
                }

            if (!err)
                {
                aDataType = type;
                }

            reader.Release();
            reader.Close();
            }
        }

    fs.Close();

    return err;
    }

// -----------------------------------------------------------------------------
// SupportedStaticDataType: Returns a static DRM datatype
//
// Parameters:
//      aIndex: index of the dynamic data type
//      aDataType: out parameter for the data type
//
// Returns:
//      DRMCommon::EOk: Data type returned successfully
//      KErrEof: Data type not found
// -----------------------------------------------------------------------------
LOCAL_C TInt SupportedStaticDataType(
    const TInt aIndex,
    TDataType& aDataType)
    {
    TInt err = 0;
    TInt index = 0;
    TInt length = 0;
    TInt i = 0;
    RFs fs;
    RResourceFile resourceFile;
    HBufC8* res = NULL;
    TResourceReader theReader;

    err = fs.Connect();
    if (err)
        {
        fs.Close();
        return err;
        }

#ifndef RD_MULTIPLE_DRIVE

    TRAP(err, resourceFile.OpenL(fs, KResourceFile));

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    TFileName resFile;
    resFile.Format( KResourceFile, (TUint)driveLetter );

    TRAP(err, resourceFile.OpenL(fs, resFile));

#endif
    if (err)
        {
        fs.Close();
        return err;
        }

    TRAP(err, (res = resourceFile.AllocReadL(DATATYPE)));
    if (err)
        {
        resourceFile.Close();
        fs.Close();
        return err;
        }

    theReader.SetBuffer(res);
    index = aIndex + 1;
    TPtrC temp16(theReader.ReadTPtrC(index, res));
    length = temp16.Length();
    HBufC8* temp = NULL;
    temp = HBufC8::NewMax(length);
    if (temp)
        {
        TPtr8 ptr(temp->Des());
        ptr.SetLength(length);
        for (i = 0; i < length; i++)
            {
            ptr[i] = (unsigned char) temp16[i];
            }
        aDataType = TDataType(ptr);
        }
    else
        {
        err = KErrNoMemory;
        }

    if (temp)
        {
        delete temp;
        }

    temp = NULL;
    resourceFile.Close();
    if (res)
        {
        delete res;
        }

    res = NULL;
    fs.Close();
    return err;
    }

// -----------------------------------------------------------------------------
// UnRegisterDynamicDataTypeL: Unregister one of the dynamically addable data
//      types
//
// Parameters:
//      aIndex: index of the data type in the list of dynamic types
//
// Returns:
//      DRMCommon::EOK: Data type removed successfully
// -----------------------------------------------------------------------------
LOCAL_C TInt UnRegisterDynamicDataTypeL(
    const TInt aIndex)
    {
    TInt r;
    TInt i;
    TDataType type;
    RFileWriteStream writer;
    RFileReadStream reader;
    RFs fs;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Open the data types file

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(reader.Open(fs, KDataTypesFile, EFileRead));

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    TFileName dataTypesFile;
    dataTypesFile.Format( KDataTypesFile, (TUint)driveLetter );

    User::LeaveIfError(reader.Open(fs, dataTypesFile, EFileRead));

#endif

    CleanupReleasePushL(reader);

    // Create and open a replacement file

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(writer.Replace(fs, KTempFile, EFileWrite));

#else //RD_MULTIPLE_DRIVE

    TFileName tempFile;
    tempFile.Format( KTempFile, (TUint)driveLetter );

    User::LeaveIfError(writer.Replace(fs, tempFile, EFileWrite));

#endif

    CleanupReleasePushL(writer);

    // Write all dynamic types into the replacement file, excluding the
    // data type to be removed

    r = KErrNone;
    for (i = 0; r == KErrNone; i++)
        {
        TRAP(r, reader >> type);
        if (r == KErrNone && i != aIndex)
            {
            writer << type;
            writer.CommitL();
            }
        }
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();

    // Replace the data type file with the replacement file

#ifndef RD_MULTIPLE_DRIVE

    fs.Replace(KTempFile, KDataTypesFile);

#else //RD_MULTIPLE_DRIVE

    fs.Replace(tempFile, dataTypesFile);

#endif


    CleanupStack::PopAndDestroy();

    return DRMCommon::EOk;
    }

// -----------------------------------------------------------------------------
// RegisterDynamicDataTypeL: Register a new dynamically addable data type
//
// Parameters:
//      aDataType: new type to be added
//
// Returns:
//      DRMCommon::EOK: Data type removed successfully
// -----------------------------------------------------------------------------
LOCAL_C TInt RegisterDynamicDataTypeL(const TDataType& aDataType)
    {
    TInt err = KErrNone;
    TDataType type;
    RFileWriteStream writer;
    RFileReadStream reader;
    RFs fs;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Create and open a replacement file

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(writer.Replace(fs, KTempFile, EFileWrite));

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    TFileName tempFile;
    tempFile.Format( KTempFile, (TUint)driveLetter );

    User::LeaveIfError(writer.Replace(fs, tempFile, EFileWrite));

#endif

    CleanupReleasePushL(writer);

    // Write the new data type into the replacement file

    writer << aDataType;
    writer.CommitL();

    // Write all other dynamic types into the replacement file

#ifndef RD_MULTIPLE_DRIVE

    err = reader.Open(fs, KDataTypesFile, EFileRead);

#else //RD_MULTIPLE_DRIVE

    TFileName dataTypesFile;
    dataTypesFile.Format( KDataTypesFile, (TUint)driveLetter );

    err = reader.Open(fs, dataTypesFile, EFileRead);

#endif

    if (err == KErrNone)
        {
        CleanupReleasePushL(reader);
        while (err == KErrNone)
            {
            TRAP(err, (reader >> type, writer << type, writer.CommitL()));
            }
        CleanupStack::PopAndDestroy();
        }

    CleanupStack::PopAndDestroy();

    // Replace the data type file with the replacement file

#ifndef RD_MULTIPLE_DRIVE

    fs.Replace(KTempFile, KDataTypesFile);

#else //RD_MULTIPLE_DRIVE

    fs.Replace(tempFile, dataTypesFile);

#endif

    CleanupStack::PopAndDestroy();

    return DRMCommon::EOk;
    }

TInt CalculatePaddingL(COma1Dcf* dcf)
    {
    RDRMRightsClient rdb;
    TBuf8<KDCFKeySize * 2> blocks;
    TInt pos;
    TBool blocksValid = ETrue;

    if (!dcf->iPlainTextLengthValid)
        {
        User::LeaveIfError(rdb.Connect());
        CleanupClosePushL(rdb);
        if (dcf->iFile.SubSessionHandle() != KNullHandle)
            {
            pos = dcf->iOffset + dcf->iDataLength - 2 * KDCFKeySize;
            dcf->iFile.Seek(ESeekStart, pos);
            dcf->iFile.Read(blocks);
            }
        else if (dcf->iData->Length() >= dcf->iOffset + dcf->iDataLength)
            {
            blocks.Copy(&(dcf->iData->Des())[dcf->iOffset +
                dcf->iDataLength - 2 * KDCFKeySize],
                KDCFKeySize * 2);
            }
        else
            {
            blocksValid = EFalse;
            }

        if (blocksValid && rdb.InitializeKey(*dcf->iContentID) == KErrNone)
            {
            dcf->iPadding = rdb.CalculatePadding(blocks);
            if (dcf->iPadding >= 0)
                {
                dcf->iPlainTextLength -= dcf->iPadding;
                dcf->iPlainTextLengthValid = ETrue;
                }
            }
        CleanupStack::PopAndDestroy(); // rdb
        }

    return dcf->iPadding;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DRMCommon::DRMCommon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
EXPORT_C DRMCommon::DRMCommon(void)
    {
    }

// -----------------------------------------------------------------------------
// DRMCommon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
EXPORT_C void DRMCommon::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// DRMCommon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C DRMCommon* DRMCommon::NewL()
    {
    DRMCommon* self = new(ELeave) DRMCommon();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


// Destructor
EXPORT_C DRMCommon::~DRMCommon()
    {
    }

// -----------------------------------------------------------------------------
// DRMCommon::CheckContentRights
// Checks if the give rights for a specific content URI are available.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::CheckContentRights(
    const TDesC8& aContentID,
    TUint32 aRightsSpec)
    {
    TInt r = ENoRights;
    RDRMRightsClient client;
    TUint32 reason = 0;

    if (client.Connect() == KErrNone)
        {
        r = client.CheckRights(aRightsSpec, aContentID, reason);
        client.Close();
        }
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::CheckFileRights
// Checks if the give rights for a specific content URI are available by
// opening a given file and reading the content URI from there.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::CheckFileRights(
    const TDesC& aFileName,
    TUint32 aRightsSpec)
    {
    TInt r = KErrNone;
    TInt retValue = KErrNone;
    TRAP(r, retValue = CheckFileRightsL(aFileName, aRightsSpec));
    if (r != KErrNone)
        {
        return r;
        }
    return retValue;
    }

// -----------------------------------------------------------------------------
// DRMCommon::CheckFileRights
// Checks if the give rights for a specific content URI are available by
// opening a given file and reading the content URI from there.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::CheckFileRights(
    RFile& aFileHandle,
    TUint32 aRightsSpec)
    {
    TInt r = KErrNone;
    TInt retValue = KErrNone;
    TRAP(r, retValue = CheckFileRightsL(aFileHandle, aRightsSpec));
    if (r != KErrNone)
        {
        return r;
        }
    return retValue;
    }

// -----------------------------------------------------------------------------
// DRMCommon::IsProtectedContent
// Check if a memory buffer contains DRM proteced content by looking for a
// predefined header string.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::IsProtectedContent(
    const TDesC8& aContent,
    TBool& aProtection)
    {
    if (COma1Dcf::IsValidDcf(aContent) || COma2Dcf::IsValidDcf(aContent))
        {
        aProtection = ETrue;
        }
    else
        {
        aProtection = EFalse;
        }
    return EOk;
    }

// -----------------------------------------------------------------------------
// DRMCommon::IsProtectedFile
// Check if a file contains DRM proteced content by looking for a
// predefined header string.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::IsProtectedFile(
    const TDesC& aFileName,
    TBool& aProtection)
    {
    TInt r = KErrNone;
    TRAP(r, IsProtectedFileL(aFileName, aProtection));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::IsProtectedFile
// Check if a file contains DRM proteced content by looking for a
// predefined header string.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::IsProtectedFile(
    RFile& aFileHandle,
    TBool& aProtection)
    {
    TInt r = KErrNone;
    TRAP(r, IsProtectedFileL(aFileHandle, aProtection));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetContentInfo
// Returns DRM information about a memory buffer.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetContentInfo(
    const TDesC8& aContent,
    TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength)
    {
    TInt r = KErrNone;
    TRAP(r, GetContentInfoL(aContent, aProtection, aMIMEType, aContentURI,
        aDataLength));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetFileInfo
// Returns DRM information about a file.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetFileInfo(
    const TDesC& aFileName,
    TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength)
    {
    TInt r = KErrNone;
    TRAP(r, GetFileInfoL(aFileName, aProtection, aMIMEType, aContentURI,
        aDataLength));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetFileInfo
// Returns DRM information about a file.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetFileInfo(
    RFile& aFileHandle,
    TContentProtection& aProtection,
    HBufC8*& aMIMEType,
    HBufC8*& aContentURI,
    TUint& aDataLength)
    {
    TInt r = KErrNone;
    TRAP(r, GetFileInfoL(aFileHandle, aProtection, aMIMEType, aContentURI,
        aDataLength));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetContentHeader
// Returns an optional header from a memory buffer containing encrypted content.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetContentHeader(
    const TDesC8& aContent,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {
    TInt r = KErrNone;

    TRAP(r, GetContentHeaderL(aContent, aHeaderName, aHeaderValue));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetFileHeader
// Returns an optional header from a file containing encrypted content.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetFileHeader(
    const TFileName& aFileName,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {
    TInt r = KErrNone;

    TRAP(r, GetFileHeaderL(aFileName, aHeaderName, aHeaderValue));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetFileHeader
// Returns an optional header from a file containing encrypted content.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetFileHeader(
    RFile& aFileHandle,
    const TDesC8& aHeaderName,
    HBufC8*& aHeaderValue)
    {
    TInt r = KErrNone;

    TRAP(r, GetFileHeaderL(aFileHandle, aHeaderName, aHeaderValue));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMAuthenticated::SetContentHeader
// Sets the specified optional header field of a DCF buffer.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::SetContentHeader(
    HBufC8*& aContent,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue)
    {
    TInt r = KErrNone;

    TRAP(r, SetContentHeaderL(aContent, aHeaderName, aHeaderValue));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::SetContentHeader
// Sets the specified optional header field of a DCF buffer.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::SetFileHeader(
    const TDesC16& aFileName,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue)
    {
    TInt r = KErrNone;

    TRAP(r, SetFileHeaderL(aFileName, aHeaderName, aHeaderValue));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::SetContentHeader
// Sets the specified optional header field of a DCF buffer.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::SetFileHeader(
    RFile& aFileHandle,
    const TDesC8& aHeaderName,
    const TDesC8& aHeaderValue)
    {
    TInt r = KErrNone;

    TRAP(r, SetFileHeaderL(aFileHandle, aHeaderName, aHeaderValue));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetSingleRightsObject
// Looks up the rights object using the content URI and the local ID.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetSingleRightsObject(
    const TDesC8& aContentURI,
    TUint32 aLocalID,
    CDRMRights*& aRightsObject)
    {
    TInt r = KErrNone;
    TRAP(r, GetSingleRightsObjectL(aContentURI, aLocalID, aRightsObject));
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetDetailedContentRights
// Returns all rights objects for a content URI
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetDetailedContentRights(
    const TDesC8& aContentURI,
    RPointerArray<CDRMRights>*& aRightsList)
    {
    TInt r = KErrNone;

    TRAP(r, GetDetailedContentRightsL(aContentURI, aRightsList));
    if (r == KErrNone)
        {
        AddParents( const_cast<DRMCommon*>(this), *aRightsList );
        }
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetDetailedFileRights
// Returns all rights objects for a content URI from a given file
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetDetailedFileRights(
    const TDesC& aFileName,
    RPointerArray<CDRMRights>*& aRightsList)
    {
    TInt r = KErrNone;

    TRAP(r, GetDetailedFileRightsL(aFileName, aRightsList));
    if (r == KErrNone)
        {
        AddParents( const_cast<DRMCommon*>(this), *aRightsList );
        }
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetDetailedFileRights
// Returns all rights objects for a content URI from a given file
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetDetailedFileRights(
    RFile& aFileHandle,
    RPointerArray<CDRMRights>*& aRightsList)
    {
    TInt r = KErrNone;

    TRAP(r, GetDetailedFileRightsL(aFileHandle, aRightsList));
    if (r == KErrNone)
        {
        AddParents( const_cast<DRMCommon*>(this), *aRightsList );
        }
    return r;
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetActiveRights
// Returns a possible active rights object
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetActiveRights(
    const TDesC8& aContentURI,
    TUint32 aConstraints,
    CDRMRights*& aRightsObject)
    {
    TInt r = KErrNone;

    TRAP( r, GetActiveRightsL( aContentURI, aConstraints, aRightsObject ) );
    if(r != KErrNone)
        {
        return DRMCommon::ENoRights;
        }
    else
        {
        CDRMRights::TRestriction restriction;
        CDRMRights::TExpiration expiration;
        TUint32 constType(0);
        aRightsObject->GetRightsInfo(aConstraints, restriction, expiration, constType);
        return restriction;
        }
    }

// -----------------------------------------------------------------------------
// DRMCommon::GetContentURIList
// Returns a list of all content URIs that have rights in the rights database.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::GetContentURIList(
    RPointerArray<HBufC8>*& aURIList)
    {
    RDRMRightsClient client;
    TInt error = client.Connect();

    aURIList = NULL;
    if (!error)
        {
        aURIList = new RPointerArray<HBufC8>(10);
        if (aURIList)
            {
            error = client.ExportContentIDList(*aURIList);
            if (error)
                {
                aURIList->ResetAndDestroy();
                aURIList->Close();
                delete aURIList;
                aURIList = NULL;
                }
            }
        else
            {
            error = KErrNoMemory;
            }
        client.Close();
        }

    return error;
    }

// -----------------------------------------------------------------------------
// DRMCommon::Connect
// Does nothing
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::Connect()
    {
    RDRMRightsClient client; // Used to start RightsServer in bootup
    TInt ignore = 0;                 // error will be ignored, if it fails to start the
                             // rights server there is nothing we can really
                             // do about it, and normally this would work and
                             // return AOk, just used because of the SkinSrv
                             // Bootup thing
    ignore = client.Connect();        // Called by SkinSrv during bootup
        if( ignore )
            {
            // The error shouldn't matter since it will be retried
            }
    client.Close();
    return EOk;
    }

// -----------------------------------------------------------------------------
// DRMCommon::Disconnect
// Does nothing
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::Disconnect()
    {
    return EOk;
    }

// -----------------------------------------------------------------------------
// DRMCommon::SupportedDRMMethods
// Returns the implementation level of the DRM system.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::SupportedDRMMethods(
    TInt& aDRMMethod, TOMALevel& aOMALevel)
    {
    aDRMMethod =
        DRMCommon::EForwardLock |
        DRMCommon::ECombinedDelivery |
        DRMCommon::ESeparateDelivery |
        DRMCommon::ESuperDistribution;
#ifdef __DRM_OMA2
    aOMALevel = EOMA_2_0;
#else
    aOMALevel = EOMA_1_0;
#endif
    return EOk;
    }

// -----------------------------------------------------------------------------
// DRMCommon::Version
// Returns the client side API version.
// -----------------------------------------------------------------------------
EXPORT_C TVersion DRMCommon::Version()
    {
    return TVersion(KClientVersionMajor,
        KClientVersionMinor,
        KClientVersionBuild);
    }

// -----------------------------------------------------------------------------
// DRMCommon::ServerVersion
// Returns the version of the DRM engine.
// -----------------------------------------------------------------------------
EXPORT_C TVersion DRMCommon::ServerVersion()
    {
    return TVersion(KServerVersionMajor,
        KServerVersionMinor,
        KServerVersionBuild);
    }

// -----------------------------------------------------------------------------
// DRMCommon::DataTypesCount
// Returns the number of MIME types the DRM system understands.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::DataTypesCount(
    TInt& aCount)
    {
    TInt err = 0;
    TInt staticTotal = 0;
    TInt dynamicTotal = 0;

    err = StaticDataTypesCount(staticTotal);
    if (err)
        {
        return err;
        }

    err = DynamicDataTypesCount(dynamicTotal);
    if (err)
        {
        return err;
        }

    aCount = staticTotal + dynamicTotal;
    return err;
    }

// -----------------------------------------------------------------------------
// DRMCommon::SupportedDataType
// Returns a specific DRM enabled MIME type.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::SupportedDataType(
    const TInt aIndex,
    TDataType& aDataType)
    {
    TInt err = KErrNone;
    TInt total = 0;
    TInt stat = 0;

    err = DataTypesCount(total);
    if (!err) err = StaticDataTypesCount(stat);
    if (!err)
        {
        if (0 <= aIndex && aIndex < stat)
            {
            err = SupportedStaticDataType(aIndex, aDataType);
            }
        else if (stat <= aIndex && aIndex < total)
            {
            err = SupportedDynamicDataType(aIndex - stat, aDataType);
            }
        else
            {
            err = KErrArgument;
            }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// DRMCommon::StaticDataTypesCount
// Returns the number of supported built-in datatypes.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::StaticDataTypesCount(
    TInt& aCount)
    {
    TInt err = 0;
    RFs fs;
    RResourceFile resourceFile;
    TResourceReader theReader;

    err = fs.Connect();

    if (err)
        {
        fs.Close();
        return err;
        }

#ifndef RD_MULTIPLE_DRIVE

    TRAP(err, resourceFile.OpenL(fs, KResourceFile));

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    TFileName resFile;
    resFile.Format( KResourceFile, (TUint)driveLetter );

    TRAP(err, resourceFile.OpenL(fs, resFile));

#endif
    if (err)
        {
        fs.Close();
        return err;
        }

    HBufC8* res = NULL;
    TRAP(err, (res = resourceFile.AllocReadL(DATATYPE)));
    if (err)
        {
        resourceFile.Close();
        fs.Close();
        return err;
        }


    theReader.SetBuffer(res);
    aCount = theReader.ReadInt8();

    delete res;
    res = NULL;
    resourceFile.Close();
    fs.Close();
    return err;
    }

// -----------------------------------------------------------------------------
// DRMCommon::RegisterDataType
// Register a MIME type as being handled by the DRM system.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::RegisterDataType(
    const TDataType& aDataType)
    {
    TInt total = 0;
    TInt err = KErrNone;
    TInt i;
    TDataType type;

    if (aDataType.Des().Length() > 0)
        {
        err = DataTypesCount(total);
        for (i = 0 ; err == KErrNone && i < total ; i++)
            {
            err = SupportedDataType(i, type);
            if (type == aDataType)
                {
                err = KErrAlreadyExists;
                }
            }

        if (!err)
            {
            TRAP(err, RegisterDynamicDataTypeL(aDataType));
            }
        }
    else
        {
        err = KErrArgument;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// DRMCommon::UnRegisterDataType
// Unregister a MIME type as being handled by the DRM system.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::UnRegisterDataType(
    const TInt aIndex)
    {
    TInt r;
    TInt count;
    TInt total;

    StaticDataTypesCount(count);
    r = DataTypesCount(total);
    if (r == KErrNone && aIndex >= count && aIndex < total)
        {
        TRAP(r, UnRegisterDynamicDataTypeL(aIndex - count));
        }
    else
        {
        r = KErrArgument;
        }
    return r;
    }


// -----------------------------------------------------------------------------
// DRMCommon::MergeParentAndChild
// Merge rights with their parent rights
// -----------------------------------------------------------------------------
EXPORT_C void DRMCommon::MergeParentAndChild(CDRMRights* /*aRights*/)
    {
    return;
    /*
    HBufC8* parent = aRights->GetPermission().iParentUID;
    if (parent != NULL)
        {
        RPointerArray<CDRMRights>* parents;
        if (GetDetailedContentRights(*parent, parents) == KErrNone &&
            parents->Count() > 0)
            {
            TBool merged = EFalse;
            for (TInt i = 0; !merged && i < parents->Count(); i++)
                {
                CDRMRights::TRestriction restriction;
                CDRMRights::TExpiration expiration;
                TUint32 constraints;
                aRights->GetRightsInfo(EUnknown, restriction, expiration,
                    constraints);
                if ( expiration == CDRMRights::EValidRights)
                    {
                    aRights->Merge(*(*parents)[i]);
                    merged = ETrue;
                    }
                }
            parents->ResetAndDestroy();
            delete parents;
            }
        }
    */
    }




// -----------------------------------------------------------------------------
// GetFileHandle
// Get a file name trying to open it in the order required, which is:
// 1)  EFileShareReadersOrWriters
// 2)  EFileShareAny
// 3)  EFileShareReadersOnly
// -----------------------------------------------------------------------------
LOCAL_C TInt GetFileHandleRead(
    RFs& aFileServer,
    RFile& aFile,
    const TDesC& aFileName )
    {
    TInt error = KErrNone;

    // 1) Try to open in EFileShareReadersOrWriters
    error = aFile.Open( aFileServer, aFileName, EFileRead | EFileShareReadersOrWriters );
    if( error != KErrNone )
        {
        // 2) Try to open in EFileShareAny
        error = aFile.Open( aFileServer, aFileName, EFileRead | EFileShareAny );
        if( error != KErrNone )
            {
            // 3) Try to open in EFileShareReadersOnly
            error = aFile.Open( aFileServer, aFileName, EFileRead | EFileShareReadersOnly );
            }
        }
    return error;
    };

// -----------------------------------------------------------------------------
// GetFileHandle
// Get a file name trying to open it in the order required, which is:
// 1)  EFileShareReadersOrWriters
// 2)  EFileShareAny
// 3)  EFileShareReadersOnly
// -----------------------------------------------------------------------------
LOCAL_C void AddParents( DRMCommon* aDrmCommon,
                         RPointerArray<CDRMRights>& aRights )
    {
    HBufC8* parent = NULL;
    RPointerArray<CDRMRights>* parents = NULL;
    TInt error = KErrNone;
    RPointerArray<HBufC8> usedParents;


    for( TInt i = 0; i < aRights.Count(); i++ )
        {
        parent = aRights[i]->GetPermission().iParentUID;

        for(TInt counter = 0; counter < usedParents.Count(); counter++ )
            {
            if( parent && !usedParents[counter]->Compare( *parent ) )
                {
                parent = NULL;
                }
            }

        if( parent != NULL )
            {
            TRAP( error, usedParents.AppendL( parent ) );

            if( aDrmCommon->GetDetailedContentRights(parent->Des(), parents) == KErrNone )
                {
                for( TInt j = parents->Count()-1; j >= 0;j-- )
                    {
                    TRAP( error, aRights.AppendL( (*parents)[j] ) );
                    (*parents)[j] = NULL;
                    parents->Remove(j);
                    }
                parents->ResetAndDestroy();
                delete parents;
                parents = NULL;
                }

            }
        }
    usedParents.Reset();
    usedParents.Close();
    }

// -----------------------------------------------------------------------------
// DRMCommon::MapErrorCode
// Remaps DRM core specific error codes to DRM API error codes.
// -----------------------------------------------------------------------------
EXPORT_C TInt DRMCommon::MapErrorCode(
    const TInt /* aCode */)
    {
    return EUnsupported;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
