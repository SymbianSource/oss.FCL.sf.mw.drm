/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRMSettinsPlugin model.
*
*/

#ifndef  DRMSETTINGSMODEL_H
#define  DRMSETTINGSMODEL_H

// INCLUDES
#include    <centralrepository.h>
#include    <e32property.h>
#include    "RoapStorageClient.h"

// FORWARD DECLARATIONS
class CDRMRIContext;

// CONSTANTS
const TInt KDRMTransactionTrackingDisabled = 0;
const TInt KDRMTransactionTrackingEnabled = 1;

const TInt KDRMAutomaticActivationNotAllowed = 0;
const TInt KDRMAutomaticActivationAllowed = 1;


// CLASS DEFINITIONS

/**
*  CDRMSettingsModel is the model class of DRMSettingsPlugin.
*  It provides functions to get and set setting values.
*/
class   CDRMSettingsModel : public CBase
    {

    public:  // Constructor and destructor
        /**
        * Two-phased constructor
        */
        static CDRMSettingsModel* NewL();

        /**
        * Destructor
        */
        ~CDRMSettingsModel();

    public: // New

        /**
        * Get transaction tracking state
        * @return KDRMTransactionTrackingDisabled
        *         KDRMTransactionTrackingEnabled
        */
        TInt TransactionTrackingStateL();

        /**
        * Set transaction tracking state
        * @param aValue updated value
        */
        void SetTransactionTrackingStateL( TInt aValue );

        /**
        * Get automatic activation state
        * @return KDRMAutomaticActivationNotAllowed
        *         KDRMAutomaticActivationAllowed
        */
        TInt AutomaticActivationStateL();

        /**
        * Set automatic activation state
        * @param aValue updated value
        */
        void SetAutomaticActivationStateL( TInt aValue );

        /**
        * Get usage reporting state
        * @return count of services for which reporting is allowed
        */
        TInt UsageReportingCount();

        /**
        * Get RI alias
        *
        * @param aIndex : Index of the RI context
        *
        * @return RI alias name or RI url if no alias available
        */
        HBufC* GetSingleRIAliasL( TInt aIndex );

        /**
        * Get the first RI alias, where metering is allowed
        * @return RI alias name or RI url if no alias available
        */
        HBufC* GetFirstAllowedMeteringRIAliasL();

        /**
        * Get number of RI contexts
        * @return number of RIContexts
        */
        TInt GetRiContextCount();

        /**
        * Get metering status of RI Context
        *
        * @param aIndex : Index of the RI context
        *
        * @return Status of metering
        */
        TBool IsMeteringAllowed( TInt aIndex );

        /**
        * Find out if metering is allowed for all RI Contexts
        *
        * @return Status of metering
        */
        TBool IsMeteringAllowedForAll();

        /**
        * Sets the value for metering to be enabled or disabled
        *
        * @param aIndex : Index of the RI context
        * @param aIsAllowed : ETrue if set to allowed, EFalse if not allowed
        */
        void SetMeteringStatus( TInt aIndex, TBool aIsAllowed );

        /**
        * Save the changes done to RI Contexs
        */
        void SaveMeteringChanges();

    private: // Private constructors

        /**
        * Default C++ contructor
        */
        CDRMSettingsModel();

        /**
        * Symbian OS default constructor
        * @return void
        */
        void ConstructL();

    private: // Data

        // DRM Settings Central Repository.
        CRepository* iDRMSettingsRepository;

        // Roap storage client
        Roap::RRoapStorageClient* iRoapStorageClient;

        // List of service providers with which device has been registered
        RPointerArray<CDRMRIContext> iRIContextList;

    };

#endif // DRMSETTINGSMODEL_H

// End of File
