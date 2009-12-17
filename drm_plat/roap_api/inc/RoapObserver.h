/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ROAP observer interface
*
*/



#ifndef ROAP_OBSERVER_H
#define ROAP_OBSERVER_H


// FORWARD DECLARATIONS
class CDRMRights;

// CLASS DECLARATION

/**
*  An interface containing callback methods for ROAP transaction
*
*  @since 3.0
*/

namespace Roap
    {
    class MRoapObserver
        {
        public:

        /**
        * ConnectionConfL
        *
        * The function is called by ROAP engine when a network connection is to
        * be established. Return value ETrue indicates to the ROAP engine that
        * the user consent has been retrieved to create the network connection.
        *
        * @since  3.0
        *
        * @return ETrue: the network can be establish
        *         EFalse: the ROAP transaction is canceled
        *
        * @leave  System wide error code */

        virtual TBool ConnectionConfL() = 0;

        /**
        * ContactRiConfL
        *
        *
        * @since  3.0
        *
        * @return ETrue: the user consent is achieved
        *         EFalse: the user consent is not achieved
        *
        * @leave  System wide error code */

        virtual TBool ContactRiConfL() = 0;

        /**
        * TransIdConfL
        *
        * The function is called during RO acquisition when the ROAP engine wants
        * to perform trasaction tracking. The user conset must be asked before
        * the operation.
        *
        * @since  3.0
        *
        * @return ETrue: the user consent is achieved
        *         EFalse: the user consent is not achieved
        *
        * @leave  System wide error code */

        virtual TBool TransIdConfL() = 0;

        /**
        * RightsObjectDetailsL
        *
        * The function is called after successful completion of RO acquisition
        * protocol. The passes information about stored rights objects to the
        * observer.
        *
        * @since  3.0
        * @param aRightsList: A list of pointers to rights objects.
        *                     Contents of aRightsList are owend by ROAP engine
        *
        * @leave  System wide error code */

        virtual void RightsObjectDetailsL( const RPointerArray<CDRMRights>& aRightsList ) = 0;

        /**
        * ContentDownloadInfoL
        *
        * The function is called when the ROAP engine notices that it is about to
        * receive a multipart content as a ROAP response. The method must return via
        * out-parameter the path to a temp folder where the content is saved during
        * download. The name that is used as filename when saving the content to
        * the appropriate palce and the maximum size of the content (a safety upper limit)
        * must also be provided via out-parameters.
        *
        * @since 3.0
        * @param aTempFolder: (out-param) The path of the temp folder, or KNullDesC
        * @param aContentName: (out-param) The name of the content, or KNullDesC (defaut name is used)
        * @param aMaxSize: (out-param) The maximum size of the content, or -1 if not known
        *
        * @leave System wide error code */

        virtual void ContentDownloadInfoL( TPath& aTempFolder,
                                           TFileName& aContentName,
                                           TInt& aMaxSize ) = 0;

        /**
        * ContentDetailsL
        *
        * The function is called when the ROAP engine has received a DCF content (together
        * with a ROAP response) The ROAP engine saves the DCF to the appropriate location
        * and gives out the information about the saved DCF file by calling this method
        *
        * @since 3.0
        * @param aPath: The path andf the filename of the saved DCF file
        * @param aType: The plain MIME type of the saved DCF file
        * @param aAppUid: The handler app UID of the saved DCF file
        *
        * @leave System wide error code */

        virtual void ContentDetailsL( const TDesC& aPath,
                                      const TDesC8& aType,
                                      const TUid& aAppUid ) = 0;

        /**
        * RoapProgressInfoL
        *
        * The function provides progress information about ROAP processing to the
        * observer
        *
        * @since  3.0
        * @param aProgressInfo: An integer value representing the state of
        *                       ROAP processing, counting in bytes
        *
        * @leave  System wide error code */

        virtual void RoapProgressInfoL( const TInt aProgressInfo ) = 0;

        /**
        * ErrorUrlL
        *
        * The function is called in ROAP error case and it provides an error URL
        * for the caller. The calling entity should send an HTTP GET request to
        * the URL which should then return an HTML page.
        *
        * @since  3.0
        * @param aErrorUrl: The error URL
        *
        *
        * @leave  System wide error code */

        virtual void ErrorUrlL( const TDesC8& aErrorUrl ) = 0;

        /**
        * PostResponseUrlL
        *
        * The function is called if handled ROAP metering report response
        * contains PostResponseURL extension. The calling entity should send
        * an HTTP GET request to the URL, which should return ROAP trigger,
        * download descriptor or multipart message containing both
        * download descriptor and ROAP trigger.
        *
        * @since  3.2
        * @param aPostResponseUrl: The URL to be fetched
        *                          after handling ROAP metering response
        *                          with PostResponseURL extension.
        *
        *
        * @leave  System wide error code */

        virtual void PostResponseUrlL( const TDesC8& aPostResponseUrl ) = 0;

        };
    }
#endif  // ROAP_OBSERVER_H

// End of File
