/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Camese DRM Header filter
*
*/


// INCLUDES
#include <http.h>
#include <es_enum.h>
#include <utf.h>
#include <centralrepository.h> // - Album Download

#include "httpfiltercamesedrmheader.h"
#include "httpfiltercamesedatasupplier.h"
#include "cameselog.h"
#include "wmdrmdlahandler.h"

// CONSTANTS
// Filter position: DRM Header needs to be positioned below the client.
const TInt KCameseDrmHeaderFilterPosition = MHTTPFilter::EClient - 10;

const TUid KCRUidMusicShopSettings = {0x101FFB51}; // copied from MusicWapCenRepKeys.h - Album Download
const TUint32 KSupportOmaDownloadDescriptor   = 0x00000004; // copied from MusicWapCenRepKeys.h - Album Download
const TUint32 KMusicServiceFeatureFlagSupport = 0x00000012; // referred from 5.0 MusicWapCenRepKeys.h - Album Download

_LIT8( KCameseDrmHeaderFilterName, "CameseDrmHeader" );
_LIT8( KDataTypeCameseDRM,"application/vnd.ms-wmdrm.lic-chlg-req" ); 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::CHttpFilterCameseDrmHeader
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHttpFilterCameseDrmHeader::CHttpFilterCameseDrmHeader(RHTTPSession* aSession)
    : iSession(aSession),
      iTransactionId(KErrNotFound),
      iState(EIdle),
      iDataSupplierSet(EFalse)      
    {
    }


// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDrmHeader::ConstructL()
    {        
    iStringPool = iSession->StringPool();

    iStringPool.OpenL(RHTTPSession::GetTable());
    
    // Filter Name	
    RStringF filterName = iStringPool.OpenFStringL( KCameseDrmHeaderFilterName() );
    CleanupClosePushL( filterName );
	
    // Register the filter for the HTTP events we are interested.

    iSession->FilterCollection().AddFilterL(*this,   // The filter to add
                                            THTTPEvent::EGotResponseHeaders,        
                                            RStringF(),                              
                                            KAnyStatusCode,                          
                                            KCameseDrmHeaderFilterPosition,     
                                            filterName);
    
    iSession->FilterCollection().AddFilterL(*this,   // The filter to add
                                            THTTPEvent::EGotResponseBodyData,        
                                            RStringF(),                              
                                            KAnyStatusCode,                          
                                            KCameseDrmHeaderFilterPosition,     
                                            filterName);
    
    iSession->FilterCollection().AddFilterL(*this,   // The filter to add
                                            THTTPEvent::EResponseComplete,           
                                            RStringF(),                              
                                            KAnyStatusCode,                          
                                            KCameseDrmHeaderFilterPosition,     
                                            filterName);
                                            
    iSession->FilterCollection().AddFilterL(*this,   // The filter to add
                                            THTTPEvent::ECancel,           
                                            RStringF(),                              
                                            KAnyStatusCode,                          
                                            KCameseDrmHeaderFilterPosition,     
                                            filterName);
                                            
    CleanupStack::PopAndDestroy(&filterName);  
    }

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
//
CHttpFilterCameseDrmHeader::~CHttpFilterCameseDrmHeader()
    {
    CAMESE_LOG("CHttpFilterCameseDrmHeader::~CHttpFilterCameseDrmHeader");

    Reset();   
	if ( iLoadCount )
	    {
		// As we're already in a destructor, MHFUnload must not delete us again
		iLoadCount = -1;
        RStringF filterName;
		TRAPD( error, filterName = iStringPool.OpenFStringL( 
            KCameseDrmHeaderFilterName ) );
		if ( !error && iSession )
            {
	    	iSession->FilterCollection().RemoveFilter( filterName );
            }
        filterName.Close();  
	    }

    delete iDlaHandler;
	}

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpFilterCameseDrmHeader* CHttpFilterCameseDrmHeader::NewL( TAny* aSession )
    {
    ASSERT( aSession );
    CAMESE_LOG( "CHttpFilterCameseDrmHeader::NewL" );
    
    CHttpFilterCameseDrmHeader* filter = 
        new (ELeave) CHttpFilterCameseDrmHeader (reinterpret_cast< RHTTPSession* >( aSession ) );
    CleanupStack::PushL( filter );
    filter->ConstructL();
    CleanupStack::Pop( filter );
    return filter;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::MHFLoad
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDrmHeader::MHFLoad(
    RHTTPSession /*aSession*/, 
    THTTPFilterHandle /*aFilterHandler*/ )
    {
    CAMESE_LOG( "CHttpFilterCameseDrmHeader::MHFLoad" );
    ++iLoadCount;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::MHFUnload
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDrmHeader::MHFUnload(
    RHTTPSession /*aSession*/, 
    THTTPFilterHandle /*aFilterHandler*/ )
    {
    CAMESE_LOG( "CHttpFilterCameseDrmHeader::MHFUnload" );
    ASSERT( iLoadCount > 0 );

	if ( --iLoadCount )
	    {
		return;
        }
        
	delete this;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::MHFRunL
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDrmHeader::MHFRunL(
    RHTTPTransaction aTransaction,
    const THTTPEvent& aEvent )
    {
    // Check if we are interested in the transaction
    if ( (iTransactionId != KErrNotFound ) &&
         (aTransaction.Id() != iTransactionId) )
        {        
        return;
        }

    CAMESE_LOG1( "CHttpFilterCameseDrmHeader::MHFRunL aEvent=%i", 
        aEvent.iStatus );            
    switch( aEvent.iStatus )
        {
		case THTTPEvent::EGotResponseHeaders:
		    // Only interested in headers if in idle state.
		    if ( iState == EIdle )
		        {		        		    
                CheckResponseHeadersL( aTransaction );
		        }
			break;
			
	    case THTTPEvent::EGotResponseBodyData:
	        if ( iState == EDrmHeaderCapture )
	            {
	            CaptureDrmHeaderL( aTransaction );	            	        
	            }	        
	        break;
                	        
	    case THTTPEvent::EResponseComplete:
	        if ( iState == EDrmHeaderCapture )
	            {
	            RequestLicenseL( aTransaction );
	            }
	        break;  	        
	    
        case THTTPEvent::ECancel:
            CAMESE_LOG( 
                "CHttpFilterCameseDrmHeader::MHFRunL Handling Cancelation" );
            if ( iState == ERequestingLicense )
                {
                // Asynchronous cancelation while the
                // filter is blocked.
                
                CAMESE_LOG(
                    "CHttpFilterCameseDrmHeader::MHFRunL Canceling Camese" );
                // Need to unblock the license request.
                if ( iDlaHandler )
                    {
                    iDlaHandler->CancelLicenseAcquisition();
                    }    
                }
            break;
            
		default:
		    // Stray event
		    ASSERT( ETrue );
			break;
        }

    }

//------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::CheckResponseHeadersL
//------------------------------------------------------------------------
// 
void CHttpFilterCameseDrmHeader::CheckResponseHeadersL( 
    RHTTPTransaction& aTrans )
    {    
	RHTTPHeaders responseHeaders = aTrans.Response().GetHeaderCollection();
		
	RStringF contentTypeNameStr = 
	    iStringPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() );

	// read the first part of content-type field
	THTTPHdrVal contentTypeVal;

	if( !responseHeaders.GetField( contentTypeNameStr, 0, contentTypeVal ) )
    	{	    
        if ( contentTypeVal.StrF().DesC().MatchF( KDataTypeCameseDRM ) != 
            KErrNotFound )
	    	{
	    	CAMESE_LOG( "CHttpFilterCameseDrmHeader::CheckResponseHeaders Found Drm Header" );            
            StartDrmHeaderCaptureL( aTrans );
	    	}
    	}
    }
    
//------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::StartDrmHeaderCaptureL
//------------------------------------------------------------------------
// 
void CHttpFilterCameseDrmHeader::StartDrmHeaderCaptureL( 
    RHTTPTransaction& aTrans )
    {    
    // Start processing the Drm Header Packet.
    RHTTPHeaders responseHeaders = aTrans.Response().GetHeaderCollection();
    RStringF contentTypeNameStr = 
        iStringPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() );
    RStringF cacheControlStr = 
        iStringPool.StringF( HTTP::ECacheControl, RHTTPSession::GetTable() );
    RStringF noCacheVal = 
        iStringPool.StringF( HTTP::ENoCache, RHTTPSession::GetTable() );
        		    
    // Cache the transaction id
    iTransactionId = aTrans.Id();
	
    // Add a Cache-Control field indicating no-cache.
    // This will keep our response from being cached.
    responseHeaders.SetFieldL( cacheControlStr, THTTPHdrVal( noCacheVal ) );

    // Set the HTTP Status code as KErrCompletion - 
    // this will avoid letting download manager finalize
    // the temporary drm header download.
  
    // Album - Album Download
    
    TInt value( 0 );
    TBool omaDdSupported( EFalse );
    
    CRepository* repository( NULL );

    TRAPD( err, repository = CRepository::NewL( KCRUidMusicShopSettings ) );
    if ( !err )
        {
        // read value of KMusicServiceFeatureFlagSupport from repository
        // KCRUidMusicShopSettings and check if KSupportOmaDownloadDescriptor bit is set
        // in that value
        
        err = repository->Get( KMusicServiceFeatureFlagSupport, value );
        if ( !err )
            {
            // check for KSupportOmaDownloadDescriptor if the bit is set in value
            if ( value & KSupportOmaDownloadDescriptor )
                {
                omaDdSupported = ETrue;
                }
            }
        
        delete repository;
        }

    if ( !omaDdSupported )
        {        
        aTrans.Response().SetStatusCode( KErrCompletion );
        }
        
    // Album - Album Download

    // Change state to DRM Header Capture mode.
    iState = EDrmHeaderCapture;        
    }

//------------------------------------------------------------------------
// CHttpFilterCameseDownloader::ConnectionAccessPoint
//------------------------------------------------------------------------
// 
TInt CHttpFilterCameseDrmHeader::ConnectionAccessPoint(
    RHTTPTransaction& aTrans )
    {
    TInt ap( 0 );
                                                     
    // Retrieve connection information from the http session      
    const RHTTPConnectionInfo& connInfo = aTrans.Session().ConnectionInfo();
    RConnection* connPtr = NULL;
                
    THTTPHdrVal connHeader;
    TBool hasConnValue = connInfo.Property(
        iStringPool.StringF( 
            HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
        connHeader );
          	                                  
    // If we have access to the RConnection pointer, try to retrieve the IAP id.                 	                                 
    if ( hasConnValue )
        {
        // Got RConnection Property
        connPtr = REINTERPRET_CAST( RConnection*, connHeader.Int() );
        
        // Now retrieve the access point id.
        if ( connPtr )
            {                    
            TConnectionInfo connectionInfo;

            TUint count = 0;
            if ( !connPtr->EnumerateConnections(count) )
                {                            
                TPckg<TConnectionInfo> pkg(connectionInfo);
                if ( !connPtr->GetConnectionInfo( 1, pkg ) )
                    {
                    ap = connectionInfo.iIapId;
                    }
                }
            }
        }
        
    return ap;        
    }

//------------------------------------------------------------------------
// CHttpFilterCameseDownloader::CaptureDrmHeaderL
//------------------------------------------------------------------------
// 
void CHttpFilterCameseDrmHeader::CaptureDrmHeaderL( 
    RHTTPTransaction& aTrans )
    {    
    TPtrC8 ptr;

    MHTTPDataSupplier* dataSupplier = aTrans.Response().Body();			            
    dataSupplier->GetNextDataPart( ptr );

    // Append to iDrmHeaderBuffer
    if ( !iDrmHeaderBuffer )
        {
    	iDrmHeaderBuffer = ptr.AllocL();
    	}
    else
    	{
    	iDrmHeaderBuffer = iDrmHeaderBuffer->ReAllocL(
    	    iDrmHeaderBuffer->Length() + ptr.Length() );
    	iDrmHeaderBuffer->Des().Append( ptr );
    	}

    if ( !iDataSupplierSet )    			
        {            
        // Feed the actual client with our data supplier, that will feed an empty
        // descriptor.
        delete iClientDataSupplier;
        iClientDataSupplier = NULL;		    	
        iClientDataSupplier = 
            CHttpFilterCameseDataSupplier::NewL( aTrans.Response().Body(),
                KNullDesC8() );
            
        aTrans.Response().SetBody( *iClientDataSupplier );
        iDataSupplierSet = ETrue;
        }            
    }

//------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::RequestLicenseL
//------------------------------------------------------------------------
// 
void CHttpFilterCameseDrmHeader::RequestLicenseL( RHTTPTransaction& aTrans )
    {    
    ASSERT( iDrmHeaderBuffer );
    CAMESE_LOG(
        "CHttpFilterCameseDrmHeader::RequestLicenseL Passing in Drm Header" );
    
    if ( !iDlaHandler )
        {
        iDlaHandler = CWmDrmDlaHandler::NewL();
        }
    
    // Pass in our current access point to the drm interface.
    iDlaHandler->SetIapId( ConnectionAccessPoint( aTrans ) );
    
    // Use the Camese Interface, passing in
    // the DRM header url. WMDRM DLA Handler uses it for
    // error reporting.
    HBufC8* errorUrl8( aTrans.Request().URI().UriDes().AllocLC() );   
    HBufC* errorUrl( CnvUtfConverter::ConvertToUnicodeFromUtf8L( *errorUrl8 ) );
    CleanupStack::PushL( errorUrl );
    
    HBufC* contentUrl( NULL );
    HBufC* htmlData( NULL );
    
    // Update state
    iState = ERequestingLicense;
    
    // This call blocks and only returns after Camese is done with the licensing
    // process.
    TRAPD( result, iDlaHandler->AcquireLicenseFromDrmHeaderL( *iDrmHeaderBuffer,
                                                              errorUrl,
                                                              contentUrl,
                                                              htmlData ) );
    
    CAMESE_LOG("CHttpFilterCameseDrmHeader::RequestLicenseL Unblocked");
    
    delete htmlData;
    
    HBufC8* contentUrl8( NULL );
    if ( contentUrl )
        {
        CleanupStack::PushL( contentUrl );
        contentUrl8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *contentUrl );
        CleanupStack::PopAndDestroy( contentUrl );
        }
    CleanupStack::PushL( contentUrl8 );
    
    
    // Check if the filter has not been canceled.
    if ( result != KErrCancel )
        {        
        HandleRedirectL( result, aTrans, contentUrl8 );
        }
    CleanupStack::PopAndDestroy( 3, errorUrl8 ); //contentUrl8, errorUrl, errorUrl8
    
    // Reset state machine.        
    Reset();        
    }

//------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::HandleRedirectL
//------------------------------------------------------------------------
// 
void CHttpFilterCameseDrmHeader::HandleRedirectL(
    TInt aResult,
    RHTTPTransaction& aTrans,
    HBufC8*& aContentUrl )
    {        
    // Now check if we have a content URL to redirect to,
    // and if the content URL is not the DRM Header URL we
    // passed in.
    if ( aContentUrl && aContentUrl->CompareF( aTrans.Request().URI().UriDes() ) )
        {                   
        TUriParser8 uri;
        TInt err = uri.Parse( *aContentUrl );
            
        if ( !err )
            {   
            CAMESE_LOG( 
                "CHttpFilterCameseDrmHeader::RequestLicenseL Redirecting" );
            CAMESE_LOG8_1( "   > ContentUrl = %S", aContentUrl );         
            CAMESE_LOG1( "   > Licensing Result = %d", aResult );
            
            // Cancel the Transaction.
            aTrans.Cancel();

            // Set new URI - could be content or error page.
            aTrans.Request().SetURIL( uri );
            
            
            // update for - Album Download
            // Manually replace HOST firld in the HTTP header to reflect the new host
            //
            RHTTPHeaders headers = aTrans.Request().GetHeaderCollection();
            RStringF hostStr = aTrans.Session().StringPool().StringF(HTTP::EHost, RHTTPSession::GetTable());
            CleanupClosePushL(hostStr);
            TInt error = headers.RemoveField(hostStr);
            CAMESE_LOG1("   error for removing HOST field from header = %d", error);
            if (error == KErrNone || error == KErrNotFound)
	            {
	            HBufC* host =aTrans.Request().URI().DisplayFormL(EUriHost);
	            CleanupStack::PushL(host);
	            HBufC8* host8 = HBufC8::NewL(host->Length());
	            CleanupStack::PushL(host8);
	            host8->Des().Copy(*host);
	            CAMESE_LOG8_1("   > new host = %S", host8);
	            RStringF hostValueStr = aTrans.Session().StringPool().OpenFStringL(*host8);
	            CleanupClosePushL<RStringF>(hostValueStr);
	            headers.SetFieldL( hostStr, THTTPHdrVal(hostValueStr) );
	            CleanupStack::PopAndDestroy(3, host); // hostValueStr, host8, host
	            }
            else
	            {
	            User::Leave(error);
	            }
            CleanupStack::PopAndDestroy(&hostStr);
            // update end for - Album Download
        
            // Submit the new transaction.		                	                	            
            aTrans.SubmitL();  
            }
        else
            {
            CAMESE_LOG( 
                "CHttpFilterCameseDrmHeader::RequestLicenseL Malformed Url" );
            // Fail the transaction
            aTrans.Fail();
            }                
        }
    else
        {
        CAMESE_LOG("CHttpFilterCameseDrmHeader::RequestLicenseL No valid redirection Url");
        // Fail the transaction
        aTrans.Fail();
        }
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::Reset
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDrmHeader::Reset()
    { 
    CAMESE_LOG("CHttpFilterCameseDrmHeader::Reset");   
    iState = EIdle;
    iTransactionId = KErrNotFound;
    
    delete iDrmHeaderBuffer;
    iDrmHeaderBuffer = NULL;   
    
    delete iClientDataSupplier;
    iClientDataSupplier = NULL;
    iDataSupplierSet = EFalse;
    
    delete iDlaHandler;
    iDlaHandler = NULL;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::MHFRunError
// -----------------------------------------------------------------------------
//
TInt CHttpFilterCameseDrmHeader::MHFRunError(
    TInt aError,
    RHTTPTransaction aTransaction,
    const THTTPEvent& aEvent )
    {
    // Log errors
    CAMESE_LOG2("CHttpFilterCameseDrmHeader::MHFRunError Error = %i Event = %i",
                aError,
                aEvent.iStatus);
                  
    aTransaction.Fail();
    
    Reset();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::MHFSessionRunL
// -----------------------------------------------------------------------------
//
void CHttpFilterCameseDrmHeader::MHFSessionRunL(
    const THTTPSessionEvent& /*aEvent*/ )
    {
    }

// -----------------------------------------------------------------------------
// CHttpFilterCameseDrmHeader::MHFSessionRunError
// -----------------------------------------------------------------------------
//
TInt CHttpFilterCameseDrmHeader::MHFSessionRunError(
    TInt /*aError*/,
    const THTTPSessionEvent& /*aEvent*/ )
    {
    return KErrNone;
    }

//  End of File
