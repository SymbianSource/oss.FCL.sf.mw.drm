/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM plugin for receiving OMA Rights Objects
*
*/


// INCLUDE FILES
#include <txtrich.h>                    // for CRichText
#include <DRMMessageParser.h>           // for CDrmMessageParser
#include <DRMRights.h>                  // for CDRMRights
#include <ecom/implementationproxy.h>   // for TImplementationProxy
#include <push/cpushhandlerbase.h>      // for CPushHandlerBase
#include <push/pluginkiller.h>          // for CPluginKiller
#include <push/pushmessage.h>           // for CPushMessage
#include <RoapEng.h>                    // for CRoapEng
#include <RoapEngBase.h>
#include <RoapObserver.h>
#include <centralrepository.h>          // link against centralrepository.lib
#include <msvuids.h>
#include <msvids.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <uri16.h>                          // TUriParser16
#include <data_caging_path_literals.hrh>    // KDC_MTM_RESOURCE_DIR
#include <uriutils.h>                       // UriUtils and so on
#include <RoHandler.rsg>                    // for R_QTN_DRM_MGR_INB_TITLE
#include <sysutil.h>                        // Disk space checking

#include "CRoHandler.h"
#include "RoMtmCli.h"                       // for CRightsObjectMtmClient
#include "RoapSyncWrapper.h"

#include "RoHandlerStringResourceReader.h"
#include "rohandlerdmgrwrapper.h"
#include "rohandlerinternalcrkeys.h"

#ifdef _DEBUG
#define DRMDEBUGLIT( a, b ) _LIT( a, b )
#define DRMDEBUG( a ) RDebug::Print( a )
#define DRMDEBUG2( a, b ) RDebug::Print( a, b )
_LIT( KRoLogDir, "DRM" );
_LIT( KRoLogFile, "RoHandler.log" );
#define LOG( a ) RFileLogger::Write( KRoLogDir(), KRoLogFile(), EFileLoggingModeAppend, a );
#define LOGHEX( ptr, len ) RFileLogger::HexDump( \
    KRoLogDir(), KRoLogFile(), EFileLoggingModeAppend, _S(""), _S(""), ptr, len );
#define LOG2( a, b ) RFileLogger::WriteFormat( \
    KRoLogDir(), KRoLogFile(), EFileLoggingModeAppend, a, b );
#else
#define DRMDEBUGLIT( a, b )
#define DRMDEBUG( a )
#define DRMDEBUG2( a, b )
#define LOG( a )
#define LOGHEX( ptr, len )
#define LOG2( a, b )
#endif

using namespace Roap;


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x101F7B93, CRoHandler::NewL)
    //{{0x101F7B93}, CRoHandler::NewL}
    };

// For reading the string value of cenrep key Inbox entry visible (for
// received RO). The size of string "false" is 5.
const TInt KBooleanStringMaxSize = 5;

// MACROS

// LOCAL CONSTANTS AND MACROS

#ifdef RD_MULTIPLE_DRIVE
_LIT( KRoHandlerTriggerFilePath, "%c:\\system\\data\\" );
#else
_LIT( KDriveZ, "z:" );
_LIT( KRoHandlerTriggerFilePath, "c:\\system\\data\\" );
#endif

_LIT( KRoHandlerResourceFile, "RoHandler.rsc" );

_LIT8( KRoapTriggerElement, "roapTrigger" );
_LIT8( KWbxmlRoapTriggerElement, "\x03\x13j" );
_LIT8( KRoapTriggerRoAcquisition, "roAcquisition" );
_LIT8( KRoapTriggerMeteringReport, "meteringReport" );
_LIT8( KRoapRoPduElement, "roResponse" );

_LIT( KFalse, "false" );
_LIT( KZero, "0" );

_LIT( KRoAcquisitionPrefix, "ROA:" );
_LIT( KTriggerPrefix, "TRI:" );

_LIT( KEmpty, " " );

// MODULE DATA STRUCTURES

// Helper class for deleting file with given filename on cleanupstack
// Note does not own its members
// Used for cleaning up saved trigger if creating trigger related message
// inbox entry fails.
NONSHARABLE_CLASS( CFileDeleter ) : public CBase
    {
public:
    static CFileDeleter* NewLC( RFs& aFs, TFileName& aFileName )
        {
        CFileDeleter* self( new ( ELeave ) CFileDeleter( aFs, aFileName ) );
        CleanupStack::PushL( self );
        return self;
        }

    inline void SetDelete()
        {
        iDeleteFileOnDestroy = ETrue;
        }

    inline void SetNoDelete()
        {
        iDeleteFileOnDestroy = EFalse;
        }

    virtual ~CFileDeleter()
        {
        if ( iDeleteFileOnDestroy )
            {
            iFs.Delete( iFileName );
            }
        }

private:
    CFileDeleter()
        {
        }
    CFileDeleter( const CFileDeleter& )
        {
        }
    CFileDeleter( RFs& aFs, TFileName& aFileName )
        : iFs( aFs ), iFileName( aFileName )
        {
        }

    CFileDeleter& operator=( const CFileDeleter& );

    TBool iDeleteFileOnDestroy;
    RFs iFs;
    TFileName iFileName;
    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text, RFs &aFs );
LOCAL_C void WriteFileL( const TDesC8& text, RFs &aFs, const TDesC& aName );
LOCAL_C void CreateLogL();
LOCAL_C void WriteL( const TDesC8& aText );
LOCAL_C void WriteL( const TDesC& aText );
LOCAL_C void WriteL( const TDesC8& aText, TInt aErr );
LOCAL_C void WriteL( const TDesC& aText, TInt aErr );
LOCAL_C void WriteCurrentTimeL();
#endif

