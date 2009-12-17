/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype definitions for DRM Rights Database
*
*/


#ifndef DRMTYPES_H
#define DRMTYPES_H

// INCLUDES

#include <caf/caf.h>

// CONSTANTS

const TUint KDCFKeySize = 16;
const TInt KDRMCounterMax = KMaxTInt32;
const TInt KWrappedMacAndRekSize = 40;
const TInt KWrappedCekSize = 24;
const TInt KCekSize = 16;
const TInt KMacAndRekSize = 32;
const TInt KRiIdSize = 20;

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS

// TYPE DEFINITIONS

typedef TUint32 TDRMUniqueID;
typedef TInt32 TDRMCounter;

// TExportMode defines value of the export mode
/*
typedef enum
    {   
    EExportNone = 0;
    EExportCopy,
    EExportMove
    } TExportMode;
*/

// Rights Object version
struct TROVersion
    {
    TUint16 iVersionMain;
    TUint16 iVersionSub;
    
    TROVersion() : iVersionMain(0),iVersionSub(0) { };
    
    };

// Rights type enumeration, determines the bits in the bitvector
typedef enum 
    {
    ERightsPlay     = 0x0001,
    ERightsDisplay  = 0x0002,
    ERightsExecute  = 0x0004,
    ERightsPrint    = 0x0008,
    ERightsExport   = 0x0010,
    ERightsTopLevel = 0x0020,
    ERightsAll      = 0x003F
    } TRightsType;

// Constraint type enumeration
typedef enum 
    { 
    EConstraintNone         = 0x00000000,
    EConstraintStartTime    = 0x00000001,
    EConstraintEndTime      = 0x00000002,
    EConstraintInterval     = 0x00000004,
    EConstraintCounter      = 0x00000008,
    EConstraintTopLevel     = 0x00000010,
    EConstraintTimedCounter = 0x00000020,
    EConstraintAccumulated  = 0x00000040,
    EConstraintIndividual   = 0x00000080,
    EConstraintSystem       = 0x00000100,
    EConstraintVendor       = 0x00000200,
    EConstraintSoftware     = 0x00000400,
    EConstraintMetering     = 0x00000800,
    EConstraintNullDrmTime  = 0x10000000
    } TConstraintType;

typedef enum
    {
    } TDrmIntents;
    
typedef enum
    {
    ENoRestriction = 0x00000000,
    ENoRingingTone = 0x00000001      
    } TInfoBits;
    
// Rights Server specific error codes    
    
enum
    {
    KErrRightsServerRiNotRegistered = -30150,
    KErrRightsServerDomainNotRegistered = -30151,
    KErrRightsServerMacFailed = -30152
    };

namespace DRMClock
    {
    // Security level of the drm clock
    typedef enum 
        {
        KSecure = 0,
        KInsecure = 1
        } ESecurityLevel;
    }
#endif      // DRMTYPES_H   
            
// End of File
