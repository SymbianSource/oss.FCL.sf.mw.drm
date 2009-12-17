/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Oma Drm 2.1 WBXML RoapTrigger to XML parser
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <xml/parserfeature.h>

#include "wbxmlroaptriggerparser.h"

// CONSTANTS
_LIT8( KWBXMLParserMimeType, "text/wbxml" );
_LIT8( KSpace, " " );
_LIT8( KLess, "<" );
_LIT8( KMore, ">" );
_LIT8( KEqual, "=" );
_LIT8( KQuote, "\"" );
_LIT8( KSlash, "/" );

const TInt KBufferGran( 512 );

// ============================ LOCAL FUNCTIONS ================================

// -----------------------------------------------------------------------------
// XmlEscapeL
// -----------------------------------------------------------------------------
//
LOCAL_C void XmlEscapeL( CBufFlat*& aTarget, const TDesC8& aSource )
    {
    _LIT8( KEscapedQuote, "&quot;" );
    _LIT8( KEscapedApostrophe, "&apos;" );
    _LIT8( KEscapedAmpersand, "&amp;" );
    _LIT8( KEscapedLessThan, "&lt;" );
    _LIT8( KEscapedGreaterThan, "&gt;");

    const TInt sourceLength( aSource.Length() );
    for ( TInt i( 0 ); i < sourceLength; ++i )
        {
        switch ( aSource[i] )
            {
        case '&':
            aTarget->InsertL( aTarget->Size(), KEscapedAmpersand() );
            break;
        case '<':
            aTarget->InsertL( aTarget->Size(), KEscapedLessThan() );
            break;
        case '>':
            aTarget->InsertL( aTarget->Size(), KEscapedGreaterThan() );
            break;
        case '\"':
            aTarget->InsertL( aTarget->Size(), KEscapedQuote() );
            break;
        case '\'':
            aTarget->InsertL( aTarget->Size(), KEscapedApostrophe() );
            break;
        default:
            aTarget->InsertL( aTarget->Size(), aSource.Mid( i, 1 ) );
            break;
            }
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::CWbxmlRoapTriggerParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
DRM::CWbxmlRoapTriggerParser::CWbxmlRoapTriggerParser()
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::ConstructL()
    {
    iParser = Xml::CParser::NewL( KWBXMLParserMimeType, *this );
    User::LeaveIfError( iParser->EnableFeature( Xml::ERawContent ) );
    iContent = CBufFlat::NewL( KBufferGran );
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CWbxmlRoapTriggerParser* DRM::CWbxmlRoapTriggerParser::NewL()
    {
    DRM::CWbxmlRoapTriggerParser* self( NewLC() );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C DRM::CWbxmlRoapTriggerParser* DRM::CWbxmlRoapTriggerParser::NewLC()
    {
    DRM::CWbxmlRoapTriggerParser* self(
        new ( ELeave ) DRM::CWbxmlRoapTriggerParser );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
EXPORT_C DRM::CWbxmlRoapTriggerParser::~CWbxmlRoapTriggerParser()
    {
    delete iParser;
    delete iContent;
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::ParseL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* DRM::CWbxmlRoapTriggerParser::ParseL(
    const TDesC8& aMessage )
    {
    TInt i( 0 );
    TInt n( 0 );

    iParser->ParseBeginL();

    while ( i < aMessage.Length() )
        {
        n = Min( aMessage.Length() - i, KBufferGran );
        iParser->ParseL( aMessage.Mid( i, n ) );
        i += n;
        }

    iParser->ParseEndL();

    return iContent->Ptr( 0 ).AllocL();
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnStartDocumentL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnStartDocumentL(
    const Xml::RDocumentParameters& /*aDocParam*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnEndDocumentL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnEndDocumentL(
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnStartElementL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnStartElementL(
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    TInt /*aErrorCode*/)
    {
    CBufFlat* escapedValueBuffer( CBufFlat::NewL( KBufferGran ) );
    CleanupStack::PushL( escapedValueBuffer );
    // <tag attribute="attribute value">
    iContent->InsertL( iContent->Size(), KLess );
    iContent->InsertL( iContent->Size(), aElement.LocalName().DesC() );

    for ( TInt i( 0 ); i < aAttributes.Count(); i++ )
        {
        iContent->InsertL( iContent->Size(), KSpace );
        iContent->InsertL( iContent->Size(),
                           aAttributes[i].Attribute().LocalName().DesC() );
        iContent->InsertL( iContent->Size(), KEqual );
        iContent->InsertL( iContent->Size(), KQuote );
        XmlEscapeL( escapedValueBuffer, aAttributes[i].Value().DesC() );
        iContent->InsertL( iContent->Size(), escapedValueBuffer->Ptr( 0 ) );
        iContent->InsertL( iContent->Size(), KQuote );
        escapedValueBuffer->Delete( 0, escapedValueBuffer->Size() );
        }

    iContent->InsertL( iContent->Size(), KMore );
    CleanupStack::PopAndDestroy( escapedValueBuffer );
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnEndElementL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnEndElementL(
    const Xml::RTagInfo& aElement,
    TInt /*aErrorCode*/)
    {
    // </tag>
    iContent->InsertL( iContent->Size(), KLess );
    iContent->InsertL( iContent->Size(), KSlash );
    iContent->InsertL( iContent->Size(), aElement.LocalName().DesC() );
    iContent->InsertL( iContent->Size(), KMore );
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnContentL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnContentL(
    const TDesC8& aBytes,
    TInt /*aErrorCode*/)
    {
    XmlEscapeL( iContent, aBytes );
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnStartPrefixMappingL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnEndPrefixMappingL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnEndPrefixMappingL(
    const RString& /*aPrefix*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnIgnorableWhiteSpaceL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnSkippedEntityL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnSkippedEntityL(
    const RString& /*aName*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnProcessingInstructionL
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/,
    const TDesC8& /*aData*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnOutOfData
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnOutOfData()
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::OnError
// -----------------------------------------------------------------------------
//
void DRM::CWbxmlRoapTriggerParser::OnError(
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// DRM::CWbxmlRoapTriggerParser::GetExtendedInterface
// -----------------------------------------------------------------------------
//
TAny* DRM::CWbxmlRoapTriggerParser::GetExtendedInterface(
    const TInt32 /*aUid*/)
    {
    return NULL;
    }

//  End of File