// -----------------------------------------------------------------------------
// Testing stuff
// -----------------------------------------------------------------------------
//

#ifdef _DRM_TESTING
LOCAL_C void WriteLogL( const TDesC8& text, RFs &aFs )
    {
    _LIT( KLogFile, "c:\\CROHandler.txt" );
    WriteFileL( text, aFs, KLogFile );
    }

LOCAL_C void WriteFileL( const TDesC8& text, RFs &aFs, const TDesC& aName )
    {
    RFile file;
    TInt size;
    User::LeaveIfError( file.Open( aFs, aName, EFileWrite ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( size ) );
    User::LeaveIfError( file.Write( size, text ) );
    CleanupStack::PopAndDestroy( &file );
    }

LOCAL_C void CreateLogL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    RFile file;
    User::LeaveIfError( file.Replace( fs, _L( "c:\\CROHandler.txt" ), EFileWrite ) );
    file.Close();
    CleanupStack::PopAndDestroy( &fs );
    }

LOCAL_C void WriteL( const TDesC& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    HBufC8* text = HBufC8::NewLC( 1000 );
    TPtr8 textptr( text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8( "\r\n" ) );
    WriteLogL( textptr, fs );
    CleanupStack::PopAndDestroy( text );
    CleanupStack::PopAndDestroy( &fs );
    WriteCurrentTimeL();
    }

LOCAL_C void WriteL( const TDesC8& aText )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    HBufC8* text = HBufC8::NewLC( 1000 );
    TPtr8 textptr( text->Des() );
    textptr.Append( aText );
    textptr.Append( _L8( "\r\n" ) );
    WriteLogL( textptr, fs );
    CleanupStack::PopAndDestroy( text );
    CleanupStack::PopAndDestroy( &fs );
    WriteCurrentTimeL();
    }

LOCAL_C void WriteL( const TDesC8& aText, TInt aErr )
    {
    _LIT8( KErr, ": %d" );
    HBufC8* text = HBufC8::NewLC( 1000 + 20 );
    TBuf8<20> num;
    TPtr8 textptr( text->Des() );
    textptr.Append( aText );
    num.Format( KErr(), aErr );
    textptr.Append( num );
    WriteL( textptr );
    CleanupStack::PopAndDestroy( text );
    }

LOCAL_C void WriteL( const TDesC& aText, TInt aErr )
    {
    _LIT8( KErr, ": %d" );
    HBufC8* text = HBufC8::NewLC( 1000+20 );
    TBuf8<20> num;
    TPtr8 textptr( text->Des() );
    textptr.Append( aText );
    num.Format( KErr(), aErr );
    textptr.Append( num );
    WriteL( textptr );
    CleanupStack::PopAndDestroy( text );
    }

LOCAL_C void WriteCurrentTimeL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    HBufC8* text = HBufC8::NewLC( 100 );
    TPtr8 textptr( text->Des() );
    // Date and Time display
    TTime time;
    time.HomeTime();
    TBuf<256> dateString;
    _LIT( KDate, "%*E%*D%X%*N%*Y %1 %2 '%3" );
    time.FormatL( dateString, KDate );
    textptr.Append( _L( "\r\n\t\tData:\t" ) );
    textptr.Append( dateString );
    _LIT( KTime, "%-B%:0%J%:1%T%:2%S%:3%+B" );
    time.FormatL( dateString, KTime );
    textptr.Append( _L( "\r\n\t\tTime:\t" ) );
    textptr.Append( dateString );
    textptr.Append( _L( "\r\n" ) );
    textptr.Append( _L( "\r\n" ) );
    WriteLogL( textptr, fs );
    CleanupStack::PopAndDestroy( text );
    CleanupStack::PopAndDestroy( &fs );
    }
#endif


// ----------------------------------------------------------------------------
// DoResetAndDestroy
// Does RPointerArray< >->ResetAndDestroy() for the given array aPtr.
// ----------------------------------------------------------------------------
//
LOCAL_C void DoResetAndDestroy( TAny* aPtr )
    {
    ( reinterpret_cast< RPointerArray< CDRMRights >* >( aPtr ) )->ResetAndDestroy();
    delete aPtr;
    aPtr = NULL;
    }

// ----------------------------------------------------------------------------
// LeaveIfNullL
// Leaves with given error or with KErrArgument if aBuf is null
// ----------------------------------------------------------------------------
//
LOCAL_C inline void LeaveIfNullL( const TInt aRet, const HBufC8* aBuf )
    {
    if ( !aBuf )
        {
        User::LeaveIfError( aRet );
        User::Leave( KErrArgument );
        }
    }

// ----------------------------------------------------------------------------
// IsMeteringSupported
// ----------------------------------------------------------------------------
//
LOCAL_C TBool IsMeteringSupported()
    {
#ifdef RD_DRM_METERING
    return ETrue;
#else
    return EFalse;
#endif
    }


// ============================ MEMBER FUNCTIONS ===============================

/*
-----------------------------------------------------------------------------

    Method: NewL

    Description: 1st phase constructor

    Return Value: new CRoHandler

    Status: Proposal

-----------------------------------------------------------------------------
*/

CRoHandler* CRoHandler::NewL
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, CreateLogL() );
    TRAP( r, WriteL( _L8( "NewL" ) ) );
#endif
    CRoHandler* self( new( ELeave ) CRoHandler() );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "NewL-End" ) ) );
#endif
    return self;
    }

