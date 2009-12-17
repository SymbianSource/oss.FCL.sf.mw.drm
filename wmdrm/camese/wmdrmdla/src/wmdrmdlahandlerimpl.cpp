/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for WMDRM DLA Handler
*
*/


#include <ecom/ecom.h>
#include "wmdrmdlahandlerimpl.h"
#include "wmdrmdlahttplicacqpluginfw.h"
#include "wmdrmdlahttpmeteringpluginfw.h"
#include "wmdrmdlarequest.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::ConstructL()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::ConstructL" );
    iLicenseAcquisitionFw = CWmDrmDlaHttpLicAcqPluginFw::NewL();
    iMeteringFw = CWmDrmDlaHttpMeteringPluginFw::NewL();
    iWait = new (ELeave) CActiveSchedulerWait;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CWmDrmDlaHandlerImpl
// ---------------------------------------------------------------------------
//
CWmDrmDlaHandlerImpl::CWmDrmDlaHandlerImpl() 
    : CActive( EPriorityStandard ), iState( CWmDrmDlaHandlerImpl::EIdle )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::CWmDrmDlaHandlerImpl" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::NewL
// ---------------------------------------------------------------------------
//
CWmDrmDlaHandlerImpl* CWmDrmDlaHandlerImpl::NewL()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::NewL" );
    CWmDrmDlaHandlerImpl* self = CWmDrmDlaHandlerImpl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::NewLC
// ---------------------------------------------------------------------------
//
CWmDrmDlaHandlerImpl* CWmDrmDlaHandlerImpl::NewLC()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::NewLC" );
    CWmDrmDlaHandlerImpl* self = new( ELeave ) CWmDrmDlaHandlerImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::~CWmDrmDlaHandlerImpl
