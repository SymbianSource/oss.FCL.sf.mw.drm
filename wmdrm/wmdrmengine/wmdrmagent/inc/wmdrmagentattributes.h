/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WMDRMAGENTATTRIBUTES_H
#define WMDRMAGENTATTRIBUTES_H

#include <caf/caftypes.h>
#include <caf/virtualpathptr.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>

class CAsf;

using namespace ContentAccess;

/**
*  Helper class providing functionality for getting WMDRM specific attributes
*  using an ASF file and a WMDRM Porting Kit instance
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( TWmDrmAgentAttributes )
    {
public:

    static TInt GetAttributeL(
        CAsf* aAsfFile,
        TInt aAttribute,
        TInt& aValue,
        const TVirtualPathPtr& aVirtualPath,
        TBool aReusePreviousQuery = EFalse );

    static TInt GetAttributeSetL(
        CAsf* aAsfFile,
        RAttributeSet& aAttributeSet,
        const TVirtualPathPtr& aVirtualPath );

    static TInt GetStringAttributeL(
        CAsf* aAsfFile,
        TInt aAttribute,
        TDes& aValue,
        const TVirtualPathPtr& aVirtualPath,
        TBool aReusePreviousQuery = EFalse );

    static TInt GetStringAttributeSetL(
        CAsf* aAsfFile,
        RStringAttributeSet& aStringAttributeSet,
        const TVirtualPathPtr& aVirtualPath );
    };

#endif // WMDRMAGENTATTRIBUTES_H