/*
-----------------------------------------------------------------------------

    Method: CRoHandler

    Description: C++ default constructor. Initialises the object
                 with zero/NULL values.

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

CRoHandler::CRoHandler
        (
        //None.
        )
    : CPushHandlerBase(),
    iFirstTime( ETrue ), iPushMsg( NULL ), iMsvId( NULL ),
    iPutRightsToInbox( ETrue )
    {
    }

/*
-----------------------------------------------------------------------------

    Method: ConstructL

    Description: Adds the AO to the Active Scheduler. Constructs iDrm object
                 then connects to drm server

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::ConstructL
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "ConstructL" ) ) );
#endif

    CRepository* repository( NULL );
    TInt err( KErrNone );

    User::LeaveIfError( iFs.Connect() );

    // create drm
    iMessageParser = CDRMMessageParser::NewL();

    // Create wbxml trigger parser instance
    iWbxmlTriggerParser = DRM::CWbxmlRoapTriggerParser::NewL();

    // Create CMsvSession
    // new session is opened Synchronously
    iSession = CMsvSession::OpenSyncL( *this );

    iMtmReg = CClientMtmRegistry::NewL( *iSession );

    // Check if metering feature is active
    iMeteringSupported = IsMeteringSupported();

    // Check cenrep key in order to find out whether received RO
    // should be stored to inbox or not.
    TRAP( err, repository = CRepository::NewL( KCRUidRoHandler ) );
    if ( !err )
        {
        CleanupStack::PushL( repository );
        TBuf<KBooleanStringMaxSize> string;
        TInt error = repository->Get( KDRMRoHandlerInboxEntryVisible,
        string );

        // If the value of the repository key is found either "false" or
        // "0", do not store rights to an Inbox entry.
        if ( ( string.CompareF( KFalse ) == 0 ) ||
            ( string.CompareF( KZero ) == 0 ) )
        {
        iPutRightsToInbox = EFalse;
        }

    CleanupStack::PopAndDestroy( repository );
        }

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "Repository->Get done" ), iPutRightsToInbox ) );
    TRAP( r, WriteL( _L8( "ConstructL-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: ~CRoHandler

    Description: Calls also baseclass destructor which calls
                 REcomSession::DestroyedImplementation( iDtor_ID_Key).

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

CRoHandler::~CRoHandler
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "~CRoHandler" ) ) );
#endif

    iFs.Close();
    // Delete the necessary instance attributes
    delete iPushMsg;
    delete iMessageParser;
    delete iWbxmlTriggerParser;
    delete iParsedXmlTrigger;
    delete iMtm;
    delete iMtmReg;

    iMsvId = NULL;

    // session must be deleted last (and constructed first)
    delete iSession;

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "~CRoHandler-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: AddRoContentL

    Description: Adds Message content and number

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/
void CRoHandler::AddRoContentL
        (
        TDesC& aMessageContent             //message content descriptor
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "AddRoContentL" ) ) );
#endif
    TMsvEntry msvEntry( ( iMtm->Entry() ).Entry() );

    // We get the message body from Mtm and insert a bodytext
    CRichText& mtmBody( iMtm->Body() );
    mtmBody.Reset();
    mtmBody.InsertL( 0, aMessageContent ); // insert our msg tag as the body text

    // set iRecipient into the Details of the entry
    msvEntry.SetNew( ETrue );              // set New
    msvEntry.SetUnread( ETrue );
    msvEntry.SetVisible( ETrue );
    msvEntry.SetInPreparation( EFalse );   // set inPreparation to false
    msvEntry.iDate.UniversalTime();        // set time to Universal Time

    // To handle the sms specifics we start using SmsMtm
    CRightsObjectMtmClient* roMtm( NULL );
    roMtm = static_cast< CRightsObjectMtmClient* >( iMtm );

    // save message
    CMsvEntry& entry( iMtm->Entry() );
    entry.ChangeL( msvEntry );             // make sure that we are handling the right entry
    roMtm->SaveMessageL();                 // closes the message

    // This moves the message entry to Inbox
    MoveMessageEntryL( KMsvGlobalInBoxIndexEntryId );

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "AddRoContentL-End" ) ) );
#endif
    }


/*
-----------------------------------------------------------------------------

    Method: HandleSessionEventL

    Description: Starts the message handling procedure asynchronously.
                 Basically all the work is done in RunL method.

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/
void CRoHandler::HandleSessionEventL
        (
        TMsvSessionEvent /* aEvent*/ ,
        TAny* /* aArg1*/ ,
        TAny* /* aArg2 */,
        TAny* /* aArg3 */
        )
    {
    }

