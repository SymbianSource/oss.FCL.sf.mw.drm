/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Methods of class representing
*                decoded OCSP response
*
*/


#include <asn1dec.h>
#include <x509cert.h>
#include <x500dn.h>

#include "RoapOcsp.h"
#include "oids.h"
#include "certid.h"

COCSPResponse::COCSPResponse()
    {
    }

COCSPResponse::~COCSPResponse()
    {
    delete iArchiveCutoff;
    iCertInfos.ResetAndDestroy();
    }

TInt COCSPResponse::CertCount() const
    {
    return iCertInfos.Count();
    }

COCSPResponseCertInfo* COCSPResponse::CertInfo( TUint aIndex ) const
    {
    return iCertInfos[aIndex];
    }

const TPtrC8 COCSPResponse::SignedDataL() const
    {
    return iSignedData;
    }

const TPtrC8* COCSPResponse::SigningCerts() const
    {
    return iSigningCerts.Length() ? &iSigningCerts : NULL;
    }

const TPtrC8* COCSPResponse::DataElementEncoding( const TUint aIndex ) const
    {
    const TPtrC8* result( NULL );

    switch ( aIndex )
        {
        case ECertificateChain:
            result = iSigningCerts.Length() ? &iSigningCerts : NULL;
            break;
        case ENonce:
            result = iNonce.Length() ? &iNonce : NULL;
            break;
        case EResponderIDName:
            result = iResponderIDName.Length() ? &iResponderIDName : NULL;
            break;
        case EResponderIDKeyHash:
            result = iResponderIDKeyHash.Length() ? &iResponderIDKeyHash : NULL;
            break;
        default:
            return NULL;
        }

    return result;
    }

void COCSPResponse::InternalizeL( RReadStream& /* aStream */)
    {
    User::Leave( KErrNotSupported );
    }

TTime COCSPResponse::ProducedAt() const
    {
    return iProducedAt;
    }

const TTime* COCSPResponse::ArchiveCutoff() const
    {
    return iArchiveCutoff;
    }

TInt COCSPResponse::Find( const COCSPCertID& aCertID ) const
    {
    TInt count = CertCount();
    for ( TInt index = 0; index < count; ++index )
        {
        const COCSPCertID& responseCertID( *CertInfo( index )->CertID() );
        if ( responseCertID == aCertID )
            {
            return index;
            }
        }

    return KErrNotFound;
    }
