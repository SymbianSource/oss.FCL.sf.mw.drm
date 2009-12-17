/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef ROAP_ENG_BASE_H
#define ROAP_ENG_BASE_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <apmrec.h>
#include <apparc.h>

class CAiwGenericParamList;
class CDRMRights;
class CDRMRIContext;

namespace Roap
{
// FORWARD DECLARATION

class CRoapTrigger;
class CRoapHttpHandler;
class CRoapResponse;
class CRoapMessage;
class MRoapObserver;

// CLASS DECLARATION

enum TTriggerType
    {
    ERegistrationTrigger,
    ERoAcquisitionTrigger,
    EJoinDomainTrigger,
    ELeaveDomainTrigger,
    EExtendedTrigger,
    EMeteringReportTrigger
    };
    
enum TRiContextStatus
    {
    EValidContext,
    EInvalidContext,
    EInvalidContextNoConf    
    };
    
enum TDomainOperation
    {
    ENotDomainOperation,
    EJoinDomainOperation,
    EKeyUpgradeOperation
    };
    

class CRoapEngBase: public CActive
    {
    public:

        enum TRoapState         
            {
            EInit,                           
            EReady,                 
            ECreateConn,           
            EStartTrans,
            ERespReceived,           
            EComplete           
            };
            
		enum TRequestType
			{
			EDeviceHello,
			ERegistration,          
			EROAcquisition,
			EJoinDomain,
			ELeaveDomain,
			EMeteringRequest
			};


    public: // destructor
     
        IMPORT_C virtual ~CRoapEngBase();

    public:
    
        /**
        * SetTriggerL
        * 
        * The function takes in a ROAP Trigger and parses it. The the internal state
        * of ROAP Engine is changed accordingly. After the trigger is set a caller
        * can either accept the ROAP transaction by calling AcceptL or reject by calling
        * Reject. 
        * 
        * @since  3.0
        * @param  aXmlTrigger:    An ROAP Trigger in XML format
        *
        * @param  aParamList:     Generic paramaters from application that lauched the ROAP UI
        *                         Can contain e.g. access point etc.
        *
        * @param  aType:          On return, defines the type of the parsed trigger
        *
        * @param  aContextStatus: On return, indicates if the Device has stored a valid
        *                         RI context or not
        *
        * @param  aDomainOper:    On return, in the case of Join Domain protocol indicates
        *                         the intended Domain operation. Join Domain or Key upgrade.                             
        *
        * @param  aContentIdList: Out parameter. ContentIDs that are related to  the
        *                         RO acquisition. Can be used e.g. for opening the
        *                         content file after the ROAP has succesfully finished.
        *                         The elements inside the array must be deleted by the
        *                         caller
        *
        * @leave  System wide error code
        */
        
        IMPORT_C void SetTriggerL( const TDesC8& aXmlTrigger,
                                   const CAiwGenericParamList* aParamList,        
                                   TTriggerType& aType,
                                   TRiContextStatus& aContextStatus,
                                   TDomainOperation& aDomainOperation,
                                   RPointerArray<HBufC8>& aContentIdList );
        /**
        * AcceptL
        *
        * The function starts the ROAP operation. SetTriggerL must be called successfully 
        * before calling this function. 
        * 
        * @since  3.0
        * @param  aObserver: An observer object. Can be NULL if the observer is not needed.
        *
        * @param  aStatus:   Indicates the completion status of ROAP. Contains an error code or
        *                    KErrNone is everything worked ok.
        *
        * @leave  System wide error code
        */        

        IMPORT_C void AcceptL( MRoapObserver* aObserver, TRequestStatus* aStatus );
        
        /**
        * Reject
        *
        * The function cancels the ROAP operation. The initial state (as it was before calling
        * SetTriggerL) is restored. The function handles cleanup of stored data internally
        * (by calling the DoCleanupL method)
        * 
        * @since  3.0
        */        

        IMPORT_C void Reject();
        
        /**
        * DoCleanup
        *
        * The function deletes the data that has been stored during the last ROAP processing.
        * The asynchronous AcceptL call must be finished before it is useful to call this method.
        * 
        * @since  3.0
        *
        * @return System wide error code
        */    
        
        IMPORT_C TInt DoCleanup();   
        
        /**
        * HandleRoReponseL
        * 
        * The function is for handling the 1-pass RO acquisition case. Takes in RO response
        * PDU and parses it and stores the Rights Object into the database.
        *
        * @since  3.0
        * @param  aResponse:     RO acquisition response PDU in XML format.
        * @param  aParsedRights: Out parameter. Contains pointers to stored ROs. The array
        *                        contents are owned by a caller. If the function leaves
        *                        the caller must cleanup and close aParsedRights
        *
        * @leave  System wide error code
        */        
        
        IMPORT_C void HandleRoReponseL( const TDesC8& aResponse, RPointerArray<CDRMRights>& aParsedRights );  
        

        /**
        * Trigger
        * 
        * The function returns a reference to the parsed CRoapTrigger object. The SetTriggerL
        * method must be called successfully before this method can be called. 
        *
        * @since  3.0
        *
        * @return Parsed CRoapTrigger obejct. 
        */ 

        IMPORT_C const CRoapTrigger& Trigger() const;
            
    protected:  // Constructors
      
        CRoapEngBase();
        
        void ConstructL();
            
        
    protected:  // from CActive

        virtual void DoCancel();

        virtual void RunL();

        virtual TInt RunError( TInt aError );


    protected:

        void SetL();

        void CreateConnectionL();

        void StartTransactionL();

        void HandleResponseL();
      
        void SetProtocolL();
        
        void CompleteL();
        
        void CompleteRequest();
        
        void Continue( TRoapState aNextState );


    protected:

        virtual CRoapTrigger* ParseTriggerL( const TDesC8& aXmlTrigger ) = 0;

        virtual void GetRIContextL( TBool& aRegistered, const TDesC8& aRiId ) = 0;
        
        virtual void  GetDomainContextL( TBool& aIsJoined,
                                         TBool& aIsValidGeneration,
                                         const TDesC8& aDomainId ) = 0;

        virtual void CreateReqMessageL() = 0;
        
        virtual	void HandleRoapResponseL( const TDesC8& aXmlResponse ) = 0;
        
        virtual void HandleRightsResponsePduL( const TDesC8& aRightsResp,  TBool aOnePass ) = 0;
        
        virtual	void HandleMultipartL() = 0;
        
        virtual HBufC8* SignMessageL( const TDesC8& aMessage ) const = 0;
        
        virtual TInt MapStatusL() = 0;


    protected:

        CRoapTrigger* iTrigger;
        CRoapMessage* iRequest;
        CRoapMessage* iResponse;
        CRoapHttpHandler* iHttpHandler;
        CRoapResponse* iRoapResp;
        CDRMRIContext* iStoredRiContext;
        TRoapState iState;
        TInt iStatusCode;
        TInt iResult;
        TRequestStatus* iParentStatus;
        TUint32 iPreferredIap;
        TRequestType iReqMessage;
        TBool iCompleted;
        MRoapObserver* iObserver;
        TBool iTransTrackConf;
        RPointerArray<CDRMRights> iReturnedROs;
        TBool iAllowedToContactRi;
        TBool iUseRiContextUrl;
        TBool iImplicitJoinDomain;
        HBufC8* iDomainRightsResp; // for the implicit Join Domain
        TInt iProgressInBytes;
        TBool iReRegistered;
        HBufC8* iRiAlias;
    };
}

#endif