/*
-----------------------------------------------------------------------------

    Method: HandleMessageL

    Description: Asynchronous version of the HandleMessageL().
                 NOT SUPPORTED

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/
void CRoHandler::HandleMessageL
        (
        CPushMessage* aPushMsg,  // push message
        TRequestStatus& aStatus  // status of the request
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "HandleMessageL(2)" ) ) );
#endif
    iPushMsg = aPushMsg;
    SetConfirmationStatus( aStatus );
    SignalConfirmationStatus( KErrNotSupported );
    iPluginKiller->KillPushPlugin();
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "HandleMessageL(2)-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: HandleMessageL

    Description: Synchronous version of the HandleMessageL().

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::HandleMessageL
        (
        CPushMessage* aPushMsg // push message
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "HandleMessageL(1)" ) ) );
#endif
    iPushMsg = aPushMsg;
    //
    // Do sanity checks for the message.
    //
    User::LeaveIfError( PerformChecks() );
    iMsgType = CheckMessageTypeL();
    switch( iMsgType )
        {
        case EOma1Ro:
            {
            HandleRightsMessageL();
            break;
            }
#ifdef __DRM_OMA2
        case EOma2RoapTrigger:
        case EOma2RoapTriggerRoAcquisition:
            {
            HandleRoapTriggerL();
            break;
            }
        case EOma2RoapTriggerMetering:
            {
            HandleMeteringTriggerSilentlyL();
            break;
            }
        case EOma2RoapPdu:
            {
            HandleRoapPduL();
            break;
            }
#endif
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }

    iPluginKiller->KillPushPlugin();

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "HandleMessageL(1)-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: HandleRightsMessageL

    Description: Process rights object and

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/


void CRoHandler::HandleRightsMessageL()
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "HandleRightsMessageL" ) ) );
#endif

    HBufC16 *number( NULL );
    HBufC16 *messageContent( NULL );
    HBufC16* buffer( NULL );
    _LIT( KMarker, "\x00" );

    TInt ret( 0 );
    if ( iMessageBodyPtr.Size() == 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Heap desc contains Content URI
    HBufC8* contentURI( NULL );
    TUint32 localId( 0 );
    RPointerArray< CDRMRights >*
        rights( new ( ELeave ) RPointerArray< CDRMRights > );

    if ( rights )
        {
        // process rights
        ret = iMessageParser->ProcessRightsObject( iMessageBodyPtr, *rights );
        }
    else
        {
        User::Leave( KErrNoMemory );
        }

    if ( !ret && rights->Count() )
        {
        TCleanupItem cleanup( DoResetAndDestroy, rights );
        CleanupStack::PushL( cleanup );
        ret = ( *rights )[ 0 ]->GetContentURI( contentURI );
        // null contentURI means invalid RO
        LeaveIfNullL( ret, contentURI );
        localId = ( *rights )[ 0 ]->GetLocalID();
        CleanupStack::PopAndDestroy( rights );
        rights = NULL;
        CleanupStack::PushL( contentURI );
        }
    else
        {
        rights->ResetAndDestroy();
        delete rights;
        rights = NULL;
        User::LeaveIfError( ret );
        }

    if ( iPutRightsToInbox && contentURI ) // coverity check
        {
        buffer = HBufC16::NewL( contentURI->Length() );
        TPtr uri16( buffer->Des() );
        uri16.Copy( *contentURI );
        CleanupStack::PopAndDestroy( contentURI );
        contentURI = NULL;
        CleanupStack::PushL( buffer );

        number = HBufC16::NewLC( 11 ); //longer than max of tuint32
        TPtr ptr( number->Des() );
        ptr.AppendNum( localId, EDecimal );

        messageContent = HBufC16::NewL( ptr.Length() + uri16.Length() + 5 );
        TPtr ptrToMz( messageContent->Des() );
        ptrToMz.Append( _L( "1" ) );
        ptrToMz.Append( KMarker );
        ptrToMz.Append( ptr ); // add localID
        ptrToMz.Append( KMarker );
        ptrToMz.Append( uri16 ); //add uri16
        ptrToMz.Append( KMarker );

        CleanupStack::PopAndDestroy( number );
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PushL( messageContent );

        // create empty sms
        iMsvId = CreateNewMessageL();
        SetEntryL( iMsvId );
        // adds content in sms and moves it inbox
        AddRoContentL( ptrToMz );

        CleanupStack::PopAndDestroy( messageContent );
        }
    else
        {
        CleanupStack::PopAndDestroy( contentURI );
        }

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "HandleRightsMessageL-End" ) ) );
#endif
    }


/*
-----------------------------------------------------------------------------

    Method: HandleRoapPduL

    Description: Handles OMA 2.0 ROAP RO Response message

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/
void CRoHandler::HandleRoapPduL()
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "HandleRoapPduL" ) ) );
#endif
    Roap::CRoapEngBase* roapHandler( NULL );
    RPointerArray< CDRMRights > rights;

    roapHandler = CRoapEng::NewL();
    CleanupStack::PushL( roapHandler );

    TCleanupItem cleanup( DoResetAndDestroy, &rights );
    CleanupStack::PushL( cleanup );


    //Parse received rights.
#ifdef  _DRM_TESTING
    TRAPD( err, roapHandler->HandleRoReponseL( iMessageBodyPtr, rights ) );
    TRAP( r, WriteL( _L8( "HandleRoapPduL->HandleRoReponseL done" ), err ) );
    User::LeaveIfError( err );
#else
    roapHandler->HandleRoReponseL( iMessageBodyPtr, rights );
#endif


    //handle parsed rights and save uri into mtm
    if ( rights.Count() )
        {
        HBufC8* contentURI( NULL );
        HBufC16* buffer( NULL );
        HBufC16* number( NULL );
        HBufC16* messageContent( NULL );
        TUint32 localId( 0 );
        TInt ret( KErrNone );

#ifdef  _DRM_TESTING
        TRAP( r, WriteL( _L8( "HandleRoapPduL->CID" ) ) );
#endif
        ret = rights[0]->GetContentURI( contentURI );
        // null contentURI means invalid RO
        LeaveIfNullL( ret,  contentURI );
#ifdef  _DRM_TESTING
        TRAP( r, WriteL( *contentURI ) );
#endif
        localId = rights[0]->GetLocalID();
        CleanupStack::PushL( contentURI );
        buffer = HBufC16::NewL( contentURI->Length() );
        TPtr uri16( buffer->Des() );
        uri16.Copy( *contentURI );
        CleanupStack::PopAndDestroy( contentURI );
        contentURI = NULL;
        CleanupStack::PushL( buffer );
        number = HBufC16::NewLC( 11 ); //longer than max of tuint32
        TPtr ptr( number->Des() );
        ptr.AppendNum( localId, EDecimal );
        messageContent = HBufC16::NewL( ptr.Length() + uri16.Length() + 4 );
        TPtr ptrToMz( messageContent->Des() );
        ptrToMz.Append( _L( "1 " ) );
        ptrToMz.Append( ptr ); // add localID
        ptrToMz.Append( _L( " " ) ); // add space
        ptrToMz.Append( uri16 ); //add uri16
        CleanupStack::PopAndDestroy( number );
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PushL( messageContent );
        // create empty sms
        iMsvId = CreateNewMessageL();
        SetEntryL( iMsvId );
        // adds content in sms and moves it inbox
        AddRoContentL( ptrToMz );
        CleanupStack::PopAndDestroy( messageContent );
        }

    // Finish up
    CleanupStack::PopAndDestroy( &rights );
    CleanupStack::PopAndDestroy( roapHandler );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "HandleRoapPduL-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: HandleRoapTriggerL

    Description: Handles OMA 2.0 ROAP Trigger message

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/
void CRoHandler::HandleRoapTriggerL()
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "HandleRoapTriggerL" ) ) );
#endif

    _LIT( KFile, "%S%u.tri" );
    RFile f;
    TFileName fileName;
    TPtr ptr( NULL, 0 );
    HBufC* content( NULL );
    TInt ret( KErrNone );
    TInt driveNumber( -1 );


    // create empty sms
    iMsvId = CreateNewMessageL();
    SetEntryL( iMsvId );

#ifndef RD_MULTIPLE_DRIVE

    driveNumber = EDriveC;
    ret = iFs.MkDirAll( KRoHandlerTriggerFilePath );
    fileName.Format( KFile, &KRoHandlerTriggerFilePath, iMsvId );

#else //RD_MULTIPLE_DRIVE

    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
    iFs.DriveToChar( driveNumber, driveLetter );

    TFileName roHandlerTriggerFilePath;
    roHandlerTriggerFilePath.Format(
                    KRoHandlerTriggerFilePath, ( TUint )driveLetter );

    ret = iFs.MkDirAll( roHandlerTriggerFilePath );
    fileName.Format( KFile, &roHandlerTriggerFilePath, iMsvId );

#endif
    if ( ret && ret != KErrAlreadyExists )
        {
        User::LeaveIfError( ret );
        }

    // Create cleanup item for file to be deleted on leave.
    CFileDeleter* fileDeleter( CFileDeleter::NewLC( iFs, fileName ) );

    // Leave if there is not enough space for file to be created.
    if ( SysUtil::DiskSpaceBelowCriticalLevelL(
            &iFs, iMessageBodyPtr.Size(), driveNumber ) )
        {
        User::Leave( KErrDiskFull );
        }

    User::LeaveIfError( f.Replace( iFs, fileName, EFileWrite ) );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( fileName, iMsvId ) );
#endif
    CleanupClosePushL( f );
    User::LeaveIfError( f.Write( iMessageBodyPtr ) );
    CleanupStack::PopAndDestroy( &f );

    // Delete created file on leave.
    fileDeleter->SetDelete();


    //format content
    if ( iMsgType == EOma2RoapTriggerRoAcquisition )
        {
#ifdef _DRM_TESTING
        TRAP( r, WriteL( _L8( "HandleRoapTriggerL->EOma2RoapTriggerRoAcquisition" ) ) );
#endif
        content = HBufC::NewLC(
            fileName.Length() + KRoAcquisitionPrefix().Length() );
        ptr.Set( content->Des() );
        ptr.Append( KRoAcquisitionPrefix() );
        ptr.Append( fileName );
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
        // do the ROAP silently, but if it fails, create the inbox entry
        if ( !DoRoapL( iMessageBodyPtr ) )
            {
            AddRoContentL( ptr );
            }
#else
        // adds content in sms and moves it inbox
        AddRoContentL( ptr );
#endif
        CleanupStack::PopAndDestroy( content );
        }
    else
        {
        content = HBufC::NewLC(
            fileName.Length() + KTriggerPrefix().Length() );
        ptr.Set( content->Des() );
        ptr.Append( KTriggerPrefix() );
        ptr.Append( fileName );
        AddRoContentL( ptr );
        CleanupStack::PopAndDestroy( content );
        }
    // No leaves. So keep created file.
    fileDeleter->SetNoDelete();
    CleanupStack::PopAndDestroy( fileDeleter );

#ifdef _DRM_TESTING
    TRAP( r, WriteL( ptr ) );
#endif

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "HandleRoapTriggerL-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: CancelHandleMessage

    Description: Cancels the pending asynchronous HandleMessageL.

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::CancelHandleMessage
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "CancelHandleMessage" ) ) );
#endif

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "CancelHandleMessage-End" ) ) );
#endif
    }


/*
-----------------------------------------------------------------------------

    Method: CreateNewMessageL

    Description: Creates a new message server entry and set up default values.

    Return values:      TMsvId (the id of created entry)

    Status: Proposal

-----------------------------------------------------------------------------
*/

