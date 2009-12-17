/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for the consume operations. This class handles all
*                operations regarding rights consumption in DRM5.
*
*/

#ifndef DRMCONSUME_H
#define DRMCONSUME_H

// INCLUDES
#include <e32base.h>
#include <caf/caftypes.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FORWARD DECLARATIONS
class CDRMDbSession;
class RMessage2;
class CDRMPermission;
class CDRMConstraint;

// TYPE DEFINITIONS
// FUNCTION PROTOTYPES

// CLASS DECLARATION
/**
*  CDRMConsume acts as content consumer in DRM5 Rights Server. It is accessed
*  by CDRMDbSession. Content consumption is initiated via HandleL() method
*  in the beginning of the consumption, and later on the class can be
*  controlled through Pause(), Continue() and Stop(). Destructor calls
*  the Cancel() method.
*  
*  @lib RightsServer.exe
*  @since 3.0
*/
NONSHARABLE_CLASS( CDRMConsume ) : public CTimer
    {

    public: // Constructors and destructor.
        /**
        * NewLC
        *
        * Two-phase constructor.
        *
        * @since    3.0
        * @param    aParent Callback handle to the session
        *                   owning this instance.
        * @param    aURI    Content-ID.
        * @return   A pointer to newly created object.
        */
        static CDRMConsume* NewLC( CDRMDbSession& aParent, 
                                   const TDesC8& aURI,
                                   const TDesC8* aParentId );
        /**
        * Destructor.
        */
        virtual ~CDRMConsume();
        
    public: // New functions.
        /**
        * HandleL
        *
        * Starts the content consumption.
        *   
        * Whenever this method is called, rights database also gets
        * updated if related permissions are stateful. When calling HandleL
        * several times for the same class instance instance, the permissions 
        * are consumed only once if the related permissions returned by 
        * CDRMDbSession::FindRightsObjectL() are the same returned earlier.
        *
        * @since    3.0
        * @param    aIntent Usage intent.        
        */
        void HandleL( ContentAccess::TIntent aIntent );
        
        /**
        * Pause
        *
        * Pauses the content consumption.
        *
        * Content consuption can be resumed via ContinueL() and HandleL(). 
        * See comments in HandleL() method for information about permission 
        * consumption.
        *
        * @since    3.0
        */
        void Pause();
        
        /**
        * ContinueL
        *
        * Continues the content consumption.
        *
        * This method works similarily to HandleL.
        *
        * @since    3.0
        */
        void ContinueL();
        
        /**
        * Stop
        *
        * Stops the content consumption.
        *
        * This method works similarily to Pause()
        *
        * @since    3.0 
        */
        void Stop();
        
        /**
        * CountConstraintActive
        *
        * Checks whether a count constraint was used earlier
        *
        * @since    3.0 
        * @return   ETrue if a count constraint was used earlier,
        *           EFalse otherwise
        */
        TBool CountConstraintActive();
        
        /**
        * IsExpired
        *
        * Returns ETrue if the last consumption lead to expired rights
        *
        * @since    3.0 
        * @return   ETrue if the rights are expired,
        *           EFalse otherwise
        */
        TBool IsExpired();
        
        /**
        * Returns the current child permission
        *
        * @since    3.0 
        * @return   the permission
        */
        CDRMPermission& GetChild();

    protected: // from base class CActive
        /**
        * RunL
        * 
        * Handles the completed asynchronous request.
        */
        void RunL();
        
        /**
        * RunError
        *
        * Handles errors happened during RunL().
        *
        * At the moment this method does nothing.
        * 
        * @since    3.0
        * @param    Error code
        * @return   KErrNone if the error could be handled. Otherwise an
        *           error code is returned.
        */
        TInt RunError( TInt aError );
        
    protected: // New Functions
        /**
        * Constructor
        * 
        * Constructs the class instance.
        * 
        * @since    3.0
        * @param    aSession Callback session to the CDRMDbSession owning
        *           this instance.
        */
        CDRMConsume( CDRMDbSession& aSession );
        
        /**
        * Second phase constructor
        * 
        * Finalizes the construction of the class instance.
        *
        * @since    3.0
        * @param    aURI Content-ID
        */
        void ConstructL( const TDesC8& aURI,
                         const TDesC8* aParentId );
        
    private: // From base class CActive
        /**
        * DoCancel
        *
        * Handles the cancelling operations.
        * This method updates timed constraints of the permission found
        * earlier (if such constraints exist). Calls DoCancelL() in trap
        * harness.
        *
        * @since    3.0
        */
        void DoCancel(); 
        
    private: // New functions
        /**
        * InitializeL
        * 
        * This method finds the proper permissions (child & parent) for the 
        * given intent and previously set content-ID.
        *
        * @since    3.0
        * @param    aIntent Consumption intent
        * @param    aSecureTime Whether the given time is secure or not
        * @param    aTrustedTime Current time.
        */
        void InitializeL( ContentAccess::TIntent aIntent,
                          TBool aSecureTime,
                          const TTime& aTrustedTime );
        
        /**
        * ActivateL
        *
        * In case of time based or interval based constraints, this method 
        * activates the internal timer to monitor delay.
        *
        * @since    3.0
        * @aParam   aSecureTime Whether the given time is secure or not.
        * @aParam   aTrustedTime Current time.
        */
        void ActivateL( TBool aSecureTime,
                        const TTime& aTrustedTime );

        /**
        * Consume
        *
        * This method consumes the internally stored permissions according to
        * the given parameters. If a(ny) internally stored permission is
        * changed, the internal status masks iTimedCounts and iTimedCounts.
        *
        * @since    3.0
        * @param    aUpdateCounter Whether to consume counter constraint.
        * @param    aInitInterval  Whether to initialize (possibly) 
        *                          uninialized interval.
        * @param    aUpdateTimedCount Whether to update timed counter.
        * @param    aElapsedTime How much time has elapsed since initialization
        * @param    aSecureTime Whether the given time is secure or not.
        * @param    aTrustedTime Current time.
        */
        void Consume( TBool aUpdateCounter,
                      TBool aInitInterval,
                      TBool aUpdateTimedCount,
                      const TTimeIntervalSeconds& aElapsedTime,
                      TBool aSecureTime,
                      const TTime& aTrustedTime );
        
        /**
        * CombinePermissionsL
        * 
        * Combines internal iParent and iChild into iCombined.
        *
        * @since    3.0
        */
        void CombinePermissionsL();
        
        /**
        * UpdateDBL
        *
        * Commits the internal iChild & iParent to the database in case
        * they have changed.
        */
        void UpdateDBL();
        
        /**
        * ConsumeConstraint
        * 
        * Consumes the given constraint by the given parameters.
        * 
        * @since    3.0
        @ @param    aConstraint Constraint to be consumed.
        * @param    aUpdateCounter Whether to decrease the counter constraint.
        * @param    aInitInterval Whether to initialize (possibly)
        *                         uninitialized interval.
        * @param    aUpdateTimedCount Whether to decrease timed counter.
        * @param    aElapsedTime How much time has elapsed since initialization
        * @param    aSecureTime Whether the given time is secure or not.
        * @param    aCumulativeTime The time that has elapsed cumulatively 
        *                           from the start of a timed counter
        * @param    aTrustedTime Current time.
        */
        TBool ConsumeConstraint( CDRMConstraint& aConstraint,
                                 TBool aUpdateCounter,
                                 TBool aInitInterval,
                                 TBool aUpdateTimedCount,
                                 const TTimeIntervalSeconds& aElapsedTime,
                                 TBool aSecureTime,
                                 const TTime& aTrustedTime,
                                 TTimeIntervalSeconds& aCumulativeTime );
        
        /**
        * ConsumeTimedItemsL
        * 
        * Consumes timed items.
        *
        * @since    3.0
        * @param    aDelay The delay between timer initialization and this 
        *                  moment.
        * @param    aSecureTime Whether the given time is secure or not.
        * @param    aTrustedTime Current time.
        */
        void ConsumeTimedItemsL( TTimeIntervalSeconds aDelay,
                                 TBool aSecureTime,
                                 const TTime& aTrustedTime );
        
        /**
        * DoContinueL
        * 
        * Handles the operations of RunL() method when timer goes off.
        * 
        * @since    3.0
        */        
        void DoContinueL();
        
        /** 
        * SecurityLevelChanged
        * 
        * Checks whether the secure time is now insecure or vice versa, 
        * compared to the situation when ActivateL() was called.
        *
        * @since    3.0
        * @param    aSecureTime Current time security level.
        * @return   Whether the security level of the time is different.
        */
        TBool SecurityLevelChanged( TBool aSecureTime ) const;
        
        /**
        * DoCancelL
        *
        * Updates time based constraints of internally stored permissions
        * in case of cancel operation.
        * 
        * @since    3.0
        */
        void DoCancelL();
        
        /**
        *
        * Updates the metering count, defines the metering accumulated time
        * and updates the metering database if there was metered content. 
        * 
        * @since S60 3.2
        */
        void UpdateMeteringDbL();
        
        /**
        *
        * Creates a Rights Issuer Context to Roap storage and checks the rights 
        * of the Rights Issuer to use metering.
        *
        * @since S60 3.2
        * @param aRiId
        * @return Whether metering can be allowed base on the Rights Issuer Id or not
        */
        TBool CheckRiContextRightsL( const TDesC8& aRiId );
        
         
    private: // New functions.
        /**
        * Default constructor.
        * 
        * Prohibited.
        */
        CDRMConsume(); 
    
    public:     // Data
    protected:  // Data
    private:    // Data
        // Callback handle.
        CDRMDbSession& iSession;
        
        // Usage intent when the consumption was initialized.
        ContentAccess::TIntent iIntent;
        
        // Content-id
        HBufC8* iURI;
        
        // id of the parent, if this exists we are using a parent RO
        HBufC8* iParentId;
        
        // The child permission used in the consumption operation.
        CDRMPermission* iChild;
        
        // A constraint combined from iChild & iParent.
        CDRMConstraint* iCombined;
        
        // The time when CTimer went active.
        TTime iTime;
        
        // Tells how much the waiting time was when initialized.
        TInt16 iCurrentDelay;
        
        // Internal state.
        TUint8 iMask;
        
        // Internal state to check whether timed counters have been modified.
        TUint8 iTimedCounts;
        
        // Internal state to check whether counters have been modified.
        TUint8 iCounters;
        
        // Set to ETrue if a count constraint was used
        TBool iCountConstraintActive;
        
        // Set to ETrue if the last consumption lead to expired rights
        TBool iExpired;
        
        // Tells how much time has been actually spent rendering
        TTimeIntervalSeconds iCumulativeDelayTop;
        TTimeIntervalSeconds iCumulativeDelayChild;
        
        //  Tells the total cumulative time. It is needed when calculating
        //  accumulated time for metering purposes. 
        TTimeIntervalSeconds iTotalCumulativeTime;
        
        // Bit to tell if we use timed count
        TBool iUsingTimedCount;      
    };

#endif // DRMCONSUME_H

// EOF
