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


// Plugin interface


#ifndef __SISTRINGDICT00_H__
#define __SISTRINGDICT00_H__

#include <e32std.h>
#include <stringpool.h>
#include <xml/plugins/stringdictionary.h>


class CDrmRel1_0StringDict00 : public CBase, public Xml::MStringDictionary
/**
The DrmRel1_0StringDict00 class is a derived class defining the API of a single string dictionary.

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between
strings and WBXML tokens should also be maintained.

@internalTechnology
@interim This is the first drop of this component.
*/
    {
public:
    static Xml::MStringDictionary* NewL(TAny* aStringPool);
    virtual ~CDrmRel1_0StringDict00();
    void ConstructL();

    void ElementL(TInt aToken, RString& aElement) const;
    void AttributeL(TInt aToken, RString& aAttribute) const;
    void AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const;
    void AttributeValueL(TInt aToken, RString& aValue) const;

    TBool CompareThisDictionary(const RString& aDictionaryDescription) const;

    TInt SwitchCodePage(TInt aCodePage);
    void PublicIdentifier(RString& aPubId);
    void NamespaceUri(RString& aUri);

    void Release();

public:

    // CodePage tokens.
    //
    // Note: There will be correlation between these token values and their
    // strings via the CDictionaryCodePage using the RStringPool.

    enum TTagToken
        {
        // CodePage 00
        // -//OMA//DTD DRMREL 1.0//EN
        ERights = 0x05,
        EContext = 0x06,
        EVersion = 0x07,
        EUid = 0x08,
        EAgreement = 0x09,
        EAsset = 0x0a,
        EKeyInfo = 0x0b,
        EKeyValue = 0x0c,
        EPermission = 0x0d,
        EPlay = 0x0e,
        EDisplay = 0x0f,
        EExecute = 0x10,
        EPrint = 0x11,
        EConstraint = 0x12,
        ECount = 0x13,
        EDateTime = 0x14,
        EStart = 0x15,
        EEnd = 0x16,
        EInterval = 0x17
        };


    enum TAttributeToken
        {
        // CodePage 00
        // -//OMA//DTD DRMREL 1.0//EN
        EXmlnsOEx = 0x05,
        EXmlnsODd = 0x06,
        EXmlnsDs = 0x07
        };



    enum TAttributeValueToken
        {
        // CodePage 00
        // -//OMA//DTD DRMREL 1.0//EN
        EValueXmlnsOEx = 0x05,
        EValueXmlnsODd = 0x06,
        EValueXmlnsDs = 0x07,
        EOdrlEx = 0x85,
        EOdrlDd = 0x86,
        EXmlDsig = 0x87
        };



private:

    CDrmRel1_0StringDict00(RStringPool* aStringPool);

private:

/**
The StringPool for this string dictionary.
We don't own this.
*/
    RStringPool             iStringPool;

    Xml::CDictionaryCodePage*   iCodepage00Table;

    TInt                    iCodePage;

    };


const TInt iTagCodePage00[] =
    {
    CDrmRel1_0StringDict00::ERights,
    CDrmRel1_0StringDict00::EContext,
    CDrmRel1_0StringDict00::EVersion,
    CDrmRel1_0StringDict00::EUid,
    CDrmRel1_0StringDict00::EAgreement,
    CDrmRel1_0StringDict00::EAsset,
    CDrmRel1_0StringDict00::EKeyInfo,
    CDrmRel1_0StringDict00::EKeyValue,
    CDrmRel1_0StringDict00::EPermission,
    CDrmRel1_0StringDict00::EPlay,
    CDrmRel1_0StringDict00::EDisplay,
    CDrmRel1_0StringDict00::EExecute,
    CDrmRel1_0StringDict00::EPrint,
    CDrmRel1_0StringDict00::EConstraint,
    CDrmRel1_0StringDict00::ECount,
    CDrmRel1_0StringDict00::EDateTime,
    CDrmRel1_0StringDict00::EStart,
    CDrmRel1_0StringDict00::EEnd,
    CDrmRel1_0StringDict00::EInterval,
    0 // ok to end with zero as this is used by a global token
    };


const TInt iAttributeCodePage00[] =
    {
    CDrmRel1_0StringDict00::EXmlnsOEx,
    CDrmRel1_0StringDict00::EXmlnsODd,
    CDrmRel1_0StringDict00::EXmlnsDs,
    0  // ok to end with zero as this is used by a global token
    };



const TInt iAttributeValueCodePage00[] =
    {
    CDrmRel1_0StringDict00::EXmlnsOEx,
    CDrmRel1_0StringDict00::EXmlnsODd,
    CDrmRel1_0StringDict00::EXmlnsDs,
    CDrmRel1_0StringDict00::EOdrlEx,
    CDrmRel1_0StringDict00::EOdrlDd,
    CDrmRel1_0StringDict00::EXmlDsig,
    0 // ok to end with zero as this is used by a global token
    };


#endif //__SISTRINGDICT00_H__
