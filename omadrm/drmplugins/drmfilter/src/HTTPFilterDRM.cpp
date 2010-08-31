/*
 * Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implementation of the DRM Filter HTTP Filter
 *
 */

#include <http/rhttptransaction.h>
#include <http/rhttpheaders.h>
#include <http/rhttpresponse.h>
#include <httpstringconstants.h>
#include <httperr.h>
#include <bautils.h>
#include <DRMMessageParser.h>

#include "HTTPFilterDRM.h"

// LOCAL CONSTANTS
_LIT8( KDRMString1, "application/vnd.oma.drm.message" );
// FL or CD

_LIT8( KDRMString3, "application/vnd.oma.drm.content" );
// SD only

_LIT8( KJADString, "text/vnd.sun.j2me.app-descriptor" );
// Jad files

_LIT8( KXOmaFieldName, "x-oma-drm-separate-delivery");
// Estimated arrival time for rights of the SD content

_LIT8( KDRMOldContentType, "x-drm-old-content-type");
// old content type header to be added

_LIT8( KHTTPFilterDRMName, "DRM");
// Name of filter

_LIT8( KCFMRingtoneStr, "ringtone");
// cfm ringtone string

_LIT8( KCFMFwdStr, "fwd");
// cfm forward string

_LIT8( KCFMValStr, "no");
// cfm negative value string

const TInt KXOmaDefaultValue = 25;
//default value for the 'x-oma-drm-separate-delivery' header

const TInt KCFMFieldMaxLength( 200 );
const TInt KBadMimeType( -12002 ); // common error code
const TInt NW_STAT_FAILURE( -20000 ); // common error code

