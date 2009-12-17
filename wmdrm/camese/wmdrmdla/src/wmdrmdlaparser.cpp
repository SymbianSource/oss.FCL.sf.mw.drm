/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the WMDRM DLA Parser
*
*/


// INCLUDE FILES
#include <xml/xmlframeworkconstants.h>
#include "wmdrmdlaparser.h"

#define _LOGGING_FILE L"wmdrmdla.txt"
#include "logfn.h"

// CONSTANTS
_LIT8( KLaInfoTag8, "lainfo" );
_LIT8( KTIDTag8, "tid" );
_LIT8( KContentUrlTag8, "contenturl" );

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::NewL
// ----------------------------------------------------------------------------
//
CWmDrmDlaParser* CWmDrmDlaParser::NewL()
    {
    LOGFN( "CWmDrmDlaParser::NewL" );
    CWmDrmDlaParser* self = new (ELeave) CWmDrmDlaParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::CWmDrmDlaParser
// ----------------------------------------------------------------------------
//
CWmDrmDlaParser::CWmDrmDlaParser()
    {
    LOGFN( "CWmDrmDlaParser::CWmDrmDlaParser" );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::~CWmDrmDlaParser
// ----------------------------------------------------------------------------
//
CWmDrmDlaParser::~CWmDrmDlaParser()
    {
    LOGFN( "CWmDrmDlaParser::~CWmDrmDlaParser" );
    delete iParser;
    delete iBuffer;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::ConstructL
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::ConstructL()
    {
    LOGFN( "CWmDrmDlaParser::ConstructL" );
    // Mime type of the parsed document
    _LIT8( KXmlMimeType, "text/xml" );
    // Construct the parser object
    iParser = Xml::CParser::NewL( KXmlMimeType, *this );
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::ProcessLicenseResponse
// ----------------------------------------------------------------------------
//
TInt CWmDrmDlaParser::ProcessLicenseResponse(
    const TDesC8& aLicenseResponse,
    HBufC8*& aTID,
    HBufC8*& aContentURL)
    {
    TInt error = KErrNone;
    
    LOGFNR( "CWmDrmDlaParser::ProcessLicenseResponse", error );
    
    // Find beginning of XML document ('<')
    TInt pos = aLicenseResponse.Locate( '<' );
    if ( pos != KErrNotFound )
      {
      iContentUrl = &aContentURL;
      iTID = &aTID;
      iErrorCode = KErrNone;
      TPtrC8 ptrUrl = aLicenseResponse.Mid( pos );
      TRAP( error, Xml::ParseL( *iParser, ptrUrl ) );
      if ( !error )
        {
        error = iErrorCode;
        }

      iTID = NULL;
      iContentUrl = NULL;

      delete iServerUrl;
      iServerUrl = NULL;
      }
    else
      {
      error = KErrCorrupt;
      }
    return error;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::GetLicenseServerURLFromDRMHeader
// ----------------------------------------------------------------------------
//
TInt CWmDrmDlaParser::GetLicenseServerURLFromDRMHeader(
    const TDesC8& aDrmHeader,
    HBufC8*& aServerURL )
    {
    TInt error( KErrNone );
    
    LOGFNR( "CWmDrmDlaParser::GetLicenseServerURLFromDRMHeader", error );
    
    // Find beginning of XML document ('<')
    TInt pos = aDrmHeader.Locate( '<' );
    if ( pos != KErrNotFound )
        {
        iServerUrl = &aServerURL;
        iErrorCode = KErrNone;
        TPtrC8 ptrUrl = aDrmHeader.Mid( pos );
        TRAP( error, Xml::ParseL( *iParser, ptrUrl ) );
        if ( !error )
            {
            error = iErrorCode;
            }

        iServerUrl = NULL;

        delete iTID;
        iTID = NULL;

        delete iContentUrl;
        iContentUrl = NULL;
        }
    else
        {
        error = KErrCorrupt;
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnStartDocumentL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnStartDocumentL(
    const Xml::RDocumentParameters& /*aDocParam*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnEndDocumentL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnStartElementL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnStartElementL(
    const Xml::RTagInfo& /*aElement*/,
    const Xml::RAttributeArray& /*aAttributes*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnEndElementL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnEndElementL(
    const Xml::RTagInfo& aElement,
    TInt aErrorCode)
    {
    User::LeaveIfError( aErrorCode );
    if ( !aElement.LocalName().DesC().CompareF( KLaInfoTag8 ) )
        {
        *iServerUrl = iBuffer;
        iBuffer = NULL;
        }
    else if ( !aElement.LocalName().DesC().CompareF( KTIDTag8 ) )
        {
        *iTID = iBuffer;
        iBuffer = NULL;
        }
    else if ( !aElement.LocalName().DesC().CompareF( KContentUrlTag8 ) )
        {
        *iContentUrl = iBuffer;
        iBuffer = NULL;
        }
    else
        {
        delete iBuffer;
        iBuffer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnContentL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnContentL( const TDesC8& aBytes, TInt /*aErrorCode*/ )
    {
    if ( !iBuffer )
        {
        iBuffer = aBytes.AllocL();
        }
    else
        {
        iBuffer = iBuffer->ReAllocL( iBuffer->Length() + aBytes.Length() );
        iBuffer->Des().Append( aBytes );
        }
 
   }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnStartPrefixMappingL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnEndPrefixMappingL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnEndPrefixMappingL(
    const RString& /*aPrefix*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnIgnorableWhiteSpaceL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnSkippedEntityL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnSkippedEntityL(
    const RString& /*aName*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnProcessingInstructionL
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/,
    const TDesC8& /*aData*/,
    TInt /*aErrorCode*/ )
    {
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::OnError
// From MContentHandler
// ----------------------------------------------------------------------------
//
void CWmDrmDlaParser::OnError( TInt aErrorCode )
    {
    iErrorCode = aErrorCode;
    }

// ----------------------------------------------------------------------------
// CWmDrmDlaParser::GetExtendedInterface
// From MContentHandler
// ----------------------------------------------------------------------------
//
TAny* CWmDrmDlaParser::GetExtendedInterface( const TInt32 /* aUid */ )
    {
    return NULL;
    }
