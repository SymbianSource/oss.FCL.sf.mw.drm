/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
#include "DRMEncryptorAppUi.h"
#include "DRMEncryptorContainer.h" 
#include "DRMEncryptor.hrh"
#include "Performance.h"
#include "DRMPlayServerTest.h"

#include <avkon.hrh>
#include <aknnotewrappers.h> 

extern TUint EncryptL(TUint& aEncryptedCount, TUint& aRightsCount, TUint& aMessagesProcessed);
extern TUint KeyStorage();
extern void DeleteRdbL();
extern void GetDrmClockL();
extern void SetDrmClockL();
extern TUint Bb5KeyStorage();
extern void DeleteWmDrmRdbL();
extern void BackupWmDrmDbL();
extern void RestoreWmDrmDbL();


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CDRMEncryptorAppUi::ConstructL()
// ?implementation_description
// ----------------------------------------------------------
//
void CDRMEncryptorAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new( ELeave ) CDRMEncryptorContainer;
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CDRMEncryptorAppUi::~CDRMEncryptorAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CDRMEncryptorAppUi::~CDRMEncryptorAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ----------------------------------------------------
// CDRMEncryptorAppUi::HandleCommandL( TInt aCommand )
// ?implementation_description
// ----------------------------------------------------
//
void CDRMEncryptorAppUi::HandleCommandL( TInt aCommand )
    {
    TUint result = 0;
    TUint aEncryptedCount = 0;
    TUint aRightsCount = 0;
    TUint aMessagesProcessed = 0;
    TBuf<128> buffer;
            
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EDrmEncryptorUICommand1:
            {

            result = EncryptL(aEncryptedCount, aRightsCount, aMessagesProcessed);

            buffer.Append(_L("Encryption done\n"));
            if(result < 1) 
            {
                buffer.Append(_L("No files found"));
            }
            if(aMessagesProcessed != 0) 
            {
                buffer.AppendNum(aMessagesProcessed);
                buffer.Append(_L(" messages processed\n"));
            }
            if(aRightsCount != 0) 
            {
                buffer.AppendNum(aRightsCount);
                buffer.Append(_L(" rights objects processed\n"));
            }
            if(aMessagesProcessed != 0) 
            {
                buffer.AppendNum(aEncryptedCount);
                buffer.Append(_L(" files encrypted\n"));
            }
            
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(buffer);

            break;
            }
        case EDrmEncryptorKeyCommand:
            {
            result = KeyStorage();

            if(result < 1) 
            {
                buffer.Append(_L("No key files found"));
            }
            else 
            {
                buffer.Append(_L("Keys imported\n"));
                buffer.AppendNum(result);
                buffer.Append(_L(" keys imported"));
            }
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(buffer);
            break;
            }
            case EDrmEncryptorDeleteCommand:
            {
                DeleteRdbL();
                CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
                informationNote->ExecuteLD(_L("Database deleted"));
                break;
            }
            case EDrmTimeCommand:
            {
                GetDrmClockL();
                break;
            }
            case ESetDrmTimeCommand:
            {
                SetDrmClockL();
                break;
            }
            case EDrmBb5KeyTestCommand:
            {
                result = Bb5KeyStorage();

            if(result != KErrNone) 
            {
                buffer.Append(_L("No CMLA keys available"));
                buffer.AppendNum(result);

            }
            else 
            {
                buffer.Append(_L(" CMLA data OK! \n"));
                buffer.AppendNum(result);
                buffer.Append(_L(" \n Check log for Root."));
            }
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(buffer);
            break;
            }
            case EDRMPlayServerCommand:
            {
                CDRMPlayServerTest* test = CDRMPlayServerTest::NewLC();
                TInt error = test->ExecutePlayServerTest();
                CleanupStack::PopAndDestroy( test );
                if ( error == KErrCANoRights )
                    {
                    buffer.Append(_L("Test succeeded\n"));
                    buffer.Append(_L("DRMPlayServer can't be used with WMDRM"));
                    }
                else
                    {
                    buffer.Append(_L("Test failed with error code: "));
                    buffer.AppendNum(error);
                    }
                CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
                informationNote->ExecuteLD(buffer);
                break;
            }
            case EDrmEncryptorDeleteWmDrmCommand:
            {
                DeleteWmDrmRdbL();
                CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
                informationNote->ExecuteLD(_L("WmDrm Rights Deleted"));
                break;
            }
            case EDrmEncryptorBackupWmDrmCommand:
            {
                TRAPD( error, BackupWmDrmDbL() );
                if ( !error )
                    {
                    buffer.Append(_L("WmDrm License DB backup succeeded"));
                    }
                else
                    {
                    buffer.Append(_L("WmDrm License DB backup failed"));
                    }
                CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
                informationNote->ExecuteLD(buffer);
                break;
            }
            case EDrmEncryptorRestoreWmDrmCommand:
            {
                TRAPD( error, RestoreWmDrmDbL() );
                if ( !error )
                    {
                    buffer.Append(_L("WmDrm License DB restore succeeded"));
                    }
                else
                    {
                    buffer.Append(_L("WmDrm License DB restore failed"));
                    }
                CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
                informationNote->ExecuteLD(buffer);
                break;
            }            
        default:
            TestPerformanceL(aCommand);
                        
            buffer.Append(_L("Performance test done\n"));
            buffer.Append(_L("See results from \nc:\\logs\\performance.log"));
            
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(buffer);
            
            break;
        }
    }


TBool CDRMEncryptorAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
    TFileName& /*aDocumentName*/,
    const TDesC8& /*aTail*/)
    {
    return EFalse;
    }

// End of File  
