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
* Description:  Implementation of the DRM access functions that are available
*                to authenticated clients.
*
*/


// INCLUDE FILES

#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <s32mem.h>
#include <apmstd.h>
#include <symmetric.h>
#include "Oma1DcfCreator.h"
#include "DRMRightsClient.h"
#include "DrmPermission.h"
#include "DrmConstraint.h"
#include "DRMCommon.h"
#include "DrmKeyStorage.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

static const TInt KMaxEncryptionSize = 2048;

// MACROS

// LOCAL CONSTANTS AND MACROS

_LIT8(KLDContentURI, "ldf:31415926535@localhost");
_LIT8(KLDFHeader, "Encryption-Method:AES128CBC;padding=RFC2630\x0d\x0a");

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COma1DcfCreator::COma1DcfCreator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
EXPORT_C COma1DcfCreator::COma1DcfCreator(void)
    {
    iEncryptionStream = NULL;
    iEncryptionBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// COma1DcfCreator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void COma1DcfCreator::ConstructL()
    {
    iEncryptionBuffer = HBufC8::NewL(KMaxEncryptionSize);
    }

// -----------------------------------------------------------------------------
// COma1DcfCreator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C COma1DcfCreator* COma1DcfCreator::NewL()
    {
    COma1DcfCreator* self = new COma1DcfCreator();

    User::LeaveIfNull(self);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
EXPORT_C COma1DcfCreator::~COma1DcfCreator()
    {
    delete iEncryptionBuffer;
    iRdb.Close();
    }

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// COma1DcfCreator::
//
// -----------------------------------------------------------------------------
EXPORT_C void COma1DcfCreator::EncryptInitializeL(
    RWriteStream& anOutput,
    const TDesC8& aMIMEType,
    CDRMRights* aRightsObject)
    {
    const TUint KVersion = 1;
    CDRMPermission *rights = CDRMPermission::NewLC();
    TInt err = 0;
    TDRMUniqueID id = 0;
    TBuf8<KDCFKeySize> key;
    TPtrC8 uri(0, 0);

    User::LeaveIfError(iRdb.Connect());
    iBytesWritten = 0;

    // generate rights
    if (aRightsObject)
        {
        uri.Set(aRightsObject->GetAsset().iUid->Des());
        err = iRdb.InitializeKey(uri);
        if (err == KErrCANoRights)
            {
            key.SetLength(KDCFKeySize);
            iRdb.GetRandomDataL(key);
            User::LeaveIfError(iRdb.AddRecord(key, aRightsObject->GetPermission(),
                uri, id));
            User::LeaveIfError(iRdb.InitializeKey(uri));
            }
        else
            {
            User::LeaveIfError(err);
            }
        }
    else
        {
        uri.Set(KLDContentURI);
        err = iRdb.InitializeKey(uri);
        if (err == KErrCANoRights)
            {
            rights->iAvailableRights = ERightsAll;
            rights->iTopLevel->iActiveConstraints = EConstraintNone;
            rights->iPlay->iActiveConstraints = EConstraintNone;
            rights->iDisplay->iActiveConstraints = EConstraintNone;
            rights->iPrint->iActiveConstraints = EConstraintNone;
            rights->iExecute->iActiveConstraints = EConstraintNone;
            rights->iUniqueID = 0;
            key.SetLength(KDCFKeySize);
            iRdb.GetRandomDataL(key);
            User::LeaveIfError(iRdb.AddRecord(key, *rights, uri, id));
            User::LeaveIfError(iRdb.InitializeKey(uri));
            }
        else
            {
            User::LeaveIfError(err);
            }
        }

    iIv.SetLength(KDCFKeySize);
    iRdb.GetRandomDataL(iIv);

    iEncryptionStream = &anOutput;

    anOutput.WriteUint8L(KVersion);
    anOutput.WriteUint8L(aMIMEType.Length());
    anOutput.WriteUint8L(uri.Length());
    anOutput.WriteL(aMIMEType);
    anOutput.WriteL(uri);
    anOutput.WriteUint8L(KLDFHeader().Size());
    anOutput.WriteUint8L(0);
    anOutput.WriteL(KLDFHeader);
    anOutput.WriteL(iIv);
    anOutput.CommitL();
    iBytesWritten += 3 + aMIMEType.Size() + 2 + KLDFHeader().Size() +
        iIv.Size();

    CleanupStack::PopAndDestroy(); // rights
    }

// -----------------------------------------------------------------------------
// COma1DcfCreator::EncryptUpdateL
// update the encryption
// -----------------------------------------------------------------------------
EXPORT_C void COma1DcfCreator::EncryptUpdateL(const TDesC8& aMessageData)
    {
    TInt i;
    TInt n;
    TInt size;
    TPtr8 ptr(iEncryptionBuffer->Des());
    TPtrC8 data;

    data.Set(aMessageData);
    if (iRemainder.Size() > 0 && iRemainder.Size() + data.Size() >= KDCFKeySize)
        {
        ptr.Copy(iRemainder);
        n = Min(KDCFKeySize - iRemainder.Size(), data.Size());
        ptr.Append(data.Left(n));
        iRdb.Encrypt(iIv, ptr);
        iEncryptionStream->WriteL(ptr);
        iBytesWritten += ptr.Size();
        iIv.Copy(ptr.Right(KDCFKeySize));
        data.Set(data.Right(data.Size() - n));
        iRemainder.SetLength(0);
        }

    size = data.Size();
    for (i = 0; size > KDCFKeySize; i += KMaxEncryptionSize)
        {
        n = Min(KMaxEncryptionSize, ((size / KDCFKeySize) * KDCFKeySize));
        ptr.Copy(data.Mid(i, n));
        iRdb.Encrypt(iIv, ptr);
        iEncryptionStream->WriteL(ptr);
        iBytesWritten += ptr.Size();
        iIv.Copy(ptr.Right(KDCFKeySize));
        size -= n;
        }
    iRemainder.Append(data.Right(size));
    }


// -----------------------------------------------------------------------------
// COma1DcfCreator::EncryptFinalizeL
// finalize the encryption
// -----------------------------------------------------------------------------
EXPORT_C void COma1DcfCreator::EncryptFinalizeL()
    {
    TPtr8 ptr(iEncryptionBuffer->Des());

    ptr.Copy(iRemainder);
    iRdb.Encrypt(iIv, ptr, ETrue);
    iEncryptionStream->WriteL(ptr);
    iEncryptionStream->CommitL();

    iBytesWritten += ptr.Size();
    iRdb.Close();
    }

// -----------------------------------------------------------------------------
// COma1DcfCreator::EncryptContent
// Encrypt plain content to a DCF. Replaces the input buffer with the plain
// content with the DCF.
// -----------------------------------------------------------------------------
EXPORT_C void COma1DcfCreator::EncryptContentL(
    HBufC8*& aContent,
    const TDesC8& aMIMEType,
    CDRMRights* aRightsObject)
    {
    RMemWriteStream output;
    TAny* buffer;
    TInt size;

    size = aContent->Length() +
        aMIMEType.Length() +
        KLDContentURI().Length() +
        KLDFHeader().Length() + 256;
    buffer = User::Alloc(size);
    User::LeaveIfNull(buffer);
    CleanupStack::PushL(buffer);
    output.Open(buffer, size);
    CleanupClosePushL(output);
    EncryptInitializeL(output, aMIMEType, aRightsObject);
    EncryptUpdateL(*aContent);
    EncryptFinalizeL();
    delete aContent;
    aContent = HBufC8::NewMax(size);
    User::LeaveIfNull(aContent);
    size = iBytesWritten;
    *aContent = TPtr8(static_cast<TUint8*>(buffer), size, size);
    CleanupStack::PopAndDestroy(2); // output, buffer
    }

// -----------------------------------------------------------------------------
// COma1DcfCreator::EncryptFileL
// Turn a plain content file into a DCF.
// -----------------------------------------------------------------------------
EXPORT_C void COma1DcfCreator::EncryptFileL(
    const TDesC& aFileName,
    const TDesC& aDCFFileName,
    const TDesC8& aMIMEType,
    CDRMRights* aRightsObject)
    {
    TInt r = KErrNone;
    RFs fs;
    RFile input;
    RFileWriteStream output;
    HBufC8* buf = HBufC8::NewLC(8000);
    TPtr8 buffer(const_cast<TUint8*>(buf->Ptr()),0,8000);


    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    r = input.Open(fs, aFileName, EFileRead|EFileShareReadersOrWriters);
    if(r == KErrInUse)
        {
        r = input.Open(fs, aFileName, EFileRead | EFileShareAny);

        if(r == KErrInUse)
            {
            User::LeaveIfError(input.Open(fs, aFileName, EFileRead |
                                                         EFileShareReadersOnly));
            r = KErrNone;
            }
        }

    User::LeaveIfError( r );

    CleanupClosePushL(input);

    User::LeaveIfError(output.Replace(fs, aDCFFileName, EFileWrite));
    CleanupClosePushL(output);


    EncryptInitializeL(output, aMIMEType, aRightsObject);


    do
        {
        input.Read(buffer, buffer.MaxSize());
        if (buffer.Size() > 0)
            {

            EncryptUpdateL(buffer);

            }
        }
    while (buffer.Size() > 0);

    EncryptFinalizeL();

    CleanupStack::PopAndDestroy(4); // output, input, fs, buf
    }

// -----------------------------------------------------------------------------
// COma1DcfCreator::EncryptContentToFile
// Encrypt plain content to a DCF and stores it in a file.
// -----------------------------------------------------------------------------
EXPORT_C void COma1DcfCreator::EncryptContentToFileL(
    const TDesC8& aContent,
    const TDesC& aFileName,
    const TDesC8& aMIMEType,
    CDRMRights* aRightsObject)
    {
    RFs fs;
    RFileWriteStream output;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fs.Delete(aFileName);
    User::LeaveIfError(output.Create(fs, aFileName, EFileWrite));
    CleanupClosePushL(output);
    EncryptInitializeL(output, aMIMEType, aRightsObject);
    EncryptUpdateL(aContent);
    EncryptFinalizeL();
    CleanupStack::PopAndDestroy(2); // output, fs
    }

// End of File
