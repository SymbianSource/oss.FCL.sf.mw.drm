/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of Rights Object Client side MTM
*
*/



#ifndef RIGHTSOBJECTMTMCLIENT_H
#define RIGHTSOBJECTMTMCLIENT_H

//  INCLUDES
#include <e32base.h>
#include <mtclbase.h>
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRoMessageHeader;

// CLASS DECLARATION
/**
*	CRightsObjectMtmClient
*  
*  @lib RoMtmCli.lib
*  @since 2.5
*/
class CRightsObjectMtmClient : public CBaseMtm
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRightsObjectMtmClient* NewL(
					CRegisteredMtmDll& aRegisteredMtmDll , 
					CMsvSession& aMsvSession );
        
		/**
        * Destructor.
        */
         ~CRightsObjectMtmClient();

   	public: // Functions from base classes

		/*************************************
		*
		*		from CBaseMtm
		*
		**************************************/


		// Store and restore entry data 
		/**
        * SaveMessageL
        * @since 2.5
        */
		void SaveMessageL(); 

		/**
        * LoadMessageL
        * @since 2.5
        */
		void LoadMessageL();

		// Response message creation 

		/**
        * ReplyL
        * @since 2.5
        * 
		* Not supported
		*
        */	
		CMsvOperation* ReplyL  ( TMsvId aReplyEntryId , TMsvPartList aPartlist , TRequestStatus& aCompletionStatus );

		/**
        * ForwardL
        * @since 2.5
        * 
		* Not supported
		*
        */
		CMsvOperation* ForwardL( TMsvId aForwardEntryId , TMsvPartList aPartList , TRequestStatus& aCompletionStatus );

		// Validation and search 
		
		/**
        * ValidateMessage
        * @since 2.5
        * 
		* Not supported
		*
        */	
		TMsvPartList ValidateMessage( TUint aPartList );
		
		/**
        * Find
        * @since 2.5
        * 
		* Not supported
		*
        */			
		TMsvPartList Find( const TDesC& aTextToFind , TMsvPartList aPartList );

		// Addresses are not used in this MTM
		
		/**
        * AddAddresseeL
        * @since 2.5
        * 
		* Not supported
		*
        */			
		void AddAddresseeL( const TDesC& aRealAddress );
		
		/**
        * AddAddresseeL
        * @since 2.5
        * 
		* Not supported
		*
        */			
		void AddAddresseeL( const TDesC& aRealAddress , const TDesC& aAlias );
		
		/**
        * RemoveAddressee
        * @since 2.5
        * 
		* Not supported
		*
        */			
		void RemoveAddressee( TInt aIndex );

		// MTM-specific functionality

		/**
        * QueryCapability
        * @since 2.5
        * 
		* @param aCapability UID of capability to be queried 
		* @param aResponse Response value. The format of the response varies according to the capability
		*
		* @return KErrNone: aFunctionId is a recognised value and a response is returned KErrNotSupported: aFunctionId is not a recognised value 
        */	
		TInt QueryCapability( TUid aCapability , TInt& aResponse );

		/**
        * InvokeSyncFunctionL
        * @since 2.5
        * 
		* Not supported
		*
        */	
		void InvokeSyncFunctionL( TInt aFunctionId , const CMsvEntrySelection& aSelection , TDes8& aParameter );
		
		/**
        * InvokeAsyncFunctionL
        * @since 2.5
        * 
		* Not supported
		*
        */			
		CMsvOperation* InvokeAsyncFunctionL( TInt aFunctionId , const CMsvEntrySelection& aSelection , TDes8& aParameter , TRequestStatus& aCompletionStatus );
		
	protected:  

		CRightsObjectMtmClient( CRegisteredMtmDll& aRegisteredMtmDll , CMsvSession& aMsvSession );

		void ConstructL();        

		/*************************************
		*
		*		from CBaseMtm
		*
		**************************************/
		void ContextEntrySwitched(); 

    protected:  // Functions from base classes
    private:
//		void SetEntryDetailsL();
//		void SetEntryDescriptionL();
	public:     // Data
    protected:  // Data
    private:    // Data
//		CRoMessageHeader* iHeader;
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };



#endif      // RIGHTSOBJECTMTMCLIENT_H
            
// End of File
