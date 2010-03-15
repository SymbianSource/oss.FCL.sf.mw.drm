/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides operations for ui usage of DRM Utility API
*
*/


#ifndef CDRMUTILITYUI_H
#define CDRMUTILITYUI_H

//*** system include files go here:
#include <e32base.h>
#include <f32file.h>
#include <AknNotifyStd.h>

//*** forward declarations go here:
class CDRMConstraint;
class CCoeEnv;
class CStringResourceReader;
class CEikDialog;
class CAknResourceNoteDialog;
class CMediatorEventProvider;

namespace ContentAccess
    {
    class CData;
    }

namespace DRM
{

    //*** forward declarations go here:
    class CDrmUtilityInfoNoteWrapper;

 /**
  *  Utility class for DRM utility related graphic functions
  *
  *  @lib drmutilitycommon.lib
  *  @since S60 v5.0
  */
NONSHARABLE_CLASS( CDrmUtilityUI ) : public CBase
    {

public:

    /**
     * Creates a new CDrmUtilityUI object and returns a pointer to it
     *
     * @return A functional CDrmUtilityUI -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmUtilityUI* NewL( CCoeEnv* aCoeEnv = NULL );

    /**
     * Creates a new CDrmUtilityUI object and returns a pointer to it
     * leaves the returned object to the cleanup stack
     *
     * @return A functional CDrmUtilityUI -object
     * @leave System wide error code
     */
    IMPORT_C static CDrmUtilityUI* NewLC( CCoeEnv* aCoeEnv = NULL );

    /**
     * Destructor
     */
    virtual ~CDrmUtilityUI();

    /**
    * Display query dialog
    */
    IMPORT_C TInt DisplayQueryWithIdL( TInt aTextResourceId,
                                       TInt aQueryResourceId );

    IMPORT_C TInt DisplayQueryWithIdValueL( TInt aTextResourceId,
                                            TInt aQueryResourceId,
                                            const TDesC& aString );

    IMPORT_C TInt DisplayQueryL( TInt aTextResourceId, TInt aValue );

    IMPORT_C TInt DisplayQueryL( TInt aTextResourceId, const TDesC& aString );

    IMPORT_C TInt DisplayQueryL( TInt aTextResourceId,
                                 const TDesC& aString,
                                 TInt aValue,
                                 TInt aStringPos = -1,
                                 TInt aValuePos = -1 );

    IMPORT_C TInt DisplayQueryL( TDesC& aPromptText, TInt aQueryResourceId );

    IMPORT_C TInt SetAutomatedQueryL( CDRMConstraint* aConstraint );

   /**
    * Display note
    */
    IMPORT_C void ShowFutureRightsNoteL( CDRMConstraint* aConstraint );

    IMPORT_C void DisplayNoteL( TInt aTextResourceId );

    IMPORT_C void DisplayNoteL( TInt aTextResourceId, const TDesC& aString );

    IMPORT_C void DisplayNoteL( TInt aTextResourceId, TInt aValue );

    IMPORT_C void DisplayNoteL( TDesC& aPromptText,
                                TInt aResourceId,
                                const TDesC& aString = KNullDesC,
                                TInt aValue = -1 );

   /**
    * Display list query
    */
    IMPORT_C TInt DisplayPopupWindowsForPreviewL(
                                                ContentAccess::CData& aContent,
                                                TInt aPreviewType,
                                                TInt aMediaType );


protected:

private:

    CDrmUtilityUI( CCoeEnv* aCoeEnv );

    void ConstructL();

    CAknResourceNoteDialog* CreateNoteForResourceL( TInt aResId );

    TAknGlobalNoteType GlobalNoteTypeForResource( TInt aResId );

    TInt GetOmaStartTime( CDRMConstraint* aConstraint, TTime& aStartTime );

    TInt EvaluatePreviewMediaTypeL( ContentAccess::CData& aContent,
                                    TInt aPreviewType );

    void PrepareSecondaryDisplayL( CEikDialog& aDialog,
                                   TInt aResourceId,
                                   const TDesC& aString = KNullDesC,
                                   TInt aValue = -1 );

    void CancelSecondaryDisplayL( TInt aResourceId );


private: // Data members

    // Control environment, not owned
    CCoeEnv* iCoeEnv;

    // Paths of the resource files
    HBufC* iUtilityResourceFile;
    HBufC* iAvkonResourceFile;

    // Resource offsets in CoeEnv, when it is available
    TInt iUtilityResourceFileOffset;
    TInt iAvkonResourceFileOffset;

    // Used to read resources when CoeEnv is not available
    CStringResourceReader* iUtilityStringResourceReader;
    CStringResourceReader* iAvkonStringResourceReader;

    // File server
    RFs iFs;

    // Used to queue up global notes
    RPointerArray<CDrmUtilityInfoNoteWrapper> iNoteList;

    CMediatorEventProvider* iEventProvider;

    };
}

#endif // CDRMUTILITYUI_H
