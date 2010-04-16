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
 * Description:  Declaration of DRM Http Filter plugin
 *
 */

#ifndef __HTTPFILTERDRM_H_
#define __HTTPFILTERDRM_H_

#include <http/mhttpfilter.h>
#include <http/rhttpsession.h>
#include <http/cecomfilter.h>
#include <http/framework/httplogger.h>
#include <DRMCommon.h>

#include "HTTPFilterDRMDataSupplier.h"

#ifndef DEBUG_LOGGING
#undef _LOGGING
#endif

class RHTTPResponse;
class RHTTPTransaction;
class CDRMMessageParser;

/**
 *  major interface class for DRM HTTP Filter
 *
 *  @lib HTTPFilterDRM.lib
 *  @since 2.6
 */

class CHTTPFilterDRM : public CEComFilter, public MHTTPFilter
    {
public:

    static CEComFilter* InstallFilterL( TAny* aSession );

    virtual ~CHTTPFilterDRM();

    void DeleteDataSupplier( TInt aTransId );

public:
    // Methods from MHTTPFilterBase

    /// @see MHTTPFilterBase::MHFRunL
    virtual void MHFRunL( RHTTPTransaction aTransaction,
        const THTTPEvent& aEvent );

    /// @see MHTTPFilterBase::MHFSessionRunL
    virtual void MHFSessionRunL( const THTTPSessionEvent& aEvent );

    /// @see MHTTPFilterBase::MHFRunError
    virtual TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction,
        const THTTPEvent& aEvent );

    /// @see MHTTPFilterBase::MHFSessionRunError
    virtual TInt MHFSessionRunError( TInt aError,
        const THTTPSessionEvent& aEvent );

public:
    // Methods from MHTTPFilter

    /// @see MHTTPFilter::MHFUnload
    virtual void MHFUnload( RHTTPSession aSession, THTTPFilterHandle aHandle );

    /// @see MHTTPFilter::MHFLoad
    virtual void MHFLoad( RHTTPSession aSession, THTTPFilterHandle aHandle );

private:

    CHTTPFilterDRM( RHTTPSession aSession );

    ///ContructL also Installs the filter.
    void ConstructL( RHTTPSession aSession );

#if defined (_DEBUG) && defined (_LOGGING)
    void DumpResponseHeadersL( RHTTPResponse& aResponse );
#endif

    void CheckHeadersL( const RHTTPTransaction& aTrans );
    void ProcessBodyPartL( RHTTPTransaction& aTrans );
    TInt GetDRMTransIdx( const RHTTPTransaction& aTrans ) const;
    void Cleanup( const RHTTPTransaction& aTrans );
    void CleanupAll();

private:
    /// A count to make sure we delete at the right time.
    TInt iLoadCount;
    RStringPool iStringPool;
    RHTTPSession iSession;

    // data suppliers, one supplier per transaction
    RPointerArray<CHTTPFilterDRMDataSupplier> iDataSups;

    TInt iOffset;
    // used for log purpose
    __DECLARE_LOG
    // In order to enable x-oma-drm-separate-delivery
    TInt iXOmaHeaderVal;
    };

#endif //__HTTPFILTERDRM_H_