// format for output of data/time values
#if defined (_DEBUG) && defined (_LOGGING)
_LIT(KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHTTPFilterDRM::CHTTPFilterDRM
// Constructor
// -----------------------------------------------------------------------------
//
CHTTPFilterDRM::CHTTPFilterDRM( RHTTPSession aSession ) :
    iSession( aSession )
    {
    iDataSups.Reset();
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRM::InstallFilterL
// Initialize the filter and register it to sesssion's filter collection
// -----------------------------------------------------------------------------
//
CEComFilter* CHTTPFilterDRM::InstallFilterL( TAny* aSession )
    {
    RHTTPSession* session = REINTERPRET_CAST(RHTTPSession*, aSession);
    CHTTPFilterDRM* filter = new ( ELeave ) CHTTPFilterDRM( *session );
    CleanupStack::PushL( filter );

    filter->ConstructL( *session );
    CleanupStack::Pop( filter );

    return filter;
    }

// -----------------------------------------------------------------------------
// CHTTPFilterDRM::ConstructL
// Memory and resource allocation, leaves
// -----------------------------------------------------------------------------
//
void CHTTPFilterDRM::ConstructL( RHTTPSession aSession )
    {
    iStringPool = aSession.StringPool();

    const TInt KPriorityDelta( 10 );
    // register the filter
    RStringF filterName = iStringPool.OpenFStringL( KHTTPFilterDRMName );
    CleanupClosePushL( filterName );
    iSession.FilterCollection().AddFilterL( *this,
        THTTPEvent::EGotResponseHeaders, // Any transaction event
        MHTTPFilter::EProtocolHandler + KPriorityDelta, // Priority of filter
        filterName ); // Name of filter
    iSession.FilterCollection().AddFilterL( *this,
        THTTPEvent::EGotResponseBodyData,
        MHTTPFilter::EProtocolHandler + KPriorityDelta,
        filterName );
    iSession.FilterCollection().AddFilterL( *this,
        THTTPEvent::EResponseComplete,
        MHTTPFilter::EProtocolHandler + KPriorityDelta,
        filterName );
    iSession.FilterCollection().AddFilterL( *this,
        THTTPEvent::EFailed,
        MHTTPFilter::EProtocolHandler + KPriorityDelta,
        filterName );
    iSession.FilterCollection().AddFilterL( *this,
        THTTPEvent::EClosed,
        MHTTPFilter::EProtocolHandler + KPriorityDelta,
        filterName );
    CleanupStack::PopAndDestroy( &filterName );

    // log file
    __OPEN_LOG( "DRM.LOG" );
    __LOG( _L ( "--Initialize DRM Filter---\n" ) );

    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::~CHTTPFilterDRM
// Destructor
//------------------------------------------------------------------------
//
CHTTPFilterDRM::~CHTTPFilterDRM()
    {
    // If we've been destroyed from the cleanup stack during creation
    // of the object, it might still be loaded. So check. (Normaly the
    // delete is initiated by the 'delete this' in MHFUnload)
    if ( iLoadCount )
        {
        // As we're already in a destructor, MHFUnload must not delete us again
        iLoadCount = -1;
        RStringF filterName;
        TRAPD( error, ( filterName = iStringPool.OpenFStringL( KHTTPFilterDRMName ) ) );
        if ( error == KErrNone )
            iSession.FilterCollection().RemoveFilter( filterName );
        filterName.Close();
        }

    CleanupAll();
    iDataSups.Close();
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::DeleteDataSupplier
//------------------------------------------------------------------------
//
void CHTTPFilterDRM::DeleteDataSupplier( TInt aTransId )
    {
    __LOG( _L ( "CHTTPFilterDRM::DeleteDataSupplier" ) );
    TInt idx( KErrNotFound );
    for ( TInt i = 0; idx == KErrNotFound && i < iDataSups.Count(); ++i )
        {
        if ( iDataSups[i]->GetTransId() == aTransId )
            {
            idx = i;
            }
        }

    if ( idx != KErrNotFound )
        {
        __LOG1( _L ( "CHTTPFilterDRM::DeleteDataSupplier: deleting transaction %d." ), aTransId );
        // remove the problematic data supplier
        CHTTPFilterDRMDataSupplier* sup = iDataSups[idx];
        iDataSups.Remove( idx );
        delete sup;
        sup = NULL;
        }
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::MHFLoad
// See MHTTPFilterBase::MHFRunL
//------------------------------------------------------------------------
//
void CHTTPFilterDRM::MHFLoad( RHTTPSession, THTTPFilterHandle )
    {
    __LOG( _L ( "--Load DRM Filter---\n" ) );
    ++iLoadCount;
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::MHFUnload
// See MHTTPFilterBase::MHFRunError
//------------------------------------------------------------------------
//
void CHTTPFilterDRM::MHFUnload( RHTTPSession, THTTPFilterHandle )
    {
    __LOG( _L ( "--Unload DRM Filter---\n" ) );
    if ( --iLoadCount )
        {
        return;
        }

    delete this;
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::MHFRunL
// See MHTTPFilterBase::MHFRunL
//------------------------------------------------------------------------
//
void CHTTPFilterDRM::MHFRunL( RHTTPTransaction aTransaction,
    const THTTPEvent& aEvent )
    {
    if ( aEvent.iUID != KHTTPUid )
        return;

    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            __LOG( _L ( "Event: EGotResponseHeaders\n" ) );
            CheckHeadersL( aTransaction );
            }
            break;
        case THTTPEvent::EGotResponseBodyData:
            {
            __LOG( _L ( "Event: EGotResponseBodyData\n" ) );
            TInt idx = GetDRMTransIdx( aTransaction );
            if ( idx != KErrNotFound )
                {
                // for x-oma-drm-separate-delivery purpose
                if ( iXOmaHeaderVal )
                    {
                    iDataSups[idx]->SetEstimatedArrivalTime( iXOmaHeaderVal );
                    iXOmaHeaderVal = 0;

                    //remove the DataSuuplier
                    delete iDataSups[idx];
                    iDataSups.Remove( idx );
                    }
                else
                    {
                    iDataSups[idx]->ProcessDataPartL();
                    }
                }
            }
            break;
        case THTTPEvent::EResponseComplete:
            {
            __LOG( _L ( "Event: EResponseComplete\n" ) );
            }
            break;
        case THTTPEvent::EFailed:
            {
            __LOG( _L ( "Event: EFailed\n" ) );
            Cleanup( aTransaction );
            }
            break;
        case THTTPEvent::EClosed:
            {
            __LOG( _L ( "Event: EClosed\n" ) );
            }
            break;
        default:
            {
            __LOG1( _L ( "Unknow Event: ID - %d\n" ), aEvent.iStatus );
            }
            break;
        }
    }

const static TInt KGlobalErrs[] =
    {
        NW_STAT_FAILURE,
        KBadMimeType,
        KBadMimeType,
        NW_STAT_FAILURE,
        KBadMimeType,
        KBadMimeType,
        KBadMimeType,
        KBadMimeType,
        KBadMimeType
    };

//------------------------------------------------------------------------
// CHTTPFilterDRM::MHFRunError
// See MHTTPFilterBase::MHFRunError
//------------------------------------------------------------------------
//
TInt CHTTPFilterDRM::MHFRunError( TInt aError, RHTTPTransaction aTransaction,
    const THTTPEvent& )
    {
    TInt error = 0;
    TInt error2 = KErrNone;
    // map aError to global error message

    if ( aError >= DRMCommon::EInvalidRights && aError
        <= DRMCommon::EGeneralError )
        error = KGlobalErrs[DRMCommon::EGeneralError - aError];
    else
        error = aError;

    //to map caf errors and KErrCorrupt to display
    switch ( aError )
        {
        case KErrCANotSupported:
        case KErrCANoPermission:
        case KErrCANoRights:
        case KErrCorrupt:
            {
            error = NW_STAT_FAILURE; //Unable to perform operation
            }
            break;
        case KErrArgument:
            {
            error = KBadMimeType;
            }
            break;
        case KErrCANoAgent:
        case KErrCAOutOfRange:
        case KErrCAPendingRights:
        case KErrCASizeNotDetermined:
        case KErrCANewFileHandleRequired:
            {
            error = NW_STAT_FAILURE;
            }
            break;
        default:
            {
            error = NW_STAT_FAILURE;
            }
            break;
        }

    // pass the errorcode forward
    THTTPEvent httpEvent( error );
        TRAP( error2, aTransaction.SendEventL(httpEvent, THTTPEvent::EIncoming, THTTPFilterHandle::ECurrentFilter ));

    return error2;
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::MHFSessionRunL
// See MHTTPFilterBase::MHFSessionRunL
//------------------------------------------------------------------------
//
void CHTTPFilterDRM::MHFSessionRunL( const THTTPSessionEvent& )
    {
    // do nothing
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::MHFSessionRunL
// See MHTTPFilterBase::MHFSessionRunL
//------------------------------------------------------------------------
//
TInt CHTTPFilterDRM::MHFSessionRunError( TInt aError,
    const THTTPSessionEvent& )
    {
    // session problem, need to close DRM engine
    CleanupAll();
    return aError;
    }

//------------------------------------------------------------------------
// CHTTPFilterDRM::DumpResponseHeadersL
// Dump the response headers to LOG file
//------------------------------------------------------------------------
//
#if defined (_DEBUG) && defined (_LOGGING)
void CHTTPFilterDRM::DumpResponseHeadersL( RHTTPResponse& aResponse )
    {
    __LOG( _L ( "Dump the header...\n" ) );

    RHTTPHeaders headers = aResponse.GetHeaderCollection();
    RStringPool strP = iSession.StringPool();
    THTTPHdrFieldIter it = headers.Fields();

    while ( it.AtEnd() == EFalse )
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = strP.StringF( fieldName );
        THTTPHdrVal fieldVal;
        TInt fieldParts = 0; // For the case if next the call fails.
        TRAPD( ignoreErr, fieldParts = headers.FieldPartsL( fieldNameStr ) );

        // dump the first part of  a  header field
        for ( TInt ii = 0; ii < fieldParts; ii++ )
            {
            if ( headers.GetField( fieldNameStr, ii, fieldVal ) == KErrNone )
                {
                const TDesC8& fieldNameDesC = fieldNameStr.DesC();
                __DUMPBIN( fieldNameDesC );
                __LOG( _L ( " : " ) );

                switch ( fieldVal.Type() )
                    {
                    case THTTPHdrVal::KTIntVal:
                        {
                        __LOG1( _L ( "%d" ), fieldVal.Int() );
                        }
                        break;
                    case THTTPHdrVal::KStrFVal:
                        {
                        RStringF fieldValStr = strP.StringF( fieldVal.StrF() );
                        const TDesC8& fieldValDesC = fieldValStr.DesC();
                        __DUMPBIN( fieldValDesC );
                        }
                        break;
                    case THTTPHdrVal::KStrVal:
                        {
                        RString fieldValStr = strP.String( fieldVal.Str() );
                        const TDesC8& fieldValDesC = fieldValStr.DesC();
                        __DUMPBIN( fieldValDesC );
                        }
                        break;
                    case THTTPHdrVal::KDateVal:
                        {
                        const TInt KDateTimeMaxLength( 40 );
                        TDateTime date = fieldVal.DateTime();
                        TBuf<KDateTimeMaxLength> dateTimeString;
                        TTime t( date );
                        t.FormatL( dateTimeString, KDateFormat );
                        TBuf8<KDateTimeMaxLength> dtStr;
                        dtStr.Copy( dateTimeString.Left( KDateTimeMaxLength ) );
                        __DUMPBIN( dtStr );
                        }
                        break;
                    case THTTPHdrVal::KNoType:
                    default:
                        {
                        __LOG( _L ( "Unrecognized value type.\n" ) );
                        }
                        break;
                    }

                __LOG( _L ( "\n" ) );
                }
            }

        ++it;
        }
    __LOG( _L ( "Header is Dumped already...\n" ) );

    }
#endif

//------------------------------------------------------------------------
// CHTTPFilterDRM::CheckHeadersL
// Check HTTP headers and extract MIME type
//------------------------------------------------------------------------
//
void CHTTPFilterDRM::CheckHeadersL( const RHTTPTransaction& aTrans )
    {
    // read the header data and check the MIME type here
    // check the status and body
    RHTTPResponse response = aTrans.Response();
    TInt status = response.StatusCode();

#if defined (_DEBUG) && defined (_LOGGING)
    DumpResponseHeadersL( response );
#endif

    THTTPHdrVal fieldVal;
    // check if the content type is application/vnd.oma.drm.message
    if ( ( status >= HTTPStatus::EOk ) &&
        ( status < HTTPStatus::EMultipleChoices ) &&
        ( status != HTTPStatus::ENoContent ) )
        {
        const TBuf8<sizeof( KDRMString1 )> DRMStringBuf1( KDRMString1 );
        const TBuf8<sizeof( KDRMString3 )> DRMStringBuf3( KDRMString3 );
        const TBuf8<sizeof( KDRMOldContentType )> DRMHeaderName(
            KDRMOldContentType );

        RHTTPHeaders headers = response.GetHeaderCollection();
        RStringPool strP = iSession.StringPool();
        RStringF fieldNameStr = strP.StringF( HTTP::EContentType,
            RHTTPSession::GetTable() );

        if ( headers.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
            {
            RStringF fieldValStr = strP.StringF( fieldVal.StrF() );

            RStringF DRMValue1 = strP.OpenFStringL( DRMStringBuf1 );
            CleanupClosePushL( DRMValue1 );

            RStringF DRMValue3 = strP.OpenFStringL( DRMStringBuf3 );
            CleanupClosePushL( DRMValue3 );

            RStringF DRMHeaderNameF = strP.OpenFStringL( DRMHeaderName );
            CleanupClosePushL( DRMHeaderNameF );

            // DRM message
            if ( DRMValue1 == fieldValStr )
                {
                __LOG( _L ("DRM content exists, a new dummy data supplier is created!\n" ) );

                CHTTPFilterDRMDataSupplier* drmData = 0;

                drmData = CHTTPFilterDRMDataSupplier::NewL( aTrans.Id(),
                    response.Body(), const_cast<CHTTPFilterDRM*> ( this ) );
                /** Support for Hutchinson's content protection scheme, CFM
                 *
                 */
                drmData->SetProcessedContentType( EStandardDRMContent );
                /**
                 *
                 */
                CleanupStack::PushL( drmData );
                iDataSups.AppendL ( drmData );
                CleanupStack::Pop( drmData );
                response.SetBody( *drmData );
                

                // change the mime type to "application/vnd.oma.drm.content"
                headers.RemoveField( fieldNameStr );
                headers.SetFieldL( fieldNameStr, THTTPHdrVal( DRMValue3 ) );

                // add the new header
                headers.SetFieldL( DRMHeaderNameF, THTTPHdrVal( DRMValue1 ) );

                }
            else if ( DRMValue3 == fieldValStr )
                {
                RStringF xOmaFieldName = strP.OpenFStringL( KXOmaFieldName );
                if ( headers.GetField( xOmaFieldName, 0, fieldVal )
                    == KErrNone )
                    {
                    iXOmaHeaderVal = KXOmaDefaultValue;

                    switch ( fieldVal.Type() )
                        {
                        case THTTPHdrVal::KStrFVal:
                            {
                            TLex8 lexer( fieldVal.StrF().DesC() );
                            lexer.Val( iXOmaHeaderVal );
                            }
                            break;
                        case THTTPHdrVal::KStrVal:
                            {
                            TLex8 lexer( fieldVal.Str().DesC() );
                            lexer.Val( iXOmaHeaderVal );
                            }
                            break;
                        case THTTPHdrVal::KTIntVal:
                            {
                            iXOmaHeaderVal = fieldVal.Int();
                            }
                            break;
                        }
                    }
                xOmaFieldName.Close();
                if ( iXOmaHeaderVal ) //if header has value of 0 then will be ignored
                    {
                    CHTTPFilterDRMDataSupplier* drmData = 0;
                    drmData
                        = CHTTPFilterDRMDataSupplier::NewL( aTrans.Id(),
                            response.Body(),
                            const_cast<CHTTPFilterDRM*> ( this ) );
                    CleanupStack::PushL( drmData );
                    iDataSups.AppendL ( drmData );
                    CleanupStack::Pop( drmData );
                    }

                }

            CleanupStack::PopAndDestroy( &DRMHeaderNameF );
            CleanupStack::PopAndDestroy( &DRMValue3 );
            CleanupStack::PopAndDestroy( &DRMValue1 );

            // dump the header after modification
#if defined (_DEBUG) && defined (_LOGGING)
            __LOG( _L ("--- HEADER AFTER CHANGE ---\n") );
            DumpResponseHeadersL( response );
#endif
            }

        // CFM Content protection:
        //========================

        TInt idx = GetDRMTransIdx( aTrans );
        if ( idx != KErrNotFound )
            return;

        TProcessedContentType currentContentType = EUnknownContent;

        RStringF cfmValStr = strP.OpenFStringL( KCFMValStr );
        CleanupClosePushL( cfmValStr );

        // check "fwd=no"
        RStringF cfmFwdStr = strP.OpenFStringL( KCFMFwdStr );
        CleanupClosePushL( cfmFwdStr );
        if ( headers.GetParam( fieldNameStr, cfmFwdStr, fieldVal )
            != KErrNotFound )
            {
            RStringF fieldValStr = strP.StringF( fieldVal.StrF() );
            TBufC8<KCFMFieldMaxLength> buf1 = fieldValStr.DesC();
            if ( fieldValStr == cfmValStr )
                {
                currentContentType = EHutchinsonCFMNoFirstChunk;
                }
            }
        CleanupStack::PopAndDestroy( &cfmFwdStr );

        // check "ringtone=no"
        RStringF cfmRingtoneStr = strP.OpenFStringL( KCFMRingtoneStr );
        CleanupClosePushL( cfmRingtoneStr );

        if ( headers.GetParam( fieldNameStr, cfmRingtoneStr, fieldVal )
            != KErrNotFound )
            {
            RStringF fieldValStr = strP.StringF( fieldVal.StrF() );
            TBufC8<KCFMFieldMaxLength> buf1 = fieldValStr.DesC();
            if ( fieldValStr == cfmValStr )
                {
                currentContentType
                    = EHutchinsonCFMWithRingingToneNoFirstChunk;
                }
            }
        CleanupStack::PopAndDestroy( &cfmRingtoneStr );
        CleanupStack::PopAndDestroy( &cfmValStr );

        // Do not encrypt JAD files:
        if ( headers.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
            {
            // If it is a JAD always ignore any of the above:
            const TBuf8<sizeof( KJADString )> JADStringBuf( KJADString );
            RStringF JADValue = strP.OpenFStringL( JADStringBuf );
            CleanupClosePushL( JADValue );

            RStringF jadFieldValStr = strP.StringF( fieldVal.StrF() );
            if ( JADValue == jadFieldValStr )
                {
                currentContentType = EUnknownContent;
                }
            CleanupStack::PopAndDestroy( &JADValue );
            }

        // Protect the content
        if ( currentContentType != EUnknownContent ) // content protected
            {
            CHTTPFilterDRMDataSupplier* drmData = 0;
            drmData = CHTTPFilterDRMDataSupplier::NewL( aTrans.Id(),
                response.Body(), EHutchinsonCFMWithRingingToneNoFirstChunk,
                const_cast<CHTTPFilterDRM*> ( this ) );
            drmData->SetProcessedContentType( currentContentType );

            if ( headers.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
                {
                drmData->SetContentMimeTypeL( fieldVal.StrF().DesC() );
                }
            else
                {
                delete drmData;
                User::Leave( KErrUnknown );
                }

            CleanupStack::PushL( drmData );
            iDataSups.AppendL ( drmData );
            CleanupStack::Pop( drmData );
                
            response.SetBody( *drmData );

            ///
            RStringF DRMValue1 = strP.OpenFStringL( DRMStringBuf1 );
            CleanupClosePushL( DRMValue1 );

            RStringF DRMValue3 = strP.OpenFStringL( DRMStringBuf3 );
            CleanupClosePushL( DRMValue3 );

            RStringF DRMHeaderNameF = strP.OpenFStringL( DRMHeaderName );
            CleanupClosePushL( DRMHeaderNameF );

            // change the mime type to "application/vnd.oma.drm.content"

            headers.RemoveField( fieldNameStr );
            headers.SetFieldL( fieldNameStr, THTTPHdrVal( DRMValue3 ) );

            // add the new header
            headers.SetFieldL( DRMHeaderNameF, THTTPHdrVal( DRMValue1 ) );

            CleanupStack::PopAndDestroy( &DRMHeaderNameF );
            CleanupStack::PopAndDestroy( &DRMValue3 );
            CleanupStack::PopAndDestroy( &DRMValue1 );
            }
        }
    }

//-----------------------------------------------------------------------------
// CHTTPFilterDRM::GetDRMTransIdx
// Retrieve the DRM datasupplier
//-----------------------------------------------------------------------------
//
TInt CHTTPFilterDRM::GetDRMTransIdx( const RHTTPTransaction& aTrans ) const
    {
    for ( TInt i = 0; i < iDataSups.Count(); ++i )
        {
        if ( iDataSups[i]->GetTransId() == aTrans.Id() )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

//-----------------------------------------------------------------------------
// CHTTPFilterDRM::Cleanup
// Cleanup the resource related with a transaction
//-----------------------------------------------------------------------------
//
void CHTTPFilterDRM::Cleanup( const RHTTPTransaction& aTrans )
    {
    TInt idx = GetDRMTransIdx( aTrans );
    if ( idx != KErrNotFound )
        {
        // remove the problematic data supplier
        CHTTPFilterDRMDataSupplier* sup = iDataSups[idx];
        iDataSups.Remove( idx );
        delete sup;
        sup = NULL;
        }
    }

//-----------------------------------------------------------------------------
// CHTTPFilterDRM::CleanupAll
// Cleanup all the DRM transactions, in case a session error happens or a session
// is closed.
//-----------------------------------------------------------------------------
//
void CHTTPFilterDRM::CleanupAll()
    {
    iDataSups.ResetAndDestroy();
    }
