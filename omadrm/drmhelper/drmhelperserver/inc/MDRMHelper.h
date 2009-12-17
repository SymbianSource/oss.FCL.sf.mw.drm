/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __MDRMHELPER_H__
#define __MDRMHELPER_H__

//  INCLUDES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMRightsConstraints;
class TDataType;
class CCoeEnv;

// CLASS DECLARATION

class MDRMHelper
    {
    public:
    
    enum TNotificationID
        {
        ETActivateTextId = 1,
        ETCountBasedErrorTextId = 2 ,
        ETCountUsageReduceId = 3,
        ETCountUsageReduceMediaId = 4
        };

    enum TConsumeAction
        {
        EStart,
        EFinish,
        };

    enum TOMALevel
        {
        EOMA_None, 
        EOMA_1_0,
        EOMA_2_0
        };

    public: // New functions
        
        /**
        * Displays appropriate error note depending on error code.
        * @param aError: error code returned by DRM engine
        * @param aURI: URI of the file
        * @return 
        * - Button code, if user selected "buy rights" or "launch rights 
        *   manager" in query.
        * - 0 if user selected "No", "Back" or "Cancel" (or whatever which 
        *   just dismiss the query)
        * - 0 if just a simple error note is displayed
        */
        virtual TInt HandleErrorL( TInt aError, 
           const TDesC8& aURI ) = 0;

        /**
        * Displays appropriate error note depending on error code.
        * @param aError: error code returned by DRM engine
        * @param aFileName: full path to file
        * @return 
        * - Button code, if user selected "buy rights" or "launch rights 
        *   manager" in query.
        * - 0 if user selected "No", "Back" or "Cancel" (or whatever which 
        *   just dismiss the query)
        * - 0 if just a simple error note is displayed
        */
        virtual TInt HandleErrorL( TInt aError, 
           const TDesC& aFileName ) = 0;

        /**
        * Handle the general UI based error notes 
        * @param aTextId The code for DRM general notification note.
        * @param aURI The DRM files URI, that caused the DRM error.
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt ShowDRMUINotificationL( TNotificationID aTextId,
            const TDesC8& aURI ) = 0;

        /**
        * Handle the general UI based error notes 
        * @param aTextId The code for DRM general notification note.
        * @param aFileName The DRM file's name and path, that caused the DRM error.
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt ShowDRMUINotificationL( TNotificationID aTextId,
            const TDesC& aFileName ) = 0;

        /**
        * Set count limit for CheckRightsAmountL
        * @param aCounts: new count limit for displaying the note.
        * @return -
        */
        virtual void SetCountLimitL( TUint aCounts ) = 0;
        
        /**
        * Set time limit in days for CheckRightsAmountL
        * @param aDays: new time limit for displaying the note.
        * @return -
        */
        virtual void SetTimeLimitL( TUint aDays ) = 0;

        /**
        * Set percentage limit for CheckRightsAmountL. Applies to both counts and time.
        * @param aPercentage: new limit for displaying the note.
        * @return -
        */
        virtual void SetPercentageLimitL( TUint aPercentage ) = 0;

        /**
        * Launches DRM Rights Manager UI embedded with details view related 
        * to given content.
        * @param aURI: URI of the file
        * @return -
        */
        virtual void LaunchDetailsViewEmbeddedL( 
            const TDesC8& aURI ) = 0;
        
        /**
        * Launches DRM Rights Manager UI embedded with details view related 
        * to given file.
        * @param aFileName: Full path to file
        * @return -
        */      
        virtual void LaunchDetailsViewEmbeddedL( 
            const TDesC& aFileName ) = 0;

        /**
        * Gets details of rights object associated of given file.
        * @param aFileName: Full path to file
        * @param aIntent: bitmask of DRMCommon::EPlay, DRMCommon::EDisplay, 
        *                 DRMCommon::EExecute and DRMCommon::EPrint
        * @param aExpired: (out) ETrue if rights are expired
        * @param aSendingAllowed: (out) ETrue if content can be sent
        * @param aPlay: (out) play constraints, must be deleted by caller
        * @param aDisplay: (out) display constraints, must be deleted by caller
        * @param aExecute: (out) execute constraints, must be deleted by caller
        * @param aPrint: (out) print constraints, must be deleted by caller
        * @return -
        */
        virtual void GetRightsDetailsL( const TDesC& aFileName, 
            TUint32 aIntent, TBool& aExpired, TBool& aSendingAllowed, 
            CDRMRightsConstraints*& aPlay, 
            CDRMRightsConstraints*& aDisplay, 
            CDRMRightsConstraints*& aExecute, 
            CDRMRightsConstraints*& aPrint ) = 0; 

        /**
        * Checks whether some content can be used as automated content or not.
        * @param aURI: URI of the file
        * @param aValue: (out) result of the query
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt CanSetAutomated( const TDesC8& aURI, TBool& aValue ) = 0;
        
        /**
        * Checks whether some content can be used as automated content or not.
        * @param aFileName: Full path to the file
        * @param aValue: (out) result of the query
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt CanSetAutomated( const TDesC& aFilename, TBool& aValue ) = 0;
        
        /**
        * Registers some content for automated usage (e.g. default ring tone).
        * @param aURI: URI of the file
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt SetAutomated( const TDesC8& aURI ) = 0;
        
        /**
        * Registers some content for automated usage (e.g. default ring tone).
        * @param aFileName: Full path to the file
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt SetAutomated( const TDesC& aFilename ) = 0;
        
        /**
        * Removes automated content. Used to indicated that the given content is 
        * not anymore used automaticallly by the device.
        * @param aURI: URI of the file
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt RemoveAutomated( const TDesC8& aURI ) = 0;
        
        /**
        * Removes automated content. Used to indicated that the given content is 
        * not anymore used automaticallly by the device.
        * @param aFileName: Full path to the file
        * @return KErrnone if everything went ok, else wide scale of Symbian errors
        */
        virtual TInt RemoveAutomated( const TDesC& aFilename ) = 0;

        /**
        * Sends an idle-signal to DRM Helper Server. This indicates 
        * that the device is on idle state to the DRM Helper Server.
        * @return -
        */
        virtual void IndicateIdle() = 0;

        /**
        * GetContentURIList
        *
        * Returns a list of all content URIs that have rights in the rights 
        * database.
        *
        * @since  2.5
        * @param  aURIList: Out parameter for the URI list.
        * @return Ok if the URI list could be retreived.
        */
        virtual TInt GetContentURIList(
            RPointerArray<HBufC8>*& aURIList) = 0;
    
        /**
        * DataTypesCount
        *
        * Returns the number of previously registered datatypes which are
        * supported by the DRM system and associated applications.
        *
        * @since  2.0
        * @param  aCount: Out parameter for the datatype count
        * @return EOk if the number of datatypes has been returned correctly
        */
        virtual TInt DataTypesCount(
            TInt& aCount) = 0;
    
        /**
        * SupportedDataType
        *
        * Returns a specific datatype which has been registered before by giving an index
        *
        * @since  2.0
        * @param  aIndex: Number of the datatype to return
        * @param  aDataType: Out parameter for the datatype
        * @return EOk if the datatype was returned correctly
        */
    
        virtual TInt SupportedDataType(
            const TInt aIndex, TDataType& aDataType) = 0;
    
        /**
        * RegisterDataType
        *
        * Registers a datatype as a supported datatype.
        *
        * @since  2.0
        * @param  aDataType: Datatype to register
        * @return EOk if the datatype has been registered
        */
    
        virtual TInt RegisterDataType(
            const TDataType& aDataType) = 0;
    
        /**
        * UnRegisterDataType
        *
        * Unregisters a datatype via its index.
        *
        * @since  2.5
        * @param  aIndex: Number of the datatype to unregister
        * @return EOk if the datatype has been unregistered
        */
    
        virtual TInt UnRegisterDataType(
            const TInt aIndex) = 0;
    
        /**
        * SupportedDRMMethods
        *
        * Returns the supported DRM protection methods and the OMA compliance
        * level of the DRM engine
        *
        * @since  2.0
        * @param  aDRMMethod: Out parameter, bit mask containing any 
        *         combination of EForwardLock, ECombinedDelivery, 
        *         ESeparateDelivery etc.
        * @param  aOMALevel: OMA compliance of the DRM engine
        * @return DRMCommon error code
        */
    
        virtual TInt SupportedDRMMethods(
            TInt& aDRMMethod, TOMALevel& aOMALevel) = 0;

        /**
        * Consume
        *
        * Overrides any rights consumption. This function must only be used by
        * applications with user interaction and not by intermediate components.
        * The main use cases are signalling that content is to be installed
        * (including thumbnail generation), or that content is to be consumed.
        * They are characterized as calls to this function with the correct
        * intent (i.e. EPlay or EInstall) and the corresponding action (i.e.
        * telling whether the indicated intent is started or finished).
        * Calls to this function must be the result of a user interaction or
        * otherwise high level event.
        *
        * @since  2.5
        * @param  aContentURI: URI of the content which will be consumed
        * @param  aRightsSpec: the consumption intent (EPlay, EDisplay, EPrint,
        *         EExecute or EInstall)
        * @param  anAction: the consumption action, telling whether the
        *         content consumption starts or finishes.
        * @return EOk if consume action was successfully signalled to the engine.
        */
        virtual TInt Consume(
            const TDesC8& aContentURI,
            TUint32 aRightsSpec,
            TConsumeAction anAction) = 0;
    
        /**
        * ConsumeFile
        *
        * Overrides any rights consumption. This function must only be used by
        * applications with user interaction and not by intermediate components.
        * The main use cases are signalling that content is to be installed
        * (including thumbnail generation), or that content is to be consumed.
        * They are characterized as calls to this function with the correct
        * intent (i.e. EPlay or EInstall) and the corresponding action (i.e.
        * telling whether the indicated intent is started or finished).
        * Calls to this function must be the result of a user interaction or
        * otherwise high level event.
        *
        * @since  2.5
        * @param  
        * @param  aRightsSpec: the consumption intent (EPlay, EDisplay, EPrint,
        *         EExecute or EInstall)
        * @param  anAction: the consumption action, telling whether the
        *         content consumption starts or finishes.
        * @return EOk if consume action was successfully signalled to the engine.
        */
        virtual TInt ConsumeFile(
            const TDesC& aFileName,
            TUint32 aRightsSpec,
            TConsumeAction anAction) = 0;
    
    };

class MDRMHelperFactory
    {
public:

    IMPORT_C static MDRMHelper* CreateL(CCoeEnv& aCoeEnv);
    IMPORT_C static MDRMHelper* CreateLC(CCoeEnv& aCoeEnv);

    IMPORT_C static MDRMHelper* CreateL();
    IMPORT_C static MDRMHelper* CreateLC();

    };

#endif      // MDRMHELPER_H   
            
// End of File
