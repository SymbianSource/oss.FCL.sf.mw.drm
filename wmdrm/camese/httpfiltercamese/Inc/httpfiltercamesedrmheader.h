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
* Description:  Camese DRM Header HTTP filter
*
*/



#ifndef HTTPFILTERCAMESEDRMHEADER_H
#define HTTPFILTERCAMESEDRMHEADER_H

//  INCLUDES
#include <http/mhttpfilter.h>
#include <http/cecomfilter.h>

// FORWARD DECLARATIONS
class RSocketServ;
class RConnection;
class CHttpFilterCameseNoteManager;
class CHttpFilterCameseDataSupplier;
class CWmDrmDlaHandler;

// CLASS DECLARATION

/**
*  Camese Drm Header Filter implementation class.
*  @since 3.1
*/
class CHttpFilterCameseDrmHeader : public CEComFilter,
                                   public MHTTPFilter
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aSession pointer to the http session
    * @return an instance of the drm header filter
    */
    static CHttpFilterCameseDrmHeader* NewL(TAny* aSession);

    /**
    * Destructor.
    */
    virtual ~CHttpFilterCameseDrmHeader();

// From MHTTPFilter
public:

    /**
    * @see MHTTPFilter
    */
    virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

    /**
    * @see MHTTPFilter
    */
    virtual void MHFSessionRunL(const THTTPSessionEvent& aEvent);

    /**
    * @see MHTTPFilter
    */
    virtual TInt MHFRunError(TInt aError,
                             RHTTPTransaction aTransaction,
                             const THTTPEvent& aEvent);

    /**
    * @see MHTTPFilter
    */
    virtual TInt MHFSessionRunError(TInt aError, const THTTPSessionEvent& aEvent);

    /**
    * @see MHTTPFilter
    */
    virtual void MHFUnload(RHTTPSession aSession, THTTPFilterHandle aHandle);

    /**
    * @see MHTTPFilter
    */
    virtual void MHFLoad(RHTTPSession aSession, THTTPFilterHandle aHandle);

private:
    /**
    * States during Drm Header Capture
    */
    enum TState
        {
        EIdle,
        EDrmHeaderCapture,
        ERequestingLicense       
        };
    
private:

    /**
    * C++ constructor.
    * @param aSession pointer to the http session
    */
    CHttpFilterCameseDrmHeader(RHTTPSession* aSession);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:
    
    /**
    * Parses the response headers to look for the DRM header
    * mime type.
    * @param aTrans the HTTP Transaction
    */
    void CheckResponseHeadersL( RHTTPTransaction& aTrans );
    
    /**
    * Starts DRM Header capture.
    * @param aTrans the HTTP Transaction
    */
    void StartDrmHeaderCaptureL( RHTTPTransaction& aTrans );

    /**
    * Captures the DRM header for further processing.
    * @param aTrans the HTTP Transaction
    */
    void CaptureDrmHeaderL( RHTTPTransaction& aTrans );
    
    /**
    * Requests Camese License Processing
    * @param aTrans the HTTP Transaction
    */
    void RequestLicenseL( RHTTPTransaction& aTrans );
    
    /**
    * Handles redirecting the filter to a media download or web page.
    * @param aResult the result of the license requisition operation.
    * @param aTrans the HTTP Transaction
    * @param aContentUrl the URL for redirection.
    */
    void HandleRedirectL(TInt aResult,
                         RHTTPTransaction& aTrans,
                         HBufC8*& aContentUrl);
    /**
    * Reset Filter State machine
    */
    void Reset();

    /**
    * Retrieves the connection access point id to pass in to the Camese Utility.
    * @param aTrans the HTTP Transaction
    * @return the AP Id
    */
    TInt ConnectionAccessPoint(RHTTPTransaction& aTrans);        
    
private:    // Data
	
    // Count of how many times this filter has been loaded - used to ensure 
    // destruction happens at the correct time	
    TInt iLoadCount;

    // The HTTP Session for which the filter was loaded
    RHTTPSession*   iSession;                                   // not owned
    
    CHttpFilterCameseDataSupplier* iClientDataSupplier;         // not owned
    RStringPool iStringPool;
	
    // Drm Header Transaction Id
    TInt iTransactionId;
	
    // Filter state machine
    TState iState;
	
	// Indicates that the data supplier has been set for the transaction
	TBool iDataSupplierSet;
	
	//WMDRM DLA Handler
	CWmDrmDlaHandler* iDlaHandler;                              // owned
	
    // Drm Header Buffer
    HBufC8* iDrmHeaderBuffer;                                   // owned
	};

#endif      // HTTPFILTERCAMESEDRMHEADER_H

// End of File
