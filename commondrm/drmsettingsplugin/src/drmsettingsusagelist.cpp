/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List class for Usage Reporting checkbox
*
*/


// INCLUDE FILES
#include "drmsettingsusagelist.h"
#include "drmsettingsmodel.h"

// CONSTANTS
const TInt KDRMSettingsListGranularity( 5 );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CDRMSettingUsageList::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CDRMSettingUsageList* CDRMSettingUsageList::NewL( 
    CDRMSettingsModel* aModel )
    {
    CDRMSettingUsageList* self( 
        new( ELeave ) CDRMSettingUsageList( aModel ) );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CDRMSettingUsageList::CDRMSettingUsageList
// Default constructor.
// ----------------------------------------------------------------------------
//
CDRMSettingUsageList::CDRMSettingUsageList( CDRMSettingsModel* aModel ) 
    : CSelectionItemList( KDRMSettingsListGranularity ), iModel( aModel )
    {
	}

// ----------------------------------------------------------------------------
// CDRMSettingUsageList::ConstructL
// ----------------------------------------------------------------------------
//
void CDRMSettingUsageList::ConstructL()
    {
    for ( TInt i( 0 ); i < iModel->GetRiContextCount(); i++ )
        {
        HBufC* alias( iModel->GetSingleRIAliasL( i ) );
        CleanupStack::PushL( alias );
        TBool isAllowed( iModel->IsMeteringAllowed( i ) );
        CSelectableItem* selItem( new (ELeave) CSelectableItem( *alias, 
                                                                isAllowed ) );
        selItem->ConstructL();
        this->AppendL( selItem );
        CleanupStack::PopAndDestroy( alias );
        }
    }

// ----------------------------------------------------------------------------
// CDRMSettingUsageList::~CDRMSettingUsageList
//
// Destructor
// ----------------------------------------------------------------------------
//
CDRMSettingUsageList::~CDRMSettingUsageList()
    {
    this->ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CDRMSettingUsageList::UpdateContexts
// ----------------------------------------------------------------------------
//
void CDRMSettingUsageList::UpdateContexts()
    {
    for ( TInt i( 0 ); i < iModel->GetRiContextCount(); i++ )
        {
        CSelectableItem* selItem( this->At( i ) );
        iModel->SetMeteringStatus( i, selItem->SelectionStatus() );
        }
    }
	
//  End of File
