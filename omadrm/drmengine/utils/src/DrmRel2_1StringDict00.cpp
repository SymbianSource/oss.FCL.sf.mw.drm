/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for OMA 2.1 Drm String Dictionary
*
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <xml/plugins/dictionarycodepage.h>
#include <xml/xmlframeworkerrors.h>

#include "drmrel2_1stringdict00.h"
#include "drmrel2_1stringdict00tagtable.h"
#include "drmrel2_1stringdict00attributetable.h"
#include "drmrel2_1stringdict00attributevaluetable.h"

using namespace Xml;

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::NewL
// -----------------------------------------------------------------------------
//
MStringDictionary* CDrmRel2_1StringDict00::NewL( TAny* aStringPool )
    {
    CDrmRel2_1StringDict00* self(
       new(ELeave) CDrmRel2_1StringDict00( 
                            reinterpret_cast<RStringPool*>( aStringPool ) ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return ( static_cast<MStringDictionary*>( self ) );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::CDrmRel2_1StringDict00
// -----------------------------------------------------------------------------
//
CDrmRel2_1StringDict00::CDrmRel2_1StringDict00( RStringPool* aStringPool )
    : iStringPool( *aStringPool ), iCodePage( 0 )
    {
    // do nothing;
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::ConstructL
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::ConstructL()
    {
    // we don't own this stringpool
    iStringPool.OpenL( DrmRel2_1StringDict00TagTable::Table );
    iStringPool.OpenL( DrmRel2_1StringDict00AttributeTable::Table );
    iStringPool.OpenL( DrmRel2_1StringDict00AttributeValueTable::Table );

    iCodepage00Table = CDictionaryCodePage::NewL(
        &DrmRel2_1StringDict00TagTable::Table, 
        &DrmRel2_1StringDict00AttributeTable::Table, 
        &DrmRel2_1StringDict00AttributeValueTable::Table, 
        0 ); // codepage

    // Construct the correlation mapping
    iCodepage00Table->ConstructIndexMappingL( 
                            iTagCodePage00,
                            CDictionaryCodePage::EStringTypeElement );

    iCodepage00Table->ConstructIndexMappingL( 
                            iAttributeCodePage00,
                            CDictionaryCodePage::EStringTypeAttribute );

    iCodepage00Table->ConstructIndexMappingL( 
                            iAttributeValueCodePage00,
                            CDictionaryCodePage::EStringTypeAttributeValue );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::Release
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::Release()
    {
    delete ( this );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::~CCDrmRel2_1StringDict00
// -----------------------------------------------------------------------------
//
CDrmRel2_1StringDict00::~CDrmRel2_1StringDict00()
    {
    if ( iCodepage00Table )
        {
        delete iCodepage00Table;
        }
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::ElementL
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::ElementL(
    TInt aToken, 
    RString& aElement) const
    {
    TInt index( iCodepage00Table->StringPoolIndexFromToken(
                                aToken, 
                                CDictionaryCodePage::EStringTypeElement ) );

    if ( index == KErrXmlStringPoolTableNotFound )
       {
       User::Leave( KErrXmlUnsupportedElement );
       }

    aElement = iStringPool.String(
                    index,
                    *( iCodepage00Table->StringTable( 
                                CDictionaryCodePage::EStringTypeElement ) ) );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::AttributeL
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::AttributeL(
    TInt aToken, 
    RString& aAttribute) const
    {
    TInt index( iCodepage00Table->StringPoolIndexFromToken(
                           aToken, 
                           CDictionaryCodePage::EStringTypeAttribute ) );

    if ( index == KErrXmlStringPoolTableNotFound )
        {
        User::Leave( KErrXmlUnsupportedAttribute );
        }

    aAttribute = iStringPool.String( 
                        index,
                        *(iCodepage00Table->StringTable( 
                            CDictionaryCodePage::EStringTypeAttribute ) ) );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::AttributeValuePairL
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::AttributeValuePairL(
    TInt aToken, 
    RString& aAttribute, 
    RString& aValue) const
    {
    AttributeL( aToken, aAttribute );
    AttributeValueL( aToken, aValue );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::AttributeValueL
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::AttributeValueL( 
    TInt aToken, 
    RString& aValue ) const
    {

    TInt index( iCodepage00Table->StringPoolIndexFromToken(
                    aToken, CDictionaryCodePage::EStringTypeAttributeValue ) );

    if ( index == KErrXmlStringPoolTableNotFound )
        {
        User::Leave( KErrXmlUnsupportedAttributeValue );
        }

    aValue = iStringPool.String( 
                    index,
                    *(iCodepage00Table->StringTable(
                           CDictionaryCodePage::EStringTypeAttributeValue ) ) );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::CompareThisDictionary
// -----------------------------------------------------------------------------
//
TBool CDrmRel2_1StringDict00::CompareThisDictionary(
    const RString& aDictionaryDescription ) const
    {
    // If this string dictionary has many codepages then all these comparisons 
    // should go here.Remember, the string dictionary loads up all the 
    // RStringTables into its RStringPool on construction. So if the comparison 
    // fails we do not have it.
    return ( ( aDictionaryDescription == 
                            iStringPool.String( 
                                DrmRel2_1StringDict00TagTable::EUri,
                                DrmRel2_1StringDict00TagTable::Table ) ) ||

             ( aDictionaryDescription == 
                            iStringPool.String( 
                                 DrmRel2_1StringDict00TagTable::EPublicId, 
                                 DrmRel2_1StringDict00TagTable::Table ) ) ||

            ( aDictionaryDescription == 
                            iStringPool.String(
                                DrmRel2_1StringDict00TagTable::EFormalPublicId, 
                                DrmRel2_1StringDict00TagTable::Table ) ) );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::SwitchCodePage
// -----------------------------------------------------------------------------
//
TInt CDrmRel2_1StringDict00::SwitchCodePage(TInt aCodePage)
    {
    // We only have one codepage so can't switch
    if ( aCodePage != iCodePage )
        {
        return KErrXmlMissingStringDictionary;
        }
    return iCodePage;
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::PublicIdentifier
// This method obtains the public identifier from the StringTable.
// Either the formal or non formal public id will do.
// The stringDictionary .rss files must list both these as Wbxml
// documents have one or the other.
//
// @param aPubId The public identifier for this string dictionary.
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::PublicIdentifier( RString& aPubId )
    {
    aPubId = iStringPool.String(
        DrmRel2_1StringDict00TagTable::EFormalPublicId,
        *( iCodepage00Table->StringTable( 
                                CDictionaryCodePage::EStringTypeElement ) ) );
    }

// -----------------------------------------------------------------------------
// CDrmRel2_1StringDict00::NamespaceUri
// The element types in the Device Information DTD are defined within
// a namespace associated with the Uri/Urn available from the StringTable.
// The RString need not be closed, but closing is harmless. 
//
// @param aUri The associated namespace for this string dictionary.
// -----------------------------------------------------------------------------
//
void CDrmRel2_1StringDict00::NamespaceUri( RString& aUri )
    {
    aUri = iStringPool.String(
           DrmRel2_1StringDict00TagTable::EUri,
           *( iCodepage00Table->StringTable( 
                                CDictionaryCodePage::EStringTypeElement ) ) );
    }

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = { 
                IMPLEMENTATION_PROXY_ENTRY( 0x2000B490, 
                                            CDrmRel2_1StringDict00::NewL ) };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
                  sizeof( TImplementationProxy );

    return ImplementationTable;
    }
