/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CAF Agent Data class
*
*/



// INCLUDE FILES
#include <caf.h>
#include <caf/caferr.h>
#include <attribute.h>
#include <e32test.h>
#include "wmdrmagentdata.h"
#include "wmdrmagentattributes.h"
#include "asf.h"
#include "logfn.h"

using namespace ContentAccess;

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt MapContentShareMode(TContentShareMode aMode);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MapContentShareMode
// Maps the CAF specific file share mode to the RFs/RFile sharing mode
// -----------------------------------------------------------------------------
//
TInt MapContentShareMode(TContentShareMode aMode)
    {
    TInt r = EFileRead | EFileShareAny;

    switch (aMode)
        {
        case EContentShareReadOnly:
            r = EFileRead | EFileShareReadersOnly;
            break;
        case EContentShareReadWrite:
            r = EFileRead | EFileShareReadersOrWriters;
            break;
        case EContentShareExclusive:
            r = EFileRead | EFileShareExclusive;
            break;
        }
    return r;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWmDrmAgentData::CWmDrmAgentData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWmDrmAgentData::CWmDrmAgentData():
    iVirtualPath( KNullDesC ),
    iAsf( NULL )
    {
    LOGFN( "CWmDrmAgentData::CWmDrmAgentData" );
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWmDrmAgentData* CWmDrmAgentData::NewL(
    const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode )
    {
    LOGFN( "CWmDrmAgentData::NewL" );
    CWmDrmAgentData* self = new (ELeave) CWmDrmAgentData;
    CleanupStack::PushL(self);
    self->ConstructL(aVirtualPath, aShareMode);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::NewLC
// -----------------------------------------------------------------------------
//
CWmDrmAgentData* CWmDrmAgentData::NewLC(
    const TVirtualPathPtr& aVirtualPath,
    TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentData::NewLC" );
    CWmDrmAgentData* self=new(ELeave) CWmDrmAgentData();
    CleanupStack::PushL(self);
    self->ConstructL(aVirtualPath, aShareMode);
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::NewLC
// -----------------------------------------------------------------------------
//
CWmDrmAgentData* CWmDrmAgentData::NewL(
    RFile& aFile, const TDesC& aUniqueId )
    {
    LOGFN( "CWmDrmAgentData::NewL (2)" );
    CWmDrmAgentData* self = new (ELeave) CWmDrmAgentData;
    CleanupStack::PushL(self);
    self->ConstructL(aFile, aUniqueId);
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentData::NewLC
// -----------------------------------------------------------------------------
//
CWmDrmAgentData* CWmDrmAgentData::NewLC(
    RFile& aFile,
    const TDesC& aUniqueId )
    {
    LOGFN( "CWmDrmAgentData::NewLC (2)" );
    CWmDrmAgentData* self=new(ELeave) CWmDrmAgentData();
    CleanupStack::PushL(self);
    self->ConstructL(aFile, aUniqueId);
    return self;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::~CWmDrmAgentData
// -----------------------------------------------------------------------------
//

CWmDrmAgentData::~CWmDrmAgentData()
    {
    // Tidy up RFile and RFs
    LOGFN( "CWmDrmAgentData::~CWmDrmAgentData" );
    iFile.Close();
    iFs.Close();
    delete iAsf;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::ConstructL
// The porting kit is not initialized here since it is time consuming. It is
// initialized whenever needed and just allocated at this point.
// -----------------------------------------------------------------------------
//
void CWmDrmAgentData::ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
    {
    LOGFN( "CWmDrmAgentData::ConstructL" );
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFile.Open(iFs, aVirtualPath.URI(),
        MapContentShareMode(aShareMode)));
    iAsf = CAsf::NewL(iFile);
    iVirtualPath = aVirtualPath;
    }


// -----------------------------------------------------------------------------
// CWmDrmAgentData::ConstructL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentData::ConstructL(RFile& aFile, const TDesC& /*aUniqueId*/)
    {
    TInt pos = 0;

    LOGFN( "CWmDrmAgentData::ConstructL (2)" );
    // When creating a CData from a file handle we must duplicate the file handle
    // before doing anything
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFile.Duplicate(aFile));
    User::LeaveIfError(iFile.Seek(ESeekStart, pos));  // reset to start of file
    iAsf = CAsf::NewL(iFile);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::DataSizeL
// -----------------------------------------------------------------------------
//
void CWmDrmAgentData::DataSizeL(TInt &aSize)
    {
    LOGFN( "CWmDrmAgentData::DataSizeL" );
    User::LeaveIfError(iFile.Size(aSize));
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::EvaluateIntent
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::EvaluateIntent(TIntent /*aIntent*/)
    {
    LOGFN( "CWmDrmAgentData::EvaluateIntent" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::ExecuteIntent
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::ExecuteIntent(TIntent /*aIntent*/)
    {
    LOGFN( "CWmDrmAgentData::ExecuteIntent" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::Read
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::Read(TDes8& aDes)
    {
    LOGFN( "CWmDrmAgentData::Read" );
    return iFile.Read(aDes);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::Read
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::Read(TDes8& aDes,TInt aLength)
    {
    LOGFN( "CWmDrmAgentData::Read (2)" );
    return iFile.Read(aDes,aLength);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::Read
// -----------------------------------------------------------------------------
//
void CWmDrmAgentData::Read(TDes8& aDes,TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmAgentData::Read (3)" );
    iFile.Read(aDes, aStatus);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::Read
// -----------------------------------------------------------------------------
//
void CWmDrmAgentData::Read(TDes8& aDes,
                             TInt aLength,
                             TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmAgentData::Read (4)" );
    iFile.Read(aDes, aLength, aStatus);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::Read
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::Read(TInt aPos, TDes8& aDes,
                             TInt aLength,
                             TRequestStatus& aStatus)
    {
    LOGFN( "CWmDrmAgentData::Read (5)" );
    iFile.Read(aPos, aDes, aLength, aStatus);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::Seek
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::Seek(TSeek aMode, TInt& aPos)
    {
    LOGFN( "CWmDrmAgentData::Seek" );
    return iFile.Seek(aMode, aPos);
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::SetProperty
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
    {
    LOGFN( "CWmDrmAgentData::SetProperty" );
    return KErrCANotSupported;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::GetAttribute
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::GetAttribute(TInt aAttribute, TInt& aValue)
    {
    TInt r = KErrNone;
    TInt err = KErrNone;

    LOGFN( "CWmDrmAgentData::GetAttribute" );
    TRAP(err,r = TWmDrmAgentAttributes::GetAttributeL(iAsf,
        aAttribute, aValue, iVirtualPath));
    if (err != KErrNone)
        {
        return err;
        }

    return r;

    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::GetAttributeSet
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::GetAttributeSet(RAttributeSet& aAttributeSet)
    {
    TInt err = KErrNone;
    TInt r = KErrNone;

    LOGFN( "CWmDrmAgentData::GetAttributeS" );
    TRAP(err, r = TWmDrmAgentAttributes::GetAttributeSetL(iAsf,
        aAttributeSet, iVirtualPath) );
    if (err != KErrNone)
        {
        return err;
        }
    return r;
    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::GetStringAttribute
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::GetStringAttribute(TInt aAttribute, TDes& aValue)
    {
    TInt err = KErrNone;
    TInt r = KErrNone;

    LOGFN( "CWmDrmAgentData::GetStringAttribute" );
    TRAP(err, r = TWmDrmAgentAttributes::GetStringAttributeL(iAsf,
        aAttribute, aValue, iVirtualPath));
    if (err != KErrNone)
        {
        return err;
        }
    return r;

    }

// -----------------------------------------------------------------------------
// CWmDrmAgentData::GetStringAttributeSet
// -----------------------------------------------------------------------------
//
TInt CWmDrmAgentData::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet)
    {
    TInt err = KErrNone;
    TInt r = KErrNone;

    LOGFN( "CWmDrmAgentData::GetStringAttributeSet" );
    TRAP(err,r = TWmDrmAgentAttributes::GetStringAttributeSetL(iAsf,
        aStringAttributeSet, iVirtualPath));
    if (err != KErrNone)
        {
        return err;
        }
    return r;

    }

//  End of File
