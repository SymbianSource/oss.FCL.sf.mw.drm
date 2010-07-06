/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Rights Object MTM UI Data
*
*/


// Standard includes
#if !defined(__COEMAIN_H__)
#include <coemain.h>	// CCoeEnv
#endif

// Messaging includes
#if !defined(__MTCLBASE_H__)
#include <mtclbase.h> 
#endif
#if !defined(__MSVSTD_HRH__)
#include <msvstd.hrh>
#endif
#if !defined(__MTMDEF_HRH__)
#include <mtmdef.hrh>
#endif
#if !defined(__MSVUIDS_H__)
#include <msvuids.h>
#endif
#if !defined(__MSVIDS_H__)
#include <msvids.h>
#endif

#include <msvapi.h>
#include <RoMtmUi.rsg>
#include <mtmuids.h>
#include <mtmuidef.hrh>
#include <stringresourcereader.h>
#include <avkon.rsg>
#include <data_caging_path_literals.hrh>

#include <StringLoader.h>
#include <AknsUtils.h>
#include <AknsConstants.h>//skinned icon ids

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "RoMtmDat.h"
#include "RoMTMDaC.h"
#include "RoMTMCommon.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// Constants
#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );                                      
#endif

_LIT( KRoMtmUdResourceFile, "RoMtmUi.RSC" );



// ============================= LOCAL FUNCTIONS ===============================


