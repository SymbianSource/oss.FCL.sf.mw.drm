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


#undef _ROAP_TESTING

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

// INCLUDE FILES
#include "RoapResponse.h"
#include "RoapLog.h"

using namespace Roap;

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

#ifndef RD_MULTIPLE_DRIVE
// default filename
_LIT( KMultipartPduTempPath, "c:\\");
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRoapResponse::CRoapResponse()
// -----------------------------------------------------------------------------
//
CRoapResponse::CRoapResponse()
    {
    }

// -----------------------------------------------------------------------------
// CRoapResponse::ConstructL()
// -----------------------------------------------------------------------------
//
void CRoapResponse::ConstructL()
    {
    iPdu = HBufC8::NewL( 0 );
    iMultipartState = EOther;
    iMaxSize = -1;
    }

// -----------------------------------------------------------------------------
// CRoapResponse::NewL()
// -----------------------------------------------------------------------------
//
CRoapResponse* CRoapResponse::NewL()
    {
    CRoapResponse* self = new( ELeave ) CRoapResponse;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CRoapResponse::~CRoapResponse()
// -----------------------------------------------------------------------------
//
CRoapResponse::~CRoapResponse()
    {
    delete iPdu;
    delete iContentName;
    }

// -----------------------------------------------------------------------------
// CRoapResponse::ProtocolUnit()
// -----------------------------------------------------------------------------
//
const TDesC8& CRoapResponse::ProtocolUnit() const
    {
    return *iPdu;
    }

// -----------------------------------------------------------------------------
// CRoapResponse::PduFileName()
// -----------------------------------------------------------------------------
//
const TFileName& CRoapResponse::DcfFileName() const
    {
    return iDcfFileName;
    }

// -----------------------------------------------------------------------------
// CRoapResponse::SetPduFileName()
// -----------------------------------------------------------------------------
//
void CRoapResponse::SetDcfPathL(
    const TPath& aPath )
    {
    LOG( _L("CRoapResponse::SetDcfPathL->") );

    RFs fs;
    RFile file;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    if ( aPath.Length() > 3 )
        {
        User::LeaveIfError( file.Temp( fs, aPath, iDcfFileName,
                            EFileShareExclusive | EFileWrite ) );
        }
    else
        {

#ifndef RD_MULTIPLE_DRIVE

        User::LeaveIfError( file.Temp( fs, KMultipartPduTempPath(), iDcfFileName,
                            EFileShareExclusive | EFileWrite ) );

#else //RD_MULTIPLE_DRIVE

        _LIT( KDriveRoot, "%c:\\");
        TInt driveNumber( -1 );
        TChar driveLetter;
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
        fs.DriveToChar( driveNumber, driveLetter );

        TFileName multipartTemp;
        multipartTemp.Format( KDriveRoot, (TUint)driveLetter );

        User::LeaveIfError( file.Temp( fs, multipartTemp, iDcfFileName,
                            EFileShareExclusive | EFileWrite ) );

#endif
        }
    LOG2( _L("iDcfFileName: %S"), &iDcfFileName );

    file.Close();
    CleanupStack::PopAndDestroy();
    LOG( _L("CRoapResponse::SetDcfFileName <-") );
    }

// -----------------------------------------------------------------------------
// CRoapResponse::AppendPduFileDataL()
// -----------------------------------------------------------------------------
//
void CRoapResponse::AppendMultipartDataL(
    const TDesC8& aData )
    {
    WriteL( aData );
    }

// ---------------------------------------------------------
// CRoapResponse::DataType()
// ---------------------------------------------------------
//
const TDataType& CRoapResponse::DataType() const
    {
    return iDataType;
    }

// -----------------------------------------------------------------------------
// CRoapResponse::HandleBodyDataL
//
// -----------------------------------------------------------------------------
//
void CRoapResponse::HandleBodyDataL(
    const TDesC8& aData)
    {
    LOG( _L("CRoapResponse::HandleBodyDataL ->") );
    if ( iMultipartState == ERights )
        {
        LOG( _L("  saving RO data") );
        iPdu = iPdu->ReAlloc( iPdu->Size() + aData.Size() );
        User::LeaveIfNull( iPdu );
        iPdu->Des().Append( aData );
        }
    else if ( iMultipartState == EDcf )
        {
        RFs fs;
        RFile file;

        LOG( _L("  saving DCF data") );
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );
        User::LeaveIfError( file.Open( fs, iDcfFileName,
            EFileShareExclusive | EFileWrite ) );
        CleanupClosePushL( file );
        User::LeaveIfError( file.Write( KMaxTInt32, aData ) );
        CleanupStack::PopAndDestroy( 2 ); // file, fs
        }
    LOG( _L("CRoapResponse::HandleBodyDataL <-") );
    }

// -----------------------------------------------------------------------------
// CRoapResponse::StartBodyPartL
//
// -----------------------------------------------------------------------------
//
void CRoapResponse::StartBodyPartL()
    {
    LOG( _L("CRoapResponse::StartBodyPartL ->") );
    if ( iMultipartState == EOther )
        {
        LOG( _L("  starting RO part") );
        iMultipartState = ERights;
        }
    else if ( iMultipartState == ERights )
        {
        LOG( _L("  starting DCF part") );
        iMultipartState = EDcf;
        }
    else
        {
        LOG( _L("  some other part") );
        }
    LOG( _L("CRoapResponse::StartBodyPartL <-") );
    }

// -----------------------------------------------------------------------------
// CRoapResponse::EndBodyPartL
//
// -----------------------------------------------------------------------------
//
void CRoapResponse::EndBodyPartL()
    {
    LOG( _L("CRoapResponse::EndBodyPartL ->") );
    LOG( _L("CRoapResponse::EndBodyPartL <-") );
    }

// -----------------------------------------------------------------------------
// CRoapResponse::SetContentNameL
//
// -----------------------------------------------------------------------------
//
void CRoapResponse::SetContentNameL( const TDes& aName )
    {
    LOG( _L("CRoapResponse::SetContentNameL") );

    delete iContentName;
    iContentName = NULL;

    iContentName = aName.AllocL();
    }


// -----------------------------------------------------------------------------
// CRoapResponse::GetContentNameLC
//
// -----------------------------------------------------------------------------
//
void CRoapResponse::GetContentNameLC( HBufC*& aName ) const
    {
    LOG( _L("CRoapResponse::GetContentName") );

    if ( iContentName )
        aName = iContentName->AllocLC();
    else
        aName = NULL;
    }
// -----------------------------------------------------------------------------
// CRoapResponse::SetMaxSize
//
// -----------------------------------------------------------------------------
//
void CRoapResponse::SetMaxSize( const TInt& aSize )
    {
    LOG( _L("CRoapResponse::SetMaxSize") );

    iMaxSize = aSize;
    }


// -----------------------------------------------------------------------------
// CRoapResponse::MaxSize
//
// -----------------------------------------------------------------------------
//
TInt CRoapResponse::MaxSize() const
    {
    LOG( _L("CRoapResponse::MaxSize") );

    return iMaxSize;
    }