TMsvId CRoHandler::CreateNewMessageL
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "CreateNewMessageL" ) ) );
#endif
    TMsvEntry newEntry;// This represents an entry in the Message Server index
    newEntry.iMtm = KUidMsgTypeRO;// message type is RO
    newEntry.iType = KUidMsvMessageEntry;                   // this defines the type of the entry: message
    newEntry.iServiceId = KMsvLocalServiceIndexEntryId;     // ID of local service (containing the standard folders)
    newEntry.iDate.UniversalTime();                         // set the date of the entry to Universal time
    newEntry.SetVisible( EFalse );
    newEntry.SetUnread( ETrue );                            // a soft notification would come

    HBufC* detail( GetDetailLC() );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L( "detail" ) ) );
    TRAP( r, WriteL( *detail, detail->Des().Length() ) );
#endif

    HBufC* description( GetDescriptionLC() );

    newEntry.iDetails.Set( detail->Des() );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L( "newEntry.iDetails:" ) ) );
    TRAP( r, WriteL( newEntry.iDetails, newEntry.iDetails.Length() ) );
#endif

    newEntry.iDescription.Set( description->Des() );

    newEntry.SetInPreparation( ETrue );                       // a flag that this message is in preparation
    //----
    //newEntry.iBioType = 0x1000ffff;                         // define a bio UID if sending a bio message over SMS bearer
    //----

    // - CMsvEntry accesses and acts upon a particular Message Server entry.
    // - NewL() does not create a new entry, but simply a new object to access an existing entry.
    // - It takes in as parameters the client's message server session,
    //   ID of the entry to access and initial sorting order of the children of the entry.
    //
    CMsvEntry* entry( NULL );
    entry = CMsvEntry::NewL(
        *iSession, KMsvDraftEntryIdValue, TMsvSelectionOrdering() );
    CleanupStack::PushL( entry );

    CMsvOperationActiveSchedulerWait* wait( NULL );
    wait = CMsvOperationActiveSchedulerWait::NewLC();
    CMsvOperation* oper( entry->CreateL( newEntry, wait->iStatus ) );
    CleanupStack::PushL( oper );
    wait->Start();

    // ...and keep track of the progress of the create operation.
    TMsvLocalOperationProgress progress(
        McliUtils::GetLocalProgressL( *oper ) );
    User::LeaveIfError( progress.iError );

    // Set our entry context to the created one
    entry->SetEntryL( progress.iId ); // operation progress contains the ID of the ceated entry
    CleanupStack::PopAndDestroy( oper );
    CleanupStack::PopAndDestroy( wait );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( description );
    CleanupStack::PopAndDestroy( detail );

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "CreateNewMessageL-End" ) ) );
#endif
    return progress.iId;
    }