EXPORT_C CBaseMtmUiData* NewMtmUiDataLayerL( CRegisteredMtmDll& aRegisteredDll )
// Factory function
	{
	return CRightsObjectMtmUiData::NewL( aRegisteredDll );
	}




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiDataSessionObserver::CRightsObjectMtmUiDataSessionObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmUiDataSessionObserver::CRightsObjectMtmUiDataSessionObserver()
	{
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiDataSessionObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmUiDataSessionObserver* 
    CRightsObjectMtmUiDataSessionObserver::NewL()
	{
	CRightsObjectMtmUiDataSessionObserver* self = 
        new ( ELeave ) CRightsObjectMtmUiDataSessionObserver();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// Destructor
CRightsObjectMtmUiDataSessionObserver::~CRightsObjectMtmUiDataSessionObserver()
	{ 
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiDataSessionObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUiDataSessionObserver::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiDataSessionObserver::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUiDataSessionObserver::HandleSessionEventL(
    TMsvSessionEvent aEvent, 
    TAny* aArg1, 
    TAny* /*aArg2*/, 
    TAny* /*aArg3*/ )
    {
    
#ifdef RD_MULTIPLE_DRIVE
    _LIT(KRoHandlerTriggerFilePath, "%c:\\system\\data\\"); 
#else
    _LIT(KRoHandlerTriggerFilePath, "c:\\system\\data\\");
#endif
   
    if (aEvent == EMsvEntriesDeleted)
        {
        TFileName fileName;
        CMsvEntrySelection* selection = STATIC_CAST( CMsvEntrySelection*, aArg1 );
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        for (TInt i = 0;i<selection->Count();i++)
            {
            
#ifndef RD_MULTIPLE_DRIVE
    
            _LIT(KFile,"%S%u.tri");
            fileName.Format(KFile,&KRoHandlerTriggerFilePath,selection->At(i));
            fs.Delete(fileName);
    
#else //RD_MULTIPLE_DRIVE
    
            TInt driveNumber( -1 );
            TChar driveLetter;
            DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
	        fs.DriveToChar( driveNumber, driveLetter );
	
	        TFileName roHandlerTriggerFilePath;
	        roHandlerTriggerFilePath.Format( 
	                    KRoHandlerTriggerFilePath, (TUint)driveLetter );
    
            _LIT(KFile,"%S%u.tri");
            fileName.Format( 
                        KFile, &roHandlerTriggerFilePath, selection->At(i) );
            fs.Delete(fileName);
    
#endif
            
            
            }
        CleanupStack::PopAndDestroy(&fs);
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CRightsObjectMtmUiData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmUiData::CRightsObjectMtmUiData( CRegisteredMtmDll& aRegisteredDll )
	:	CBaseMtmUiData( aRegisteredDll )
	{
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmUiData* 
    CRightsObjectMtmUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
	{
	CRightsObjectMtmUiData* base = 
        new ( ELeave ) CRightsObjectMtmUiData( aRegisteredDll );
	CleanupStack::PushL( base );
	base->ConstructL();
	CleanupStack::Pop();
	return base;
	}

// Destructor
CRightsObjectMtmUiData::~CRightsObjectMtmUiData()
	{ 
    delete iSession;
    delete iObserver;
	}


void CRightsObjectMtmUiData::ConstructL()
	{ 
    CBaseMtmUiData::ConstructL();
    iObserver = CRightsObjectMtmUiDataSessionObserver::NewL();
    iSession = CMsvSession::OpenSyncL( *iObserver );
    }

/*************************************
*
*		MTM-specific functionality 
*
**************************************/

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::GetResourceFileName
// 
// Gets the resource file name. 
//
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUiData::GetResourceFileName( TFileName& aFileName ) const
// Resource file loading
	{ 
#ifndef RD_MULTIPLE_DRIVE
    
    aFileName = KDriveZ;
    
#else //RD_MULTIPLE_DRIVE
    
    _LIT( KDrive, "%c:");
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
	
	iCoeEnv->FsSession().DriveToChar( driveNumber, driveLetter );
	
    aFileName.Format( KDrive, (TUint)driveLetter );
    
#endif
	
	aFileName.Append( KDC_MTM_RESOURCE_DIR );
   	aFileName.Append( KRoMtmUdResourceFile );
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::PopulateArraysL
// 
// Initialises bitmaps and function information. 
//
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUiData::PopulateArraysL()
// Initialise bitmaps and function information 
	{
    CreateSkinnedBitmapsL( KBitmapArraySize );
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::PopulateArraysL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TInt CRightsObjectMtmUiData::OperationSupportedL( 
										TInt /*aOperationId*/ , 
										const TMsvEntry& /*aContext*/ 
										) const
// Context-sensitive operation query 
	{
	return R_ROUD_NOT_SUPPORTED;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::QueryCapability
// 
// Queries if the MTM supports a particular capability, specified by a UID. 
//
// -----------------------------------------------------------------------------
//
TInt CRightsObjectMtmUiData::QueryCapability( TUid aCapability , TInt& aResponse ) const
// Query for capability 
	{
	switch ( aCapability.iUid )
		{
		case KUidMtmQueryMaxBodySizeValue:
		case KUidMtmQueryMaxTotalMsgSizeValue:
			{
			aResponse = KMaxRoMessageSize;
			break;
			}
		case KUidMtmQuerySupportedBodyValue:
			{
			aResponse = KMtm7BitBody | KMtm8BitBody | 
						   KMtm16BitBody | KMtmBinaryBody;
			break;
			}
		// Supported non-valued capabilities
		// boolean returns
		case KUidMtmQuerySupportSubjectValue:
		case KUidMtmQueryOffLineAllowedValue:
			{
			aResponse=ETrue;
			break;		
			}
		default:
			return KErrNotSupported;
		};
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::ContextIcon
// 
// Gets an array of bitmaps relevant to the passed context entry. 
//
// -----------------------------------------------------------------------------
//
const CBaseMtmUiData::CBitmapArray& CRightsObjectMtmUiData::ContextIcon(
											const TMsvEntry& aContext ,
											TInt /*aStateFlags*/
											) const
// Get context-specific icon
	{
    return *iIconArrays->At( aContext.Unread() ? 0 : 1 );
	}

//
// Context-specific information
//

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanOpenEntryL
// 
// Tests if an entry can be opened.
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanOpenEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const
	{
	aReasonResourceId=0;
	if ( aContext.iType != KUidMsvMessageEntry )
		{
		aReasonResourceId = R_ROUD_ONLY_MESSAGES;
		return KErrNotSupported;
		}
	else
		{
		return KErrNone;		
		}
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanCloseEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanCloseEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
	{
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanViewEntryL
// 
// Tests if an entry can be viewed.
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanViewEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const
	{
	aReasonResourceId=0;
	if ( aContext.iType != KUidMsvMessageEntry )
		{
		aReasonResourceId = R_ROUD_ONLY_MESSAGES;
		return KErrNotSupported;
		}
	else
		{
		return KErrNone;		
		}
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanEditEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanEditEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
	{
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanDeleteFromEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanDeleteFromEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
	{
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanCopyMoveToEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanCopyMoveToEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
	{
	aReasonResourceId = KErrNone;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanCopyMoveFromEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanCopyMoveFromEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
	{
	aReasonResourceId = KErrNone;
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanReplyToEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanReplyToEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
//
// MTM UI does not support replying.
//
    {
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanForwardEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanForwardEntryL( const TMsvEntry& /*aContext*/ , TInt& aReasonResourceId ) const
//
// MTM UI does not supports forwarding.
//
    {
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanCreateEntryL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanCreateEntryL( const TMsvEntry& /*aParent*/ , TMsvEntry& /*aNewEntry*/ , 
									  TInt& aReasonResourceId ) const
	{
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanDeleteServiceL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanDeleteServiceL( 
    const TMsvEntry& /*aService*/, 
    TInt& aReasonResourceId ) const
	{
	aReasonResourceId = R_ROUD_ONLY_MESSAGES;
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::CanCancelL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
TBool CRightsObjectMtmUiData::CanCancelL( 
    const TMsvEntry& /*aContext*/ , 
    TInt& aReasonResourceId ) const
	{
	aReasonResourceId = R_ROUD_NOT_SUPPORTED;
	return KErrNotSupported;	
	}

// -----------------------------------------------------------------------------
// CRightsObjectMtmUiData::StatusTextL
// 
// Not supported
//
// -----------------------------------------------------------------------------
//
HBufC* CRightsObjectMtmUiData::StatusTextL( const TMsvEntry& /*aContext*/ ) const
	{
    return NULL;
	}


// ---------------------------------------------------------
// CRightsObjectMtmUiData::CreateSkinnedBitmapsL
//
// ---------------------------------------------------------
void CRightsObjectMtmUiData::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    //CFbsBitmap* bitmap;
    //CFbsBitmap* bitmapMask;
    CBitmapArray* array = NULL;
    TParse tp;
    tp.Set( KRoMtmUiDataBitmapFile, &KDC_APP_BITMAP_DIR, NULL );

    array = new(ELeave) CArrayPtrFlat<CFbsBitmap>( aNumZoomStates );
    CleanupStack::PushL( array );
    
    id.Set( KAknsIIDQgnPropMcePushUnread );

    iIconArrays->AppendL( array );
    CleanupStack::Pop(); //array
    
    array = new(ELeave) CArrayPtrFlat<CFbsBitmap>( aNumZoomStates );
    CleanupStack::PushL( array );
    
    id.Set( KAknsIIDQgnPropMcePushRead );

    iIconArrays->AppendL( array );
    CleanupStack::Pop(); //array
    }
