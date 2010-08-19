/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Rights Object MTM UI
*
*/


// INCLUDE FILES

#include <txtrich.h>//CRichText
#include <eikrted.h> // CEikRichTextEditor
#include <eikenv.h>//CEikonEnv

#include <AiwGenericParam.h>
#include <AknLaunchAppService.h>

#include <eikon.rsg>
// Messaging includes
#include <msvuids.h>
#include <msvids.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <eikproc.h>
#include <eikdoc.h>
#include <AknDialog.h>
#include "RoMtmUi.h"
#include "RoMTMUiC.h"


#include <DocumentHandler.h> // document handler
#include <apaid.h> //TApaAppInfo
#include <apgcli.h> //RApaLsSession
#include <RoapEng.h> // CRoapEng
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <AknQueryDialog.h> // CAknQueryDialog
#include <StringLoader.h> //StringLoader
#include <ConeResLoader.h> //rconeresloader
#include <RoMtmUi.rsg> // test
#include "drmmsgwatcherobserver.h"

#include "cleanupresetanddestroy.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
_LIT(KRoAcquisitionPrefix, "ROA:");
_LIT(KTriggerPrefix, "TRI:");

#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveZ, "z:" );
#endif

_LIT( KRoMtmUiResFileName,"RoMtmUi.rsc" );
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================
// ============================ LOCAL FUNCTIONS ===============================
//#define _DRM_TESTING

#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteL( const TDesC& aText );
LOCAL_C void WriteL( const TDesC8& aText , TInt aErr );
LOCAL_C void WriteL( const TDesC& aText , TInt aErr );
LOCAL_C void WriteCurrentTimeL();
#endif

// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text , RFs &aFs )
    {
    _LIT( KLogFile , "c:\\CRightsObjectMtmUi.txt" );
    WriteFileL( text , aFs , KLogFile );
    }