HBufC* CRoHandler::ConvertDetailsL( const TDesC8& aFrom )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "ConvertDetailsL" ) ) );
#endif
    HBufC* from( HBufC::NewMaxLC( aFrom.Length() ) );
    from->Des().Copy( aFrom );

    TUriParser16 pars;
    User::LeaveIfError( pars.Parse( *from ) );

    HBufC* res( NULL );
    if ( pars.IsPresent( EUriHost ) )
        {
        res = pars.Extract( EUriHost ).AllocL();
        }
    else
        {
        res = from->AllocL();
        }

    CleanupStack::PopAndDestroy( from );

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "ConvertDetailsL-End" ) ) );
#endif
    return res;
    }

void CRoHandler::ReadFromResourceLC(
            const TDesC& aFile,
            const TInt& aIndex,
            HBufC*& aBuf )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "ReadFromResourceLC" ) ) );
#endif
    RFs fs;
    if ( aBuf )
        {
        delete aBuf;
        aBuf = NULL;
        }
    User::LeaveIfError( fs.Connect() );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "ReadFromResourceLC-fs.Connect" ) ) );
#endif
    CleanupClosePushL( fs );
    CRoHandlerStringResourceReader* reader(
        new ( ELeave ) CRoHandlerStringResourceReader( fs, aFile ) );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "ReadFromResourceLC-CRoHandlerStringResourceReader" ) ) );
#endif
    CleanupStack::PushL( reader );
    aBuf = reader->AllocReadResourceL( aIndex );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "ReadFromResourceLC-AllocReadResourceL" ) ) );
#endif
    CleanupStack::PopAndDestroy( reader );
    CleanupStack::PopAndDestroy( &fs );
    CleanupStack::PushL( aBuf );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "ReadFromResourceLC-End" ) ) );
#endif
    }

HBufC* CRoHandler::GetDetailLC()
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "GetDetailLC" ) ) );
#endif
    // Get server address.
    TPtrC8 srvAddress;
    TBool flag( iPushMsg->GetServerAddress( srvAddress ) );
    HBufC* buf( NULL );
    HBufC* result( NULL );

    // First line in Inbox: TMsvEntry::iDetails.
    if ( !flag || srvAddress.Length() == 0 )
        {
        result = KEmpty().AllocL();
        }
    else
        {
        // Convert the "From" information to the format required by the UI
        // spec and then decode it.
#ifdef _DRM_TESTING
        TRAP( r, WriteL( _L( "From form: " ) ) );
        TRAP( r, WriteL( srvAddress, srvAddress.Length() ) );
#endif
        buf = ConvertDetailsL( srvAddress );
#ifdef _DRM_TESTING
        TRAP( r, WriteL( _L( "Uri form: " ) ) );
        TRAP( r, WriteL( *buf, buf->Length() ) );
#endif
        CleanupStack::PushL( buf );
        result = ConvertUriToDisplayFormL( *buf );
#ifdef _DRM_TESTING
        TRAP( r, WriteL( _L( "Final form: " ) ) );
        TRAP( r, WriteL( *result, result->Length() ) );
#endif
        CleanupStack::PopAndDestroy( buf ); // buf
        buf = NULL;
        CleanupStack::PushL( result );
        }
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "GetDetailLC-End" ) ) );
#endif
    return result;
    }

HBufC* CRoHandler::ConvertUriToDisplayFormL( const TDesC& aUri )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "ConvertUriToDisplayFormL" ) ) );
#endif
    HBufC8* uri8( HBufC8::NewMaxLC( aUri.Length() ) );
    uri8->Des().Copy( aUri );
    TUriParser8 uriParser8;
    User::LeaveIfError( uriParser8.Parse( *uri8 ) );
    CUri16* convertedCUri( UriUtils::ConvertToDisplayFormL( uriParser8 ) );
    CleanupStack::PopAndDestroy( uri8 );
    CleanupStack::PushL( convertedCUri );
    HBufC* convertedUri( convertedCUri->Uri().UriDes().AllocL() );
    CleanupStack::PopAndDestroy( convertedCUri );
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "ConvertUriToDisplayFormL-End" ) ) );
#endif
    return convertedUri;
    }

