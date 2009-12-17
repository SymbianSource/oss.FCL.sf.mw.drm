/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Pure virtual interface for WMDRM DLA plugins
*
*/


#ifndef C_WMDRMDLAHTTPPLUGINIF_H
#define C_WMDRMDLAHTTPPLUGINIF_H

#include <e32base.h>
#include <f32file.h>

class CWmDrmDlaUiNotifier;
class CGulIcon;

/**
 *  Interface which all WMDRM DLA HTTP Plugins have to implement
 *
 *  @since S60 v9.1
 */
class CWmDrmDlaHttpPluginIf : public CBase
    {

public:

    // Instantiates an object of this type 
    inline static CWmDrmDlaHttpPluginIf* NewL( TUid aUid );

    // Destructor.
    inline virtual ~CWmDrmDlaHttpPluginIf();

    /**
     * Get service icon to be used in service selection query, if
     * more than one service is recognized to support content.
     * Will be deleted by the caller.
     *
     * @return Service icon or NULL
     */
    virtual CGulIcon* ServiceIcon() = 0;
    
    /**
     * Check if file is supported by this plugin
     *
     * @param aFile WMDRM file
     * @param aUiNotifer If file is supported, contains initialized UI Notifier
     *                   upon completion. Will be deleted by the caller after 
     *                   the plugin has been deleted.
     * @return ETrue if file is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
     virtual TBool SupportedFileL( const RFile& aFile, 
                                   CWmDrmDlaUiNotifier*& aUiNotifier ) = 0;

    /**
     * Check if DRM Header is supported by this plugin
     *
     * @param aHeader DRM Header
     * @param aUiNotifer If headr is supported, contains initialized UI Notifier
     *                   upon completion. Will be deleted by the caller after 
     *                   the plugin has been deleted.
     * @return ETrue if header is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    virtual TBool SupportedDrmHeaderL( const TDesC8& aHeader, 
                                       CWmDrmDlaUiNotifier*& aUiNotifier ) = 0;

    /**
     * Check if metering certificate is supported by this plugin
     *
     * @param aCertificate Metering certificate
     * @param aUiNotifer If certificate is supported, contains initialized 
     *                   UI Notifier upon completion. Will be deleted by the
     *                   caller after the plugin has been deleted.
     * @return ETrue if certificate is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    virtual TBool SupportedMeteringCertificateL( 
                                    const TDesC8& aCertificate, 
                                    CWmDrmDlaUiNotifier*& aUiNotifier ) = 0;

    /**
     * Check if initiator is supported by this plugin
     * This method is called only for playready content
     * 
     * @param aInitiator Initiator
     * @param aUiNotifer If initiator is supported, contains initialized 
     *                   UI Notifier upon completion. Will be deleted by
     *                   the caller after the plugin has been deleted.
     * @return ETrue if initiator is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    virtual TBool SupportedInitiatorL( const TDesC8& aInitiator, 
                                       CWmDrmDlaUiNotifier*& aUiNotifier ) = 0;

    /**
     * Check if license acquisition should be silent for this file
     *
     * @param aFile WMDRM file
     * @return ETrue if file is supported by this plugin
     * @leave with standard Symbian error code in case of error
     */
    virtual TBool SilentL( const RFile& aFile ) = 0;

    /**
     * Process DRM Header. This method is used only to pass DRM Header to the
     * plugin. The framework does not require the plugin to do anything 
     *
     * @param aDrmHeader DRM Header
     * @leave with standard Symbian error code in case of fatal error (the 
     * framework does not proceed with license acquisition if this method
     * leaves)
     */
    virtual void ProcessDrmHeaderL( const TDesC8& aDrmHeader ) = 0;

    /**
     * Process the initiator. This method is used only to pass initiator to the
     * plugin. The framework does not require the plugin to do anything. 
     * This method is called only for playready content
     *
     * @param aInitiator Initiator
     * @leave with standard Symbian error code in case of fatal error (the 
     * framework does not proceed with license acquisition if this method
     * leaves)
     */
    virtual void ProcessInitiatorL( const TDesC8& aInitiator ) = 0;

    /**
     * Acquire License. 
     *
     * @param aLicenseChallenge License challenge based on which license will
     *                          be acquired.
     * @param aLicenseServerUrl URL where license challenge is sent
     * @param aStatus Request status, will contain error code upon completion 
     * including but not limited to:
     * - KErrCancel if user cancelled the license acquisition at any point
     * - KErrCommsBreak if network connection was lost during communication 
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    virtual void AcquireLicense( const TDesC8& aLicenseChallenge, 
                                 HBufC* aLicenseServerUrl, 
                                 TRequestStatus& aStatus ) = 0;

    /**
     * Get downloaded License. 
     *
     * @leave standard Symbian error code in case of error.
     * @return Buffer containing license data. Will be deleted by the caller.
     */
    virtual HBufC8* GetLicenseResponseL() = 0;

    /**
     * Send license storage status. 
     *
     * @param aTid Transaction ID parsed from the license response, can be NULL
     * @param aUrl URL where license storage status is sent
     * @param aError License storage status.
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    virtual void SendLicenseStorageStatus( HBufC8* aTid, const TDesC& aUrl, 
                                           TInt aError, 
                                           TRequestStatus& aStatus ) = 0;

    /**
     * Send error code to server if drm header parsing failed.
     *
     * @param   aErrorUrl   URL where error code is sent
     * @param   aError      Error code
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    virtual void SendDrmHeaderError( const TDesC& aErrorUrl, TInt aError,
                                     TRequestStatus& aStatus ) = 0;

    /**
     * Get content url. Called only if framework fails to parse content URL 
     * from the license response. Needs to be implemented only if the supported
     * service uses non-standard way to pass the Content URL, otherwise this
     * can return NULL.
     *
     * @return Content URL, can be NULL. Will be deleted by the caller.
     */
    virtual HBufC* GetContentUrlL() = 0;

    /**
     * Get HTML data returned by license server. 
     *
     * @return HTML data, can be NULL. Will be deleted by the caller.
     */
    virtual HBufC* GetHtmlDataL() = 0;

    /*
     * Cancel license acquisition
     */
    virtual void CancelLicenseAcquisition() = 0;
    
    /**
     * Process metering challenge. 
     *
     * @param aMeteringChallenge Metering challenge
     * @param aUrl Url to be used in metering request
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                   with server
     * - KErrGeneral if unspecified error happened
     */
    virtual void ProcessMeteringChallenge( const TDesC8& aMeteringChallenge, 
                                           const TDesC& aUrl, 
                                           TRequestStatus& aStatus ) = 0;

    /**
     * Get metering response. 
     *
     * @param aResponse Contains metering response upon completion. Will be
     *                  deleted by the caller.
     * @param aStatus Request status, will contain error code upon completion
     */
    virtual void GetMeteringResponse( HBufC8*& aResponse, 
                                      TRequestStatus& aStatus ) = 0;

    /**
     * Inform plugin that metering finished. 
     *
     */
    virtual void MeteringFinished() = 0;
    
    /*
     * Cancel metering
     */
    virtual void CancelMetering() = 0;

    /*
     * Process join domain challenge.
     * This method is called only for playready content
     *
     * @param aJoinDomainChallenge Join domain challenge
     * @param aUrl Url to be used in join domain request
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    virtual void ProcessJoinDomainChallenge( 
        const TDesC8& aJoinDomainChallenge, 
        const TDesC& aUrl, 
        TRequestStatus& aStatus ) = 0;

    /*
     * Process leave domain challenge.
     * This method is called only for playready content
     *
     * @param aLeaveDomainChallenge Leave domain challenge
     * @param aUrl Url to be used in leave domain request
     * @param aStatus Request status, will contain error code upon completion
     * including but not limited to:
     * - KErrCommsBreak if network connection was lost during communication
     *                  with server
     * - KErrGeneral if unspecified error happened
     */
    virtual void ProcessLeaveDomainChallenge( 
        const TDesC8& aLeaveDomainChallenge, 
        const TDesC& aUrl, 
        TRequestStatus& aStatus ) = 0;

    /**
     * Sets the IAP to be used in the HTTP transactions
     *
     * @param   aIapId  Internet Access Point id
     */
    virtual void SetIapId( TInt aIapId ) = 0;
    
private: // data

    TUid iDlaHttpPluginUid;

    };

#include <wmdrmdlahttpplugin.inl>

#endif // C_WMDRMDLAHTTPPLUGINIF_H
