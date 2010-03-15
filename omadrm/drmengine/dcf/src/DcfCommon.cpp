/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include "DcfCommon.h"
#include "Oma1Dcf.h"
#include "oma2dcf.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDcfCommon::CDcfCommon
//
// -----------------------------------------------------------------------------
//
CDcfCommon::CDcfCommon():
    iData(NULL),
    iMimeType(NULL),
    iContentID(NULL),
    iRightsIssuerURL(NULL),
    iPadding(-1),
    iEncryptionMethod(EMethodAES_128_CBC),
    iTitle(NULL),
    iDescription(NULL),
    iIconUri(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDcfCommon::ConstructL
//
// -----------------------------------------------------------------------------
//
void CDcfCommon::ConstructL(
    const RFile& aFile)
    {
    iFile.Duplicate(aFile);
    iFile.Size(iLength);
    }

// -----------------------------------------------------------------------------
// CDcfCommon::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CDcfCommon* CDcfCommon::NewL(
    const RFile& aFile)
    {
    TBuf8<256> buffer;
    TInt pos = 0;
    CDcfCommon* self = NULL;

    User::LeaveIfError(aFile.Seek(ESeekStart, pos));
    User::LeaveIfError(aFile.Read(buffer));
    if (COma1Dcf::IsValidDcf(buffer))
        {
        self = COma1Dcf::NewL(aFile);
        }
    else if (COma2Dcf::IsValidDcf(buffer))
        {
        self = COma2Dcf::NewL(aFile);
        }

    return self;
    }

// -----------------------------------------------------------------------------
// CDcfCommon::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CDcfCommon* CDcfCommon::NewL(
    const TDesC& aFileName,
    RFs* aFs)
    {
    RFs* fs = NULL;
    RFile file;
    TInt r = KErrNone;
    CDcfCommon* self = NULL;

    if (aFs == NULL)
        {
        fs = new RFs();
        User::LeaveIfNull(fs);
        CleanupStack::PushL(fs);
        User::LeaveIfError(fs->Connect());
        }
    else
        {
        fs = aFs;
        }

    r = file.Open(*fs, aFileName, EFileStream | EFileRead | EFileShareReadersOrWriters );
    if( r == KErrInUse )
        {
        r = file.Open(*fs, aFileName, EFileStream | EFileRead | EFileShareAny);

        if (r == KErrInUse)
            {
            r = file.Open(*fs, aFileName, EFileStream | EFileRead |
                EFileShareReadersOnly);
            }
        }
    if(r == KErrNone)
        {
        CleanupClosePushL(file);
        self = NewL(file);
        CleanupStack::PopAndDestroy();
        }

    if (aFs == NULL)
        {
        fs->Close();
        CleanupStack::PopAndDestroy();
        }
    return self;
    }

// -----------------------------------------------------------------------------
// CDcfCommon::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CDcfCommon::~CDcfCommon()
    {
    delete iData;
    delete iMimeType;
    delete iContentID;
    delete iRightsIssuerURL;
    delete iTitle;
    delete iDescription;
    delete iIconUri;
    if (iFile.SubSessionHandle() != KNullHandle)
        {
        iFile.Close();
        }
    }

// -----------------------------------------------------------------------------
// CDcfCommon::
//
// -----------------------------------------------------------------------------
//
TInt CDcfCommon::CheckUniqueId(const TDesC& aUniqueId)
    {
    if (aUniqueId.Compare(KDefaultContentObject) == 0)
        {
        return 0;
        }
    else
        {
        return KErrNotFound;
        }
    }

//  End of File
