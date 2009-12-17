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



#ifndef ROAP_DEF_H
#define ROAP_DEF_H

#include <e32def.h>

// MIME-TYPES

// The content type for ROAP protocol data unit
_LIT8( KRoapPDUType, "application/vnd.oma.drm.roap-pdu+xml" );

// The content type for multipart-mixed 
_LIT8( KMultipartMixedType, "multipart/mixed" );

// The content type for multipart-related 
_LIT8( KMultipartRelatedType, "multipart/related" );

_LIT8( KRoapVersion, "1.0" );

_LIT8( KRoapCmlaRootHash, "GUZDJRYDWAs7KB4Wz7uz2K58WtA=" );
_LIT8( KRoapCmlaRnDRootHashOld, "lipo7D75LumzfXEnbiCIdlmHvKE=" );
_LIT8( KRoapCmlaRnDRootHash, "lrfbGaTQWkvI46UciFCI5nk1RGk=" );


LOCAL_D const TInt KRoapTransIdLength = 16;
// Amout of algorithms
LOCAL_D const TInt KRoapAlgorithmCount = 7;

_LIT(KOmaDrmOid, "2.23.43.6");
_LIT(KOmaKpOid, "2.23.43.6.1");
_LIT(KOmaKpRightsIssuerOid, "2.23.43.6.1.1");
_LIT(KOmaKpDrmAgentOid, "2.23.43.6.1.1");

// ROAP ERROR CODES

LOCAL_D const TInt KErrRoapGeneral = -30341;

LOCAL_D const TInt KErrRoapUnsupported = -30342;

LOCAL_D const TInt KErrRoapServer = -30343;

LOCAL_D const TInt KErrRoapServerFatal = -30344;

LOCAL_D const TInt KErrRoapDevice = -30345;

LOCAL_D const TInt KErrRoapInvalidDomain = -30346;

LOCAL_D const TInt KErrRoapDomainFull = -30347;

LOCAL_D const TInt KErrRoapNotRegistered = -30348;

// ROAP protocol phases

LOCAL_D const TInt KRoapProgressDelta = 1024;
LOCAL_D const TInt KRoapProgressMax = 8192;

enum
		{
		KErrRoapPermanent = 1,
		KErrRoapTemporary = 2
		};

enum
    {
    ERoapStarted = 1,
    ERoapRegistering = 2,
    ERightsAcquisition = 3,
    EDomainJoining = 4,
    EDomainLeaving = 5,
    ERoapCompleted = 6,
    ERoapError = 7
    };
    
// ROAP features
// #define _DISABLE_SIGNATURE_CHECK
// #define _DISABLE_OCSP_CHECK
// #define _DISABLE_CERT_CHECK
// #define _DISABLE_HASH_CHAIN_GENERATION // Use of Hash Chains for Domain Key Generation
#define _DISABLE_DRM_TIME_UPDATE_CHECK // Checks that the OCSP responder's cert chain is signed by CMLA

#endif // ROAP_DEF_H