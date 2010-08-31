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
*
*/
#ifndef   DRMUTILITYCONNECTION_H
#define   DRMUTILITYCONNECTION_H

#include <e32base.h> // CActive
#include <es_sock.h> // RConnection, RSocketServ

namespace DRM
{

 /**
  *  Utility class for DRMutility dmgr wrapper  related common functions
  *
  *  @since S60 v9.2
  */


// CLASS DECLARATION
// Class for handling internal connectivity logic
NONSHARABLE_CLASS( CDrmUtilityConnection ): public CActive
    {
public:     // Constructors and destructor.

    /**
    *
    */
    static CDrmUtilityConnection * NewL( const TBool aAllowQueries );

    /**
    *
    */
    static CDrmUtilityConnection * NewLC( const TBool aAllowQueries );

    /**
    * Destructor
    */
    ~CDrmUtilityConnection();

public:     // new methods

    /**
    *
    */
    void ConnectL( TRequestStatus* aStatus );

    /**
    *
    */
    void Close();

    /**
    *
    */
    TBool HasMoreConnectionAttempts();

    /**
    *
    */
    TBool IsConnected( TUint32& aIap );

    /**
    * Check if default connection has been defined
    * @see RCmManager::ReadDefConnL
    */
    static TBool HasDefaultConnectionL();

    /**
    * Check if system has access points defined
    */
    static TBool HasAccessPointsL();



private: //enumerations
    enum TConnectionType
        {
        EAttachExisting, //0
        EUsingDefault, //1
        EUsingPurposeOperator, // 2
        EUsingPurposeInternet, // 3
        EUsingQuery, // 4
        EFail // 5
        };

    enum TState
        {
        EInit, // 0
        EConnecting, // 1
        EConnected, // 2
        EConnectFailed // 3
        };

private:    // Constructors and destructor.

    /**
    * Default constructor (for preventing inheritance)
    */
    CDrmUtilityConnection( TBool aAllowQueries );

    /**
    * Second phase constructor
    */
    void ConstructL();

private:  // from CActive

    virtual void DoCancel();

    virtual void RunL();

    virtual TInt RunError( TInt aError );

private:    // new methods

    /**
    *
    */
    void DoClose();

    /**
    *
    */
    void Done();

    /**
    *
    */
    void InitL();

    /**
    *
    */
    void AttachExistingConnectionL();

    /**
    *
    */
    void ConnectUsingDefaultL();

    /**
    *
    */
    void ConnectUsingSnapPurposeL( const TUint32 aPurpose );

    /**
    *
    */
    void ConnectUsingQueryL();

    /**
    *
    */
    void UpdateIapIdL();

    /**
    *
    */
    void CompleteSelf( TInt aError );

    /**
    *
    */
    void CompleteParent( TInt aError );

private:    // data

    RSocketServ iSocketServ;
    RConnection iConnection;
    TState iState;
    TConnectionType iType;
    TRequestStatus* iParentStatus;
    const TBool iAllowQueries;
    TUint32 iIapId;
    };
}
#endif /* DRMUTILITYCONNECTION_H */
