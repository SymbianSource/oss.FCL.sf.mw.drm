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
*  wmdrmotaaccessecominterface.h
*
*/

#ifndef _WMDRMOTAACCESSECOMINTERFACE_H_
#define _WMDRMOTAACCESSECOMINTERFACE_H_

#include <e32base.h>

// Class declaration
// CWmDrmOtaAccessEcomInterface
// ecom plugin interface for WM DRM ota access implementation
// @see CWmDrmOtaAccess
NONSHARABLE_CLASS( CWmDrmOtaAccessEcomInterface ) : public CBase
    {
public:
    // Constructors
    static CWmDrmOtaAccessEcomInterface* NewL();
    static CWmDrmOtaAccessEcomInterface* NewLC();

    // Destructor
    virtual ~CWmDrmOtaAccessEcomInterface();

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
    virtual void GetLicenseChallengeL( const TDesC8& aDrmHeader, 
                               HBufC8*& aChallenge ) = 0;

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
    virtual void GetLicenseChallengeL( const TDesC8& aDrmHeader, 
                                       HBufC8*& aChallenge, 
                                       TRequestStatus& aStatus ) = 0;
    
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
    virtual void HandleLicenseResponseL( const TDesC8& aResponse ) = 0;

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
    virtual void HandleLicenseResponseL( const TDesC8& aResponse, 
                                         TRequestStatus& aStatus ) = 0;
                                              
    /**
    *  CancelAllAsync
    *
    *  Cancel all async methods        
    *
    *  @return  None    
    */
    virtual void CancelAllAsync() = 0;                                              
                                         

private:
    // internal variable used on freeing ecom related resources
    TUid iDtor_ID_Key;
    };

#include "wmdrmotaaccessecominterface.inl"

#endif _WMDRMOTAACCESSECOMINTERFACE_H_

