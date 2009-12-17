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



// INCLUDE FILES
#include <e32base.h>
#include <hash.h>
#include "Base64.h"
#include "DeviceHello.h"

using namespace Roap;

// CONSTANTS
_LIT8(KReqHeader, "<roap:deviceHello xmlns:roap=\"urn:oma:bac:dldrm:roap-1.0\"");
_LIT8(KReqVersion, "><version>");
_LIT8(KReqNonceTrigger, " triggerNonce=\"");
_LIT8(KReqVersion2, "\"><version>");
_LIT8(KReqVersionEnd, "</version>");
_LIT8(KReqDeviceIdStart, "<deviceID>");
_LIT8(KReqKeyIndentifierStart,"<keyIdentifier xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\" xsi:type=\"roap:X509SPKIHash\"><hash>");
_LIT8(KReqKeyIndentifierEnd, "</hash></keyIdentifier>");
_LIT8(KReqDeviceIdEnd,"</deviceID>");
_LIT8(KReqSupportedAlgo, "<supportedAlgorithm>");
_LIT8(KReqSupportedAlgoEnd, "</supportedAlgorithm>");
_LIT8(KReqExtensions, "<extensions>");
_LIT8(KReqExtensionsEnd, "</extensions>");
_LIT8(KReqCertificateCaching, "<extension xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
 xsi:type=\"roap:CertificateCaching\"></extension>");
_LIT8(KReqEnd, "</roap:deviceHello>");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDeviceHello::CDeviceHello
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDeviceHello::CDeviceHello():
    iTriggerNonce(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDeviceHello::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDeviceHello::ConstructL()
    {
    iVersion.SetLength(0);
    }

// -----------------------------------------------------------------------------
// CDeviceHello::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDeviceHello* CDeviceHello::NewL()
    {
    CDeviceHello* self = new( ELeave ) CDeviceHello;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CDeviceHello::~CDeviceHello()
    {
    iAlgorithms.Close();
    delete iTriggerNonce;
    iTriggerNonce = NULL;
    iDeviceIdArray.Close();
    }
    
// -----------------------------------------------------------------------------
// CDeviceHello::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CDeviceHello::MessageAsXmlL(void)
    {
    HBufC8* r = NULL;
    CBufFlat* b = NULL;
    TInt i = 0;

    b = CBufFlat::NewL(128);
    CleanupStack::PushL(b);
    BufAppendL(b, KReqHeader);   
    if (iTriggerNonce)
        {
        BufAppendL(b, KReqNonceTrigger);
        BufAppendL(b, *iTriggerNonce);
        BufAppendL(b, KReqVersion2);
        BufAppendL(b, iVersion);
        GetDeviceIdsAsXmlL( b );
        }
    else
        {
        BufAppendL(b, KReqVersion);
        BufAppendL(b, iVersion);
        GetDeviceIdsAsXmlL( b ); 
        }

    for (i = 0; i < iAlgorithms.Count(); i++)
        {
        TPtrC8 p(0, 0);
        BufAppendL(b, KReqSupportedAlgo);
        p.Set(iAlgorithms[i]);
        BufAppendL(b, p);
        BufAppendL(b, KReqSupportedAlgoEnd);
        }
        
    BufAppendL(b, KReqExtensions);
    BufAppendL(b, KReqCertificateCaching);        
    BufAppendL(b, KReqExtensionsEnd);        

    
    BufAppendL(b, KReqEnd);
        
    r = b->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy(); // b
    return r;
    }
    
// -----------------------------------------------------------------------------
// CDeviceHello::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDeviceHello::GetDeviceIdsAsXmlL(CBufFlat* aBuffer)
    {
    BufAppendL(aBuffer, KReqVersionEnd);
    
    for (TInt i = 0; i < iDeviceIdArray.Count(); i++)
        {
        HBufC8* buffer( Base64EncodeL(iDeviceIdArray[i]) );
        CleanupStack::PushL( buffer );
        BufAppendL(aBuffer, KReqDeviceIdStart);

        BufAppendL(aBuffer, KReqKeyIndentifierStart);
        BufAppendL(aBuffer, *buffer);
        BufAppendL(aBuffer, KReqKeyIndentifierEnd);
        BufAppendL(aBuffer, KReqDeviceIdEnd);
        CleanupStack::PopAndDestroy( buffer );
        }
    }

//  End of File  