HBufC* CRoHandler::GetDescriptionLC()
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "GetDescriptionLC" ) ) );
#endif
    HBufC* buf( NULL );

#ifndef RD_MULTIPLE_DRIVE

    TFileName resourceFile( KDriveZ );

#else //RD_MULTIPLE_DRIVE

    _LIT( KDriveRoot, "%c:" );
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRom, driveNumber );
    iFs.DriveToChar( driveNumber, driveLetter );

    TFileName resourceFile;
    resourceFile.Format( KDriveRoot, ( TUint )driveLetter );

#endif

    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KRoHandlerResourceFile );

    if ( iMsgType == EOma2RoapTrigger )
        {
#ifdef _DRM_TESTING
    TRAP( r, WriteL( resourceFile ) );
#endif
        ReadFromResourceLC(
            resourceFile, R_ROHL_INBOX_DESCRIPTION_ROA, buf );
        }
    else
        {
#ifdef _DRM_TESTING
    TRAP( r, WriteL( resourceFile ) );
#endif
        ReadFromResourceLC( resourceFile, R_ROHL_INBOX_DESCRIPTION, buf );
        }

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "GetDescriptionLC-End" ) ) );
#endif
    return buf;
    }

/*
-----------------------------------------------------------------------------

    Method: DoCancel

    Description:Cancels the operation.

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::DoCancel
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "DoCancel" ) ) );
#endif

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "DoCancel-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: RunL

    Description: Basically all the work is done from/through this method

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::RunL
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "RunL" ) ) );
#endif
    // First checking whether cancel has been called
    if ( iStatus == KErrCancel )
        {
        iPluginKiller->KillPushPlugin();
        return;
        }

    switch( iMsgType )
        {
        case EOma1Ro:
            {
            HandleRightsMessageL();
            break;
            }
#ifdef __DRM_OMA2
        case EOma2RoapTrigger:
        case EOma2RoapTriggerRoAcquisition:
            {
            HandleRoapTriggerL();
            break;
            }
        case EOma2RoapPdu:
            {
            HandleRoapPduL();
            break;
            }
#endif
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "RunL-End" ) ) );
#endif
    }
/*
-----------------------------------------------------------------------------

    Method: RunError

    Description: Called by ActiveScheduler in case RunL leaves.
                 Currently does nothing.

    Return Value: <errorcode>: Currently same as was given as a parameter

    Status: Proposal

-----------------------------------------------------------------------------
*/

TInt CRoHandler::RunError
        (
        TInt
#ifdef _DRM_TESTING
        aError // errorcode
#endif
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "RunError" ) ) );
#endif

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "RunError-End" ), aError ) );
#endif
    return KErrNone;
    }

/*
-----------------------------------------------------------------------------

    Method: CPushHandlerBase_Reserved1

    Description: Reserved for future expansion.

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::CPushHandlerBase_Reserved1
        (
        //None.
        )
    {
    _LIT( KNotSupported, "This method is not supported!" );
    User::Panic( KNotSupported, KErrNotSupported );
    }

/*
-----------------------------------------------------------------------------

    Method: CPushHandlerBase_Reserved2

    Description: Reserved for future expansion.

    Return Value: None.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void CRoHandler::CPushHandlerBase_Reserved2
        (
        //None.
        )
    {
    _LIT( KNotSupported, "This method is not supported!" );
    User::Panic( KNotSupported, KErrNotSupported );
    }

/*
-----------------------------------------------------------------------------

    Method: PerformChecks

    Description: Checks that message is current type

    Return Value:  TInt: KErrNone if ok else KErrCorrupt.

    Status: Proposal

-----------------------------------------------------------------------------
*/

TInt CRoHandler::PerformChecks
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "PerformChecks" ) ) );
#endif
    if ( !iPushMsg )
        {
        return KErrCorrupt;
        }
    // Check that body != NULL
    TBool bodyPresent( EFalse );
    // fetch message body
    bodyPresent = iPushMsg->GetMessageBody( iMessageBodyPtr );

    // 6 == minimum number of message fields. This is a very "mild" check but
    // at least it guarantees that there is no empty body in the message
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "PerformChecks-End" ) ) );
#endif
    if ( ( !bodyPresent ) || ( iMessageBodyPtr.Size() < 6 ) )
        {
        return KErrCorrupt;
        }
    return KErrNone;
    }


/*
-----------------------------------------------------------------------------

    Method: CheckMessageTypeL

    Description: Checks the message type

    Return Value:  EOma1Ro, EOma2RoapPdu or EOma2RoapTrigger

    Status: Proposal

-----------------------------------------------------------------------------
*/

TMessageType CRoHandler::CheckMessageTypeL
        (
        //None.
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "CheckMessageType" ) ) );
#endif

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "CheckMessageType-End" ) ) );
#endif
    TPtrC8 messageBodyPtr( iMessageBodyPtr );
    TMessageType recognizedMessageType( EOma1Ro );
    if ( iMessageBodyPtr.FindF( KWbxmlRoapTriggerElement() ) != KErrNotFound  )
        {
        iParsedXmlTrigger = iWbxmlTriggerParser->ParseL( iMessageBodyPtr );
        messageBodyPtr.Set( *iParsedXmlTrigger );
        }
    if ( messageBodyPtr.FindF( KRoapTriggerElement() ) != KErrNotFound )
        {
        if ( messageBodyPtr.FindF( KRoapTriggerRoAcquisition() ) != KErrNotFound )
            {
            recognizedMessageType = EOma2RoapTriggerRoAcquisition;
            }
        else if ( messageBodyPtr.FindF( KRoapTriggerMeteringReport() ) != KErrNotFound )
            {
            recognizedMessageType = EOma2RoapTriggerMetering;
            }
        else
            {
            recognizedMessageType = EOma2RoapTrigger;
            }

        }
    if ( iMessageBodyPtr.FindF( KRoapRoPduElement() ) != KErrNotFound )
        {
        recognizedMessageType = EOma2RoapPdu;
        }
    delete iParsedXmlTrigger;
    iParsedXmlTrigger = NULL;
    return recognizedMessageType;
    }


