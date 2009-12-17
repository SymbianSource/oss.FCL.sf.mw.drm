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
* Description:  
*
*/



// INCLUDE FILES
#include    "DcfEntry.h"
#include    <e32std.h>
#include    <e32base.h>
// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDcfEntry::CDcfEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDcfEntry::CDcfEntry():iCid(NULL),iFileName(NULL),iPosition(0),iGroupId(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CDcfEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDcfEntry::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDcfEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDcfEntry* CDcfEntry::NewL()
    {
    CDcfEntry* self = new( ELeave ) CDcfEntry;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

    
// Destructor
EXPORT_C CDcfEntry::~CDcfEntry()
    {
    delete iGroupId;
    delete iCid;    
    delete iFileName;    
    }


// -----------------------------------------------------------------------------
// CDcfEntry::SetCidL
// Set content ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfEntry::SetCidL(const TDesC8& aCid)
    {
    if(iCid)
        {
        delete iCid; 
        iCid = NULL;   
        }
    iCid = HBufC8::NewL(aCid.Length());
    TPtr8 ptr(iCid->Des());
    ptr.Copy(aCid);
    }


// -----------------------------------------------------------------------------
// CDcfEntry::SetGroupIdL
// Set group ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfEntry::SetGroupIdL(const TDesC8& aGroupId)
    {
    if(iGroupId)
        {
        delete iGroupId;    
        iGroupId = NULL;
        }
    iGroupId = HBufC8::NewL(aGroupId.Length());
    TPtr8 ptr(iGroupId->Des());
    ptr.Copy(aGroupId);
    }

    
// -----------------------------------------------------------------------------
// CDcfEntry::SetLocationL
// Set file name and position inside the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfEntry::SetLocationL(const TDesC& aFile, const TUint16& aPos)
    {
    if(iFileName)
        {
        delete iFileName;    
        iFileName = NULL;
        }
    iFileName = HBufC::NewL(aFile.Length());
    TPtr ptr(iFileName->Des());
    ptr.Copy(aFile);
    iPosition = aPos;    
    }    
    
// -----------------------------------------------------------------------------
// CDcfEntry::FileName
// return filename
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC16& CDcfEntry::FileName() const
    {
    if (iFileName)
        {
        return *iFileName;    
        }
    else
        {
        return KNullDesC;        
        }
    }   
    
// -----------------------------------------------------------------------------
// CDcfEntry::Position
// return position inside the file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CDcfEntry::Position() const
    {
    return iPosition;
    }     

// -----------------------------------------------------------------------------
// CDcfEntry::Cid
// return content ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDcfEntry::Cid() const
    {
    if (iCid)
        {
        return *iCid;  
        }
    else
        {
        return KNullDesC8;        
        }
    
    } 
    
    
// -----------------------------------------------------------------------------
// CDcfEntry::GroupId
// return group ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDcfEntry::GroupId() const
    {
    if (iGroupId)
        {
        return *iGroupId;  
        }
    else
        {
        return KNullDesC8;        
        }
    
    } 

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