// ---------------------------------------------------------------------------
//
CWmDrmDlaHandlerImpl::~CWmDrmDlaHandlerImpl()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::~CWmDrmDlaHandlerImpl" );
    CancelLicenseAcquisition();
    CancelMetering();
    iRequests.ResetAndDestroy();
    iRequests.Close();
    delete iCurrentRequest;
    delete iSyncRequest;
    delete iLicenseAcquisitionFw;
    delete iMeteringFw;
    delete iWait;
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::SilentL
// ---------------------------------------------------------------------------
//
TBool CWmDrmDlaHandlerImpl::SilentL( 
    const RFile& aFile )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::SilentL" );
    TBool silent( iLicenseAcquisitionFw->SilentL( aFile ) );
    LOG2( "silent: %d", silent );
    return silent;
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::AcquireLicenseL
// ---------------------------------------------------------------------------
///
void CWmDrmDlaHandlerImpl::AcquireLicenseL( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::AcquireLicenseL" );
    if ( iWait->IsStarted() )
        {
        User::Leave( KErrInUse );
        }
    
    CWmDrmDlaRequest* request( CWmDrmDlaRequest::NewL( aFile, 
                                                       aContentUrl, 
                                                       aHtmlData, 
                                                       ETrue, 
                                                       iStatus ) );
    if ( iState == CWmDrmDlaHandlerImpl::EIdle  )
        {
        iCurrentRequest = request;
        iState = CWmDrmDlaHandlerImpl::EAcquiringLicense;
        CompleteSelf();
        }
    else
        {
        iSyncRequest = request;
        }
    iWait->Start();
    LOG2( "iSyncReturnValue: %d", iSyncReturnValue );
    User::LeaveIfError( iSyncReturnValue );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::AcquireLicense
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::AcquireLicense( 
    const RFile& aFile, 
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHandlerImpl::AcquireLicense", err );
    
    CWmDrmDlaRequest* request( NULL );
    TRequestStatus *status = &aStatus;
    aStatus = KRequestPending;
    
    TRAP( err, request = CWmDrmDlaRequest::NewL( aFile, 
                                                 aContentUrl, 
                                                 aHtmlData, 
                                                 EFalse, 
                                                 aStatus ) );
    if ( err )
        {
        User::RequestComplete( status, err );
        }
    else if ( iState == CWmDrmDlaHandlerImpl::EIdle  )
        {
        iCurrentRequest = request;
        iState = CWmDrmDlaHandlerImpl::EAcquiringLicense;
        CompleteSelf();
        }
    else
        {
        err = iRequests.Append( request );
        if ( err )
            {
            delete request;
            User::RequestComplete( status, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::AcquireLicenseFromDrmHeaderL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::AcquireLicenseFromDrmHeaderL( 
    const TDesC8& aDrmHeader,
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::AcquireLicenseFromDrmHeaderL" );
    if ( iWait->IsStarted() )
        {
        User::Leave( KErrInUse );
        }
    
    CWmDrmDlaRequest* request( CWmDrmDlaRequest::NewL( aDrmHeader,
                                                       aErrorUrl,
                                                       aContentUrl, 
                                                       aHtmlData, 
                                                       ETrue, 
                                                       iStatus ) );
    if ( iState == CWmDrmDlaHandlerImpl::EIdle  )
        {
        iCurrentRequest = request;
        iState = CWmDrmDlaHandlerImpl::EAcquiringLicenseFromDrmHeader;
        CompleteSelf();
        }
    else
        {
        iSyncRequest = request;
        }
    iWait->Start();
    LOG2( "iSyncReturnValue: %d", iSyncReturnValue );
    User::LeaveIfError( iSyncReturnValue );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::AcquireLicenseFromDrmHeader
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::AcquireLicenseFromDrmHeader( 
    const TDesC8& aDrmHeader, 
    HBufC* aErrorUrl,
    HBufC*& aContentUrl, 
    HBufC*& aHtmlData, 
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHandlerImpl::AcquireLicenseFromDrmHeader", err );
    
    CWmDrmDlaRequest* request( NULL );
    TRequestStatus *status = &aStatus;
    aStatus = KRequestPending;
    
    TRAP( err, request = CWmDrmDlaRequest::NewL( aDrmHeader,
                                                 aErrorUrl,
                                                 aContentUrl, 
                                                 aHtmlData, 
                                                 EFalse, 
                                                 aStatus ) );
    if ( err )
        {
        User::RequestComplete( status, err );
        }
    else if ( iState == CWmDrmDlaHandlerImpl::EIdle  )
        {
        iCurrentRequest = request;
        iState = CWmDrmDlaHandlerImpl::EAcquiringLicenseFromDrmHeader;
        CompleteSelf();
        }
    else
        {
        err = iRequests.Append( request );
        if ( err )
            {
            delete request;
            User::RequestComplete( status, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CancelLicenseAcquisition
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::CancelLicenseAcquisition()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::CancelLicenseAcquisition" );
    
    //First check, if we are currently handling license acquisition request.
    //If we are, then cancel handling and cancel request 
    if ( iState == CWmDrmDlaHandlerImpl::ECompleted && 
         iCurrentRequest->iType 
             != CWmDrmDlaRequest::EProcessMeteringCertificate )
        {
        iLicenseAcquisitionFw->CancelLicenseAcquisition();
        Cancel();
        CompleteClientRequest( KErrCancel );
        CompleteSyncRequest( KErrCancel );
        }
    else if ( iState == CWmDrmDlaHandlerImpl::EAcquiringLicense || 
              iState == CWmDrmDlaHandlerImpl::EAcquiringLicenseFromDrmHeader )
        {
        Cancel();
        CompleteClientRequest( KErrCancel );
        CompleteSyncRequest( KErrCancel );
        }
    // Next go through the request queue,
    // remove all license acquisition requests and cancel them.
    TInt removed( 0 );
    for ( TInt i( 0 ); i - removed < iRequests.Count(); ++i )
        {
        if ( iRequests[ i - removed ]->iType 
                != CWmDrmDlaRequest::EProcessMeteringCertificate )
            {
            iCurrentRequest = iRequests[ i - removed ];
            iRequests.Remove( i - removed );
            ++removed;
            CompleteClientRequest( KErrCancel );
            }
        }
    if ( !iCurrentRequest )
        {
        NextRequest();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::ProcessMeteringCertificateL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::ProcessMeteringCertificateL( 
    const TDesC8& aCertificate )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::ProcessMeteringCertificateL" );
    if ( iWait->IsStarted() )
        {
        User::Leave( KErrInUse );
        }
    
    CWmDrmDlaRequest* request( CWmDrmDlaRequest::NewL( aCertificate, 
                                                       ETrue, 
                                                       iStatus ) );
    if ( iState == CWmDrmDlaHandlerImpl::EIdle  )
        {
        iCurrentRequest = request;
        iState = CWmDrmDlaHandlerImpl::EProcessingMeteringCertificate;
        CompleteSelf();
        }
    else
        {
        iSyncRequest = request;
        }
    iWait->Start();
    LOG2( "iSyncReturnValue: %d", iSyncReturnValue );
    User::LeaveIfError( iSyncReturnValue );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::ProcessMeteringCertificate
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::ProcessMeteringCertificate( 
    const TDesC8& aCertificate,
    TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    LOGFNR( "CWmDrmDlaHandlerImpl::ProcessMeteringCertificate", err );
    
    CWmDrmDlaRequest* request( NULL );
    TRequestStatus *status = &aStatus;
    aStatus = KRequestPending;
    
    TRAP( err, request = CWmDrmDlaRequest::NewL( aCertificate, 
                                                 EFalse, 
                                                 aStatus ) );
    if ( err )
        {
        User::RequestComplete( status, err );
        }
    else if ( iState == CWmDrmDlaHandlerImpl::EIdle  )
        {
        iCurrentRequest = request;
        iState = CWmDrmDlaHandlerImpl::EProcessingMeteringCertificate;
        CompleteSelf();
        }
    else
        {
        err = iRequests.Append( request );
        if ( err )
            {
            delete request;
            User::RequestComplete( status, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CancelMetering
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::CancelMetering()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::CancelMetering" );
    
    //First check, if we are currently handling process metering request.
    //If we are, then cancel handling and cancel request 
    if ( iState == CWmDrmDlaHandlerImpl::ECompleted &&
         iCurrentRequest->iType 
             == CWmDrmDlaRequest::EProcessMeteringCertificate )
        {
        iMeteringFw->CancelMetering();
        Cancel();
        CompleteClientRequest( KErrCancel );
        CompleteSyncRequest( KErrCancel );
        }
    else if ( iState == CWmDrmDlaHandlerImpl::EProcessingMeteringCertificate )
        {
        Cancel();
        CompleteClientRequest( KErrCancel );
        CompleteSyncRequest( KErrCancel );
        }
    // Next go through the request queue,
    // remove all process metering requests and cancel them.
    TInt removed( 0 );
    for ( TInt i( 0 ); i - removed < iRequests.Count(); ++i )
        {
        if ( iRequests[ i - removed ]->iType 
                == CWmDrmDlaRequest::EProcessMeteringCertificate )
            {
            iCurrentRequest = iRequests[ i - removed ];
            iRequests.Remove( i - removed );
            ++removed;
            CompleteClientRequest( KErrCancel );
            }
        }
    if ( !iCurrentRequest )
        {
        NextRequest();
        }
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::SetIapId
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::SetIapId( TInt aIapId )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::SetIapId" );
    LOG2( "aIapId: %d", aIapId );
    iLicenseAcquisitionFw->SetIapId( aIapId );
    iMeteringFw->SetIapId( aIapId );
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::DoCancel
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::DoCancel()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::DoCancel" );
    //Don't need to do anything, canceling is implemented in
    //CancelLicenseAcquisiton() and in CancelMetering() -functions
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::RunL
// ---------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::RunL()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::RunL" );
    LOG2( "iState: %d", iState );
    LOG2( "iStatus.Int(): %d", iStatus.Int() );
    //If some error happened, then set state to completed, in which
    //the failed client request will be completed with the error code
    if ( iStatus.Int() )
        {
        iState = ECompleted;
        }
        
    switch ( iState )
	    {
	    case CWmDrmDlaHandlerImpl::ECompleted:
	        Completed( iStatus.Int() );
	        break;
    	
    	case CWmDrmDlaHandlerImpl::EAcquiringLicenseFromDrmHeader:
    	    AcquiringLicenseFromDrmHeader();
    	    break;
    	
    	case CWmDrmDlaHandlerImpl::EAcquiringLicense:
    	    AcquiringLicense();
    	    break;
    	
    	case CWmDrmDlaHandlerImpl::EProcessingMeteringCertificate:
    	    ProcessingMeteringCertificate();
    	    break;
    	
    	default:
            ASSERT( EFalse );
    	    break;
    	}
    }

// ---------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CWmDrmDlaHandlerImpl::RunError( TInt aError )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::RunError" );
    //Shouldn't ever happen, but better to be sure
    Completed( aError );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CompleteSelf
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::CompleteSelf()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::CompleteSelf" );
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CompleteClientRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::CompleteClientRequest( TInt aError )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::CompleteClientRequest" );
    LOG2( "aError: %d", aError );
    if ( iCurrentRequest && !iCurrentRequest->iSync )
        {
        User::RequestComplete( iCurrentRequest->iStatus, aError );
        delete iCurrentRequest;
        iCurrentRequest = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::CompleteSyncRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::CompleteSyncRequest( TInt aError )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::CompleteSyncRequest" );
    LOG2( "aError: %d", aError );
    if ( iCurrentRequest && iCurrentRequest->iSync )
        {
        iSyncReturnValue = aError;
        delete iCurrentRequest;
        iCurrentRequest = NULL;
        if ( iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::NextRequest
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::NextRequest()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::NextRequest" );
    //If there are queued request, take next one to handling.
    //Otherwise set state to idle
    if ( iRequests.Count() > 0 || iSyncRequest )
        {
        //synchronous request has higher priority than asynchronous one
        if ( iSyncRequest )
            {
            iCurrentRequest = iSyncRequest;
            iSyncRequest = NULL;
            }
        else
            {
            iCurrentRequest = iRequests[0];
            iRequests.Remove( 0 );
            }
        
        //Switch state to match the current request type
        switch ( iCurrentRequest->iType )
            {
            case CWmDrmDlaRequest::EAcquireLicenseFromDrmHeader:
                iState = CWmDrmDlaHandlerImpl::EAcquiringLicenseFromDrmHeader;
                break;
    
            case CWmDrmDlaRequest::EAcquireLicenseFromFileHandle:
                iState = CWmDrmDlaHandlerImpl::EAcquiringLicense;
                break;
                
            case CWmDrmDlaRequest::EProcessMeteringCertificate:
                iState = CWmDrmDlaHandlerImpl::EProcessingMeteringCertificate;
                break;
                    
            default:
                ASSERT( EFalse );
                break;
            }
        CompleteSelf();
        }
    else
        {
        iState = CWmDrmDlaHandlerImpl::EIdle; 
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::Completed
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::Completed( TInt aError )
    {
    LOGFN( "CWmDrmDlaHandlerImpl::Completed" );
    if ( iCurrentRequest->iSync )
        {
        CompleteSyncRequest( aError );
        }
    else
        {
        CompleteClientRequest( aError );
        }
    
    NextRequest();
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::AcquiringLicenseFromDrmHeader
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::AcquiringLicenseFromDrmHeader()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::AcquiringLicenseFromDrmHeader" );
    iLicenseAcquisitionFw->
        AcquireLicenseFromDrmHeader( *iCurrentRequest->iDrmHeader,
                                     iCurrentRequest->iErrorUrl,
                                     *iCurrentRequest->iContentUrl,
                                     *iCurrentRequest->iHtmlData,
                                     iStatus );
    SetActive();
    iState = CWmDrmDlaHandlerImpl::ECompleted; 
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::AcquiringLicense
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::AcquiringLicense()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::AcquiringLicense" );
    iLicenseAcquisitionFw->AcquireLicense( *iCurrentRequest->iFileHandle,
                                           *iCurrentRequest->iContentUrl,
                                           *iCurrentRequest->iHtmlData,
                                           iStatus );
    SetActive();
    iState = CWmDrmDlaHandlerImpl::ECompleted;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaHandlerImpl::ProcessingMeteringCertificate
// ----------------------------------------------------------------------------
//
void CWmDrmDlaHandlerImpl::ProcessingMeteringCertificate()
    {
    LOGFN( "CWmDrmDlaHandlerImpl::ProcessingMeteringCertificate" );
    iMeteringFw->ProcessMeteringCertificate( *iCurrentRequest->iCertificate,
                                             iStatus );
    SetActive();
    iState = CWmDrmDlaHandlerImpl::ECompleted;
    }