/*
-----------------------------------------------------------------------------

    Method: SetEntryL

    Description: Set up current message entry.

    Note: It can be useful to remember the original entry id for
          error handling.

    Status: Proposal

-----------------------------------------------------------------------------
*/

void  CRoHandler::SetEntryL
        (
        TMsvId aEntryId //an entry in the Message Server index
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "SetEntryL" ) ) );
#endif

    // Get the server entry from our session
    CMsvEntry* entry( iSession->GetEntryL( aEntryId ) );
    CleanupStack::PushL( entry );

    // Check if our mtm is different from the mtm set to our entry
    if ( !iMtm || entry->Entry().iMtm != ( iMtm->Entry() ).Entry().iMtm )
        {
        // If so, we delete the old...
        delete iMtm;
        iMtm = NULL;
        // ...and get a new one from the MtmRegistry
        iMtm = iMtmReg->NewMtmL( entry->Entry().iMtm );
        }
    // Set our entry as current.
    iMtm->SetCurrentEntryL( entry );

    CleanupStack::Pop( entry );
    entry = NULL;

#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "SetEntryL-End" ) ) );
#endif
    }

/*
-----------------------------------------------------------------------------

    Method: MoveMessageEntryL

    Description: Moves an entry to another parent.

    Return values: TMsvId of the moved message

    Status: Proposal

-----------------------------------------------------------------------------
*/

TMsvId CRoHandler::MoveMessageEntryL
        (
        TMsvId aTarget //an entry in the Message Server index
        )
    {
#ifdef _DRM_TESTING
    TRAPD( r, WriteL( _L8( "MoveMessageEntryL" ) ) );
#endif
    TMsvEntry msvEntry( ( iMtm->Entry() ).Entry() );
    TMsvId id = msvEntry.Id();

    if ( msvEntry.Parent() != aTarget )
        {
        TMsvSelectionOrdering sort;
        sort.SetShowInvisibleEntries( ETrue );    // we want to handle also the invisible entries
        // Take a handle to the parent entry
        CMsvEntry* parentEntry(
            CMsvEntry::NewL( iMtm->Session(), msvEntry.Parent(), sort ) );
        CleanupStack::PushL( parentEntry );

        // Move original from the parent to the new location
        CMsvOperationActiveSchedulerWait* wait(
            CMsvOperationActiveSchedulerWait::NewLC() );
        CMsvOperation* op(
            parentEntry->MoveL( msvEntry.Id(), aTarget, wait->iStatus ) );
        CleanupStack::PushL( op );
        wait->Start();

        TMsvLocalOperationProgress prog = McliUtils::GetLocalProgressL( *op );
        User::LeaveIfError( prog.iError );
        id = prog.iId; // id of the moved entry
        CleanupStack::PopAndDestroy( op );
        CleanupStack::PopAndDestroy( wait );
        CleanupStack::PopAndDestroy( parentEntry );
        }
#ifdef _DRM_TESTING
    TRAP( r, WriteL( _L8( "MoveMessageEntryL-End" ) ) );
#endif
    return id;
    }

TBool CRoHandler::DoRoapL( const TDesC8& aTrigger )
    {
    TBool r( EFalse );
    LOG( _L8( "CRoHandler::DoRoapL ->" ) );
    LOGHEX( aTrigger.Ptr(), aTrigger.Length() );
    CRoapSyncWrapper *roap( CRoapSyncWrapper::NewL() );
    CleanupStack::PushL( roap );
    r = roap->HandleTriggerSilentlyL( aTrigger );
    CleanupStack::PopAndDestroy( roap );
    LOG( _L8( "CRoHandler::DoRoapL <-" ) );
    return r;
    }


void CRoHandler::HandleMeteringTriggerSilentlyL()
    {
    LOG( _L8( "CRoHandler::HandleMeteringTriggerSilentlyL ->" ) );
    LOGHEX( iMessageBodyPtr.Ptr(), iMessageBodyPtr.Length() );

    if ( iMeteringSupported )
        {
        LOG( _L8( "Handling" ) );
        CRoHandlerDMgrWrapper* dMgrWrapper( CRoHandlerDMgrWrapper::NewL() );
        CleanupStack::PushL( dMgrWrapper );
        dMgrWrapper->HandleRoapTriggerL( iMessageBodyPtr );
        CleanupStack::PopAndDestroy( dMgrWrapper );
        LOG( _L8( "Handled" ) );
        }
    else
        {
        LOG( _L8( "Metering trigger not supported --> dropping it" ) );
        }
    LOG( _L8( "CRoHandler::HandleMeteringTriggerSilentlyL <-" ) );
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

/*
   -----------------------------------------------------------------------------

Method: ImplementationGroupProxy

    Description:

    Return Value: TImplementationProxy*: Implementation table to the
                  ECOM framework

    Status: Proposal

-----------------------------------------------------------------------------
*/

EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (
        TInt& aTableCount
        )
    {
    aTableCount =
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }


//  End of File
