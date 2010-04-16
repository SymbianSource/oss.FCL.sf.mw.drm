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
* Description:  Implementation of Rights Object MTM UI Data
*
*/


#ifndef RIGHTSOBJECTMTMUIDATA_H
#define RIGHTSOBJECTMTMUIDATA_H

//  INCLUDES
#if !defined(__MSVSTD_H__)
#include <MSVSTD.H>
#endif

#if !defined(__MTUDBAS_H__)
#include <mtudcbas.h>
#endif

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRightsObjectMtmUiDataSessionObserver;
class CMsvSession;

// CLASS DECLARATION
/**
*   CRightsObjectMtmUiDataSessionObserver
*
*   CMsvSession Observer
*
*  @lib RoMtmDat.lib
*  @since 2.5
*/
class CRightsObjectMtmUiDataSessionObserver : public CBase, public MMsvSessionObserver
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CRightsObjectMtmUiDataSessionObserver* NewL();

        /**
        * Destructor.
        */
        ~CRightsObjectMtmUiDataSessionObserver();

        /**
        * From MMsvSessionObserver
        */
        void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

    private:
        CRightsObjectMtmUiDataSessionObserver();
        void ConstructL();
    };


// CLASS DECLARATION
/**
*   CRightsObjectMtmUiData
*
*  @lib RoMtmDat.lib
*  @since 2.5
*/
class CRightsObjectMtmUiData : public CBaseMtmUiData
    {
    public:
    // Construction, initialisation, and destruction

        /**
        * Two-phased constructor.
        */
        static CRightsObjectMtmUiData* NewL( CRegisteredMtmDll& aRegisteredDll );

        /**
        * Destructor.
        */
        ~CRightsObjectMtmUiData();

        /*************************************
        *
        *       from CBaseMtm
        *
        **************************************/

        // Context-specific information

        /**
        * CanEditEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanEditEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanViewEntryL
        * @since 2.5
        *
        * @param aContext  The entry to which the operation applies.
        * @param aReasonResourceId  On return, a resource string ID or 0.
        *
        * @return KErrNone: the operation is appropriate on the entry. KErrNotSupported: the operation is not appropriate on the entry.
        */
        TBool CanViewEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanOpenEntryL
        * @since 2.5
        *
        * @param aContext  The entry to which the operation applies.
        * @param aReasonResourceId  On return, a resource string ID or 0.
        *
        * @return KErrNone: the operation is appropriate on the entry. KErrNotSupported: the operation is not appropriate on the entry.
        */
        TBool CanOpenEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanCloseEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanCloseEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanDeleteFromEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanDeleteFromEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanCopyMoveToEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanCopyMoveToEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanCopyMoveFromEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanCopyMoveFromEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanReplyToEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanReplyToEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanForwardEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanForwardEntryL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

        /**
        * CanCreateEntryL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanCreateEntryL( const TMsvEntry& aContext , TMsvEntry& aNewEntry , TInt& aReasonResourceId ) const;

        /**
        * CanDeleteServiceL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanDeleteServiceL( const TMsvEntry& aService , TInt& aReasonResourceId ) const;

        /**
        * CanCancelL
        * @since 2.5
        *
        * Not supported
        *
        */
        TBool CanCancelL( const TMsvEntry& aContext , TInt& aReasonResourceId ) const;

    // Context-specific icons

        /**
        * ContextIcon
        * @since 2.5
        *
        * @param aContext  Context entry to return the icons for.
        * @param aStateFlags  Flasgs indicating the user interface state of the entry.
        *
        * @return Bitmap array for the given context..
        */
        const CBitmapArray& ContextIcon( const TMsvEntry& aContext , TInt aStateFlags ) const;

        /**
        * GetResourceFileName
        * @since 2.5
        *
        * @param aFileName Filename buffer to be filled with the resource file path and name
        */
        void GetResourceFileName( TFileName& aFileName ) const;

        /**
        * PopulateArraysL
        * @since 2.5
        *
        */
        void PopulateArraysL();


        /**
        * StatusTextL
        * @since 2.5
        *
        * Not supported
        *
        */
        HBufC* StatusTextL( const TMsvEntry& aContext ) const;


    // MTM-specific functionality

        /**
        * OperationSupportedL
        * @since 2.5
        *
        * Not supported
        *
        */
        TInt OperationSupportedL(
            TInt aOperationId ,
            const TMsvEntry& aContext ) const;

        /**
        * QueryCapability
        * @since 2.5
        *
        * @param aCapability UID of capability to be queried
        * @param aResponse Response value. The format of the response varies
        * according to the capability
        *
        * @return KErrNone if aCapability is a recognised value and a response is
        * returned KErrNotSupported if aCapability is not a recognised value
        */
        TInt QueryCapability( TUid aCapability , TInt& aResponse ) const;


    protected:
        /**
        * Creates skinned icons. Fallbacks are normal icons
        * @since Series60 2.8
        * @param aNumZoomStates: amount of elements in vector
        *        ( bitmap & bitmapmask )
        */
        void CreateSkinnedBitmapsL( TInt aNumZoomStates );

    private:
        void ConstructL();

        CRightsObjectMtmUiData( CRegisteredMtmDll& aRegisteredDll );

        void SetDescriptionL( const TMsvEntry& aContext ) const;


    public:     // Data
    protected:  // Data
    private:    // Data

        CRightsObjectMtmUiDataSessionObserver*  iObserver;
        CMsvSession*                            iSession;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // RIGHTSOBJECTMTMUIDATA_H

// End of File