LOCAL_C void WriteFileL( const TDesC8& text , RFs &aFs , const TDesC& aName )
    {
    RFile file;
    TInt size;
    User::LeaveIfError( file.Open( aFs, aName , EFileWrite ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( size ) );
    User::LeaveIfError( file.Write( size, text ) );
    CleanupStack::PopAndDestroy(); //file
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError( file.Replace( fs , _L("c:\\CRightsObjectMtmUi.txt") , EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy(); //fs
    }

LOCAL_C void WriteL( const TDesC& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8("\r\n") );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
    WriteCurrentTimeL();
    }

LOCAL_C void WriteL( const TDesC8& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(1000);
    TPtr8 textptr(text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8("\r\n") );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
    WriteCurrentTimeL();
    }

LOCAL_C void WriteL( const TDesC8& aText , TInt aErr )
    {
    _LIT8(KErr,": %d");
    HBufC8* text = HBufC8::NewLC(1000+20);
    TBuf8<20> num;
    TPtr8 textptr(text->Des());
    textptr.Append(aText);
    num.Format(KErr(),aErr);
    textptr.Append(num);
    WriteL(textptr);
    CleanupStack::PopAndDestroy(text);
    }

LOCAL_C void WriteL( const TDesC& aText , TInt aErr )
    {
    _LIT8(KErr,": %d");
    HBufC8* text = HBufC8::NewLC(1000+20);
    TBuf8<20> num;
    TPtr8 textptr(text->Des());
    textptr.Append(aText);
    num.Format(KErr(),aErr);
    textptr.Append(num);
    WriteL(textptr);
    CleanupStack::PopAndDestroy(text);
    }

LOCAL_C void WriteCurrentTimeL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    HBufC8* text = HBufC8::NewLC(100);
    TPtr8 textptr(text->Des() );
// Date and Time display
    TTime time;
    time.HomeTime();
    TBuf<256> dateString;
    _LIT(KDate,"%*E%*D%X%*N%*Y %1 %2 '%3");
    time.FormatL(dateString,KDate);
    textptr.Append(_L( "\r\n\t\tData:\t" ) );
    textptr.Append( dateString );
    _LIT(KTime,"%-B%:0%J%:1%T%:2%S%:3%+B");
    time.FormatL(dateString,KTime);
    textptr.Append(_L( "\r\n\t\tTime:\t" ) );
    textptr.Append( dateString );
    textptr.Append(_L( "\r\n" ) );
    textptr.Append(_L( "\r\n" ) );
    WriteLogL(textptr , fs);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(); //fs
    }
#endif
/*************************************
*
*       Global function declarations
*
**************************************/

EXPORT_C CBaseMtmUi* NewMtmUiL( CBaseMtm& aMtm , CRegisteredMtmDll& aRegisteredDll )
// Factory function
    {
    return CRightsObjectMtmUi::NewL( aMtm , aRegisteredDll );
    }
//
// CRightsObjectMtmUi: User Interface MTM
//

// ============================ MEMBER FUNCTIONS ===============================

/*************************************
*
*       Construction and destruction
*
**************************************/

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::CRightsObjectMtmUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmUi::CRightsObjectMtmUi( CBaseMtm& aBaseMtm , CRegisteredMtmDll& aRegisteredMtmDll )
    :   CBaseMtmUi( aBaseMtm , aRegisteredMtmDll )
    {
    iHostProcess = NULL;
    iType = ERo;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUi::ConstructL()
    {
#ifdef _DRM_TESTING
    CreateLogL();
    WriteL(_L8("ConstructL"));
#endif
    CBaseMtmUi::ConstructL();
    User::LeaveIfError(iFs.Connect());
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRightsObjectMtmUi* CRightsObjectMtmUi::NewL( CBaseMtm& aBaseMtm , CRegisteredMtmDll& aRegisteredMtmDll )
    {
    CRightsObjectMtmUi* self=new ( ELeave ) CRightsObjectMtmUi( aBaseMtm , aRegisteredMtmDll );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CRightsObjectMtmUi::~CRightsObjectMtmUi()
    {
#ifdef _DRM_TESTING
    TRAP(err,WriteL(_L8("~CRightsObjectMtmUi")));
#endif
    iFs.Close();
    }




/*************************************
*
*       from CBaseMtm
*
**************************************/

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::CancelL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::CancelL( TRequestStatus& /*aStatus*/ , const CMsvEntrySelection& /*aSelection*/ )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CancelL"));
#endif

    User::Leave( KErrNotSupported ); // no cancelling
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::GetResourceFileName
//
// Implementations provide this function to set the full path
// and name of the resource file associated with the User Interface MTM.
//
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUi::GetResourceFileName( TFileName& aFileName ) const
// Resource file loading
    {
#ifdef _DRM_TESTING
    TRAPD(err,WriteL(_L8("GetResourceFileName")));
#endif
    aFileName = KRoMtmResourceFile;
    }

//
//  Entry manipulation
//

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::OpenL
//
// Opens an entry.
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::OpenL( TRequestStatus& aStatus )
// Open
    {
#ifdef _DRM_TESTING
    WriteL(_L8("OpenL(1)"));
#endif

    TMsvEntry tEntry = BaseMtm().Entry().Entry();
    TMsvEntry newEntry( tEntry );

    const TUid type( tEntry.iType );
    CMsvOperation* operation = NULL;
    if ( type == KUidMsvMessageEntry )
        {
        operation = ViewL( aStatus );
        newEntry.SetUnread( EFalse );
        BaseMtm().Entry().ChangeL( newEntry );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return operation;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::OpenL
//
// Opens the first entry in the selections.
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::OpenL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection )
// Open selection
    {
#ifdef _DRM_TESTING
    WriteL(_L8("OpenL(2)"));
#endif
    BaseMtm().SwitchCurrentEntryL( aSelection.At( 0 ) );
    return OpenL( aStatus );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::CloseL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::CloseL( TRequestStatus& /*aStatus*/ )
    {
// Close
#ifdef _DRM_TESTING
    WriteL(_L8("CloseL(1)"));
#endif
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::CloseL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::CloseL( TRequestStatus& /*aStatus*/ , const CMsvEntrySelection& /*aSelection*/ )
    {
// Close selection
#ifdef _DRM_TESTING
    WriteL(_L8("CloseL(2)"));
#endif

    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::EditL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::EditL( TRequestStatus& /*aStatus*/ )
// Edit
    {
#ifdef _DRM_TESTING
    WriteL(_L8("EditL(1)"));
#endif
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::EditL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::EditL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection )
// Edit selection
    {
#ifdef _DRM_TESTING
    WriteL(_L8("EditL(2)"));
#endif
    iBaseMtm.SwitchCurrentEntryL( aSelection.At( 0 ) );
    return EditL( aStatus );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::ViewL
//
// Views an entry.
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::ViewL( TRequestStatus& aStatus )
// View
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ViewL(1)"));
#endif

    CheckMessageTypeL();
    return LaunchEditorApplicationL( aStatus , iBaseMtm.Entry().Entry().Id() , ETrue );
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::ViewL
//
// Views the first entry in the selection.
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::ViewL( TRequestStatus& aStatus , const CMsvEntrySelection& aSelection )
// View selection
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ViewL(2)"));
#endif

    iBaseMtm.SwitchCurrentEntryL( aSelection.At( 0 ) );
    return ViewL( aStatus );
    }


//
//  Message responding
//

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::ReplyL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::ReplyL( TMsvId /*aDestination*/ , TMsvPartList /*aPartlist*/ , TRequestStatus& /*aCompletionStatus*/ )
// Reply to message - no UI support
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::ForwardL
//
// Not supported
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::ForwardL( TMsvId /*aDestination*/ , TMsvPartList /*aPartlist*/ , TRequestStatus& /*aCompletionStatus*/ )
// Forwarded message - no UI support
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }


//
// Specific to CRightsObjectMtmUi
//


// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::CheckMessageTypeL
//
// Check the type of the message
//
// -----------------------------------------------------------------------------
//
void CRightsObjectMtmUi::CheckMessageTypeL()
    {
#ifdef _DRM_TESTING
        WriteL(_L8("CheckMessageTypeL"));
#endif
    BaseMtm().LoadMessageL();
    CRichText& body = BaseMtm().Body();
    TPtrC ptr16( body.Read(0) );

#ifdef _DRM_TESTING
        WriteL(ptr16);
#endif
    if ( ptr16.Length()>4 && ptr16.Left(4).Compare(KRoAcquisitionPrefix())==0 )
        {
        iType = ERoapTriggerRoAcquisition;
        }
    else if ( ptr16.Length()>4 && ptr16.Left(4).Compare(KTriggerPrefix())==0 )
        {
        iType = ERoapTrigger;
        }
    else
        {
        iType = ERo;
        }

    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::LaunchEditorApplicationL
//
// Launch RO Manager to get information for rights corresponding to the message.
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::LaunchEditorApplicationL( TRequestStatus& aStatus , TMsvId /*aId*/ ,
                                                    TBool /*aReadOnly*/ )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("LaunchEditorApplicationL-Start"));
#endif
    // get message context
    BaseMtm().LoadMessageL();
    CRichText& body = BaseMtm().Body();
    TPtrC ptr16( body.Read(0) );
    //const TUid KUidDRMUI = { 0x101f85c7 };
    CMsvOperation* operation( NULL );

    if ( iType == ERoapTrigger || iType == ERoapTriggerRoAcquisition )
        {
        _LIT8( KRoapTriggerMimeType, "application/vnd.oma.drm.roap-trigger+xml" );
        TDataType type = TDataType( KRoapTriggerMimeType );
        HBufC* filename = NULL;
        TPtr ptr(NULL,0);
        TBool show;
        filename = HBufC::NewLC(ptr16.Length()-4);
        ptr.Set(filename->Des());
        ptr.Append(ptr16.Right(ptr16.Length()-4));

        show = ShowQueryL(ptr);

        if (!show)
            {
            operation = CompletedOperationL( aStatus );
            }
        else
            {
            operation = CDrmMsgWatcherObserver::NewL(
                    Session(),
                    CActive::EPriorityStandard,
                    aStatus,
                    Type(),
                    ptr,
                    type );
            }

        CleanupStack::PopAndDestroy(filename);
        }
    else
        {
        CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
        TAiwVariant variantObject( ptr16 );
        TAiwGenericParam param( EGenericParamFile, variantObject );
        paramList->AppendL( param );

        operation = CDrmMsgWatcherObserver::NewL(
                Session(),
                CActive::EPriorityStandard,
                aStatus,
                Type(),
                paramList );

        CleanupStack::PopAndDestroy( paramList );
        }

    return operation;
    }


void CRightsObjectMtmUi::HandleServerAppExit(TInt aReason)
    {
#ifdef _DRM_TESTING
    TRAP(err,WriteL(_L8("HandleServerAppExit")));
#endif
    MAknServerAppExitObserver::HandleServerAppExit(aReason);
    }

// -----------------------------------------------------------------------------
// CRightsObjectMtmUi::CompletedOperationL
//
// return completed operation to UI
//
// -----------------------------------------------------------------------------
//
CMsvOperation* CRightsObjectMtmUi::CompletedOperationL( TRequestStatus& aObserverStatus )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CompletedOperationL"));
#endif
    //return completed operation pointer
    TPckgBuf<TMsvLocalOperationProgress> progress;
    SetProgressSuccess( progress, BaseMtm().Entry().Entry().Id() );
    CMsvCompletedOperation* operation =
        CMsvCompletedOperation::NewL( Session() , Type() , progress ,
        KMsvLocalServiceIndexEntryId , aObserverStatus );
    return operation;
    }

void CRightsObjectMtmUi::SetProgressSuccess
( TPckgBuf<TMsvLocalOperationProgress>& aProgress , TMsvId aId )
    {
    aProgress().iTotalNumberOfEntries = 1;
    aProgress().iNumberCompleted = 1;
    aProgress().iId = aId;
    }

TBool CRightsObjectMtmUi::ShowQueryL( TDesC16& aFile )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL"));
#endif
    TBool show = ETrue;
    TInt type = 0;
    HBufC* msgText = NULL;
    type = CheckTriggerTypeL(aFile);

#ifndef RD_MULTIPLE_DRIVE

    TFileName fileName( KDriveZ );

#else //RD_MULTIPLE_DRIVE

    _LIT( KDrive, "%c:");
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
    iFs.DriveToChar( driveNumber, driveLetter );

    TFileName fileName;

    fileName.Format( KDrive, (TUint)driveLetter );

#endif

    fileName.Append( KDC_MTM_RESOURCE_DIR );
    fileName.Append( KRoMtmUiResFileName );
    RConeResourceLoader loader( *iCoeEnv );
    loader.OpenL( fileName );
    switch(type)
        {
        case Roap::ERegistrationTrigger:
            {
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL->ERegistrationTrigger"));
#endif
            loader.Close();
            return ETrue;
            }
        case Roap::ERoAcquisitionTrigger:
            {
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL->ERoAcquisitionTrigger"));
#endif
            msgText = StringLoader::LoadLC( R_ROMTMUI_QUERY_RIGHTS_ACQ_MSG , iCoeEnv );
            }break;
        case Roap::EJoinDomainTrigger:
            {
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL->EJoinDomainTrigger"));
#endif
            msgText = StringLoader::LoadLC( R_ROMTMUI_QUERY_JOIN_DOMAIN_MSG , iCoeEnv );
            }break;
        case Roap::ELeaveDomainTrigger:
            {
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL->ELeaveDomainTrigger"));
#endif
            msgText = StringLoader::LoadLC( R_ROMTMUI_QUERY_LEAVE_DOMAIN_MSG , iCoeEnv );
            }break;
        default:
            {
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL->default"));
#endif
            loader.Close();
            return EFalse;
            }
        }
#ifdef _DRM_TESTING
    WriteL(*msgText);
#endif
    loader.Close();
    CAknQueryDialog* dlg = new (ELeave) CAknQueryDialog( *msgText,
        CAknQueryDialog::ENoTone );
    show = dlg->ExecuteLD( R_ROMTMUI_TRIGGER_CONFIRMATION_QUERY );
    CleanupStack::PopAndDestroy( msgText );
#ifdef _DRM_TESTING
    WriteL(_L8("ShowQueryL->End"),show);
#endif
    return show;
    }

TInt CRightsObjectMtmUi::CheckTriggerTypeL( TDesC16& aFile )
    {
#ifdef _DRM_TESTING
    WriteL(_L8("CheckTriggerTypeL"));
#endif
    Roap::CRoapEng* roap = NULL;
    RFile file;
    TInt size = 0;
    Roap::TTriggerType type;
    Roap::TRiContextStatus status;
    Roap::TDomainOperation op;
    RPointerArray<HBufC8> array;

    HBufC8* buf =NULL;
    TPtr8 ptr(NULL,0);
    User::LeaveIfError(file.Open(iFs, aFile, EFileShareReadersOrWriters));
    CleanupClosePushL(file);
    User::LeaveIfError(file.Size(size));
    buf = HBufC8::NewLC(size);
    ptr.Set(buf->Des());
    User::LeaveIfError(file.Read(ptr,size));
    CleanupStack::Pop(buf);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PushL(buf);
    roap = Roap::CRoapEng::NewL();
    CleanupStack::PushL(roap);
    CleanupResetAndDestroyPushL(array);
    roap->SetTriggerL(ptr,NULL,type,status,op,array);
    CleanupStack::PopAndDestroy(&array);
    CleanupStack::PopAndDestroy(roap);
    CleanupStack::PopAndDestroy(buf);

#ifdef _DRM_TESTING
    WriteL(_L8("CheckTriggerTypeL-End"),type);
#endif
    return type;
    }




