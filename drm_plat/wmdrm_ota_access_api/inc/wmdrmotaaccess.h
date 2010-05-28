/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*  wmdrmotaaccess.h
*
*/

#ifndef _WMDRM_OTA_ACCESS_API_H_
#define _WMDRM_OTA_ACCESS_API_H_

// Forward declarations
class CWmDrmOtaAccessEcomInterface;

// Class declaration
/**
*  Provides access to WM DRM Over-the-Air
*  Functionality
*
*  @code
*  Only one asynchronous operation can be happening at a time.
*  If another call for async operation is received, this will result in
*  KErrInUse leave code.
*
*  CWmDrmOtaAccess* access( CWmDrmOtaAccess::NewL() );
*  delete access;
*  @endcode
*
*/
NONSHARABLE_CLASS( CWmDrmOtaAccess ) : public CBase
    {     
public:
    /** Constructors */
    IMPORT_C static CWmDrmOtaAccess* NewL();
    IMPORT_C static CWmDrmOtaAccess* NewLC();

    /** Destructor */
    IMPORT_C virtual ~CWmDrmOtaAccess();
    
    /**
    *  GetLicenseChallengeL
    *  Syncronous method
    *
    *  Get the license challenge that can be sent to the server to fetch
    *  a license
    *
    *  @param   aDrmHeader  IN  The drm header
    *  @param   aChallenge  OUT Resulting Challenge
    *  @return  None
    *  @leave   Symbian OS error codes      
    */
    IMPORT_C void GetLicenseChallengeL( const TDesC8& aDrmHeader, 
                                        HBufC8*& aChallenge );

    /**
    *  GetLicenseChallengeL
    *  Asyncronous method
    *
    *  Get the license challenge that can be sent to the server to fetch
    *  a license    
    *
    *  @param   aDrmHeader  IN  The drm header
    *  @param   aChallenge  OUT Resulting Challenge
    *  @param   aStatus     OUT Asyncronous request status
    *  @return  None
    *  @leave   Symbian OS error codes      
    */
    IMPORT_C void GetLicenseChallengeL( const TDesC8& aDrmHeader, 
                                        HBufC8*& aChallenge, 
                                        TRequestStatus& aStatus );
    
    /**
    *  HandleLicenseResponseL
    *  Syncronous method
    *
    *  Handle the response received from the server in correspondence
    *  to the license challenge
    *
    *  @param   aResponse   IN  The license response
    *  @return  None
    *  @leave   Symbian OS error codes        
    */
    IMPORT_C void HandleLicenseResponseL( const TDesC8& aResponse );

    /**
    *  GetLicenseChallengeL
    *  Asyncronous method
    *
    *  Handle the response received from the server in correspondence
    *  to the license challenge
    *    
    *  @param   aResponse   IN  The license response
    *  @param   aStatus     OUT Asyncronous request status    
    *  @return  None
    *  @leave   Symbian OS error codes      
    */
    IMPORT_C void HandleLicenseResponseL( const TDesC8& aResponse, 
                                          TRequestStatus& aStatus );
                                          
                                          
    /**
    *  CancelAllAsync
    *
    *  Cancel all async methods        
    *
    *  @return  None    
    */
    IMPORT_C void CancelAllAsync();                              
                                          

private:
    /** Default constructor */
    CWmDrmOtaAccess();
    /** Second phase constructor */
    void ConstructL();

    /** Internal interface to the WM DRM system */
    CWmDrmOtaAccessEcomInterface* iWmDrmOtaAccessEcomInterface;

    };
#endif // _WMDRM_OTA_ACCESS_API_H_

