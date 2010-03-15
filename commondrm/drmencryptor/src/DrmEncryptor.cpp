/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Encryption tool for DRM5
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <barsc.h>
#include <barsread.h>
#include <apmstd.h>
#include <e32test.h>
#include <s32strm.h>
#include <s32file.h>
#include <bacline.h>
#include <e32math.h>


#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <DRMRights.h>
#include <DcfCommon.h>
#include <DRMMessageParser.h>
#include <Oma1DcfCreator.h>
#include <DRMRightsClient.h>
#include "DRMClockClient.h"

#include <DcfRep.h>
#include <DcfEntry.h>

#include <DRMEncryptor.rsg>

#include <avkon.hrh>
#include <aknnotewrappers.h>

#include <wmdrmagent.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

_LIT( KWmdrmBd, "c:\\private\\10281e17\\[10282F1B]hds.db" );
_LIT( KWmdrmBdBackup, "e:\\[10282F1B]hds.db" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

LOCAL_C void ReadFileL(HBufC8*& aContent, const TDesC& aName, RFs& aFs)
    {
    TInt size = 0;
    RFile file;
    User::LeaveIfError(file.Open(aFs, aName, EFileRead));
    User::LeaveIfError(file.Size(size));
    aContent = HBufC8::NewLC(size);
    TPtr8 ptr(aContent->Des());
    User::LeaveIfError(file.Read(ptr, size));
    CleanupStack::Pop(); //aContent
    }

// ---------------------------------------------------------
// UpdateDCFRepositoryL()
// Update saved file to DCFRepository
// ---------------------------------------------------------
//
LOCAL_C void UpdateDCFRepositoryL( const TDesC& aFileName)
    {
    CDcfEntry* dcf( NULL );
    CDcfRep* dcfRep( NULL );

    dcf = CDcfEntry::NewL();
    CleanupStack::PushL( dcf );

    dcfRep = CDcfRep::NewL();
    CleanupStack::PushL( dcfRep );

    dcf->SetLocationL( aFileName, 0 );
    dcfRep->UpdateL( dcf );

    CleanupStack::PopAndDestroy(2); // dcf, dcfRep
    }




// ==================== TEST FUNCTIONS =====================

const TInt KBufferSize = 20000;

void ProcessMessageL(const TDesC& aFile, const TDesC& aOutput)
    {
    CDRMMessageParser* c = NULL;
    HBufC8* d = NULL;
    RFs fs;
    TPtr8 inRead(NULL, 0);
    TInt error = 1;
    __UHEAP_MARK;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    c = CDRMMessageParser::NewL();
    CleanupStack::PushL(c);

    d = HBufC8::NewLC( KBufferSize );

    RFile input;
    User::LeaveIfError(input.Open( fs, aFile, EFileRead ));
    CleanupClosePushL( input );

    RFileWriteStream output;
    output.Replace( fs, aOutput, EFileWrite );
    CleanupClosePushL( output );

    c->InitializeMessageParserL( output );

    while( error )
        {
        inRead.Set( const_cast<TUint8*>(d->Ptr()),0,KBufferSize);
        error = input.Read( inRead );

        if( error )
            {
            c->FinalizeMessageParserL();

            User::Leave( error );
            }
        else
            {
            error = inRead.Length();
            }

        if( error )
            {
            c->ProcessMessageDataL(inRead);
            }
        }

    c->FinalizeMessageParserL();


    CleanupStack::PopAndDestroy( 5 ); // fs, c, d, input, output
    UpdateDCFRepositoryL( aOutput );
    __UHEAP_MARKEND;
    }

void ProcessRightsL(const TDesC& aFile)
    {
    CDRMMessageParser* c = NULL;
    HBufC8* d = NULL;
    RFs fs;
    RPointerArray<CDRMRights> rights;

    User::LeaveIfError(fs.Connect());
    c = CDRMMessageParser::NewL();
    ReadFileL(d, aFile, fs);
    c->ProcessRightsObject(*d, rights);
    rights.ResetAndDestroy();
    delete d;
    delete c;
    fs.Close();
    }

void EncryptFileL(const TDesC& aFile, TDesC& aOutput, TInt aMultiplier)
    {
    COma1DcfCreator* c = NULL;
    CDRMRights* rights = NULL;
    TBuf8<64> mime;
    RFs fs;
    TFileName aDcfFile;
    TInt aOriginalFileNameLength(aOutput.Length() - 4);

    User::LeaveIfError(fs.Connect());
    if (aFile.Right(3).CompareF(_L("amr")) == 0) //AMR
        {
        mime.Copy(_L8("audio/amr"));
        }
    else if (aFile.Right(3).CompareF(_L("awb")) == 0) //AMR-AWB
        {
        mime.Copy(_L8("audio/amr-wb"));
        }
    else if (aFile.Right(3).CompareF(_L("mp3")) == 0) //MP3
        {
        mime.Copy(_L8("audio/mpeg"));
        }
    else if (aFile.Right(3).CompareF(_L("mp4")) == 0) //MP4
        {
        mime.Copy(_L8("audio/mp4"));
        }
    else if (aFile.Right(3).CompareF(_L("m4a")) == 0) //M4A
        {
        mime.Copy(_L8("audio/mp4"));
        }
    else if (aFile.Right(3).CompareF(_L("3gp")) == 0) //3GPP
        {
        mime.Copy(_L8("audio/3gpp"));
        }
    else if (aFile.Right(3).CompareF(_L("3g2")) == 0) //3GPP2
        {
        mime.Copy(_L8("audio/3gpp2"));
        }
    else if (aFile.Right(3).CompareF(_L("aac")) == 0) //AAC
        {
        mime.Copy(_L8("audio/aac"));
        }
    else if (aFile.Right(3).CompareF(_L("mid")) == 0) //MIDI
        {
        mime.Copy(_L8("audio/midi"));
        }
    else if (aFile.Right(5).CompareF(_L(".spmid")) == 0) //SP-MIDI
        {
        mime.Copy(_L8("audio/sp-midi"));
        }
    else if (aFile.Right(3).CompareF(_L("rmf")) == 0) //RMF
        {
        mime.Copy(_L8("audio/rmf"));
        }
    else if (aFile.Right(4).CompareF(_L("mxmf")) == 0) //Mobile-XMF
        {
        mime.Copy(_L8("audio/mobile-xmf"));
        }
    else if (aFile.Right(3).CompareF(_L("wav")) == 0) //WAV
        {
        mime.Copy(_L8("audio/x-wav"));
        }
    else if (aFile.Right(3).CompareF(_L("gif")) == 0) // GIF
        {
        mime.Copy(_L8("image/gif"));
        }
    else if (aFile.Right(3).CompareF(_L("jpg")) == 0) // JPEG
        {
        mime.Copy(_L8("image/jpeg"));
        }
    else if (aFile.Right(3).CompareF(_L("txt")) == 0) // text
        {
        mime.Copy(_L8("text/plain"));
        }
    else if (aFile.Right(3).CompareF(_L("pip")) == 0) // PIP
        {
        mime.Copy(_L8("application/x-pip"));
        }

    aDcfFile.Append(aOutput);

    for(TInt i = 0; i < aMultiplier ; ++i)
        {
        aDcfFile.Delete(aOriginalFileNameLength, aDcfFile.Length());
        aDcfFile.Append(_L("-"));
        aDcfFile.AppendNum(i);
        aDcfFile.Append(_L(".dcf"));
        c = COma1DcfCreator::NewL();
        CleanupStack::PushL(c);
        fs.Delete(aOutput);
        c->EncryptFileL(aFile, aDcfFile, mime, rights);
        UpdateDCFRepositoryL( aDcfFile );
        delete rights;
        CleanupStack::PopAndDestroy(); // c
        }
    fs.Close();

    }

TUint EncryptL(TUint& aEncryptedCount, TUint& aRightsCount, TUint& aMessagesProcessed)
    {
    TInt i;
    CDir* files;
    TFileName input;
    TFileName output;
    TUint inputNameSize = 0;
    TUint outputNameSize = 0;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    TInt aMultiplier(1);


#ifdef __WINS__
    input.Append(_L("c:\\data\\DRM\\"));
    output.Append(_L("c:\\data\\Others\\"));
#else
#ifndef RD_MULTIPLE_DRIVE

    input.Append(_L("e:\\DRM\\"));
    output.Append(_L("e:\\Others\\"));

#else //RD_MULTIPLE_DRIVE

    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, driveNumber );
    fs.DriveToChar( driveNumber, driveLetter );

    _LIT( KdrmDir, "%c:\\DRM\\" );
    input.Format( KdrmDir, (TUint)driveLetter );

    _LIT( KothersDir, "%c:\\Others\\" );
    output.Format( KothersDir, (TUint)driveLetter );

#endif
#endif

    inputNameSize = input.Length();
    outputNameSize = output.Length();


    fs.MkDir(input);
    fs.MkDir(output);

    fs.GetDir(input, KEntryAttNormal, ESortNone, files);
    for (i = 0; i < files->Count(); i++)
        {
        input.Append((*files)[i].iName);

        output.Append((*files)[i].iName);

        if (input.Right(2).CompareF(_L("dm")) == 0)
            {

            for(TInt ii = 0; ii < aMultiplier ; ++ii)
                {
                output.Delete(outputNameSize +(*files)[i].iName.Length() , output.Length()-1);
                output.Append(_L("-"));
                output.AppendNum(ii);
                output.Append(_L(".dcf"));
                ProcessMessageL(input, output);
                ++aMessagesProcessed;
                }
            }
        else if (input.Right(3).CompareF(_L("oro")) == 0 ||
                input.Right(3).CompareF(_L("drc")) == 0 ||
                input.Right(2).CompareF(_L("ro")) == 0 ||
                input.Right(2).CompareF(_L("dr")) == 0 )
            {
            for (TInt iii = 0; iii < aMultiplier; ++iii)
                {
                ProcessRightsL(input);
                ++aRightsCount;
                }
            }
        else if (input.Right(3).CompareF(_L("dcf")) != 0)
            {
            output.Append(_L(".dcf"));
            EncryptFileL(input, output, aMultiplier);
            ++aEncryptedCount;
            }

        //restore paths
        input.Delete(inputNameSize, input.Length()-1);
        output.Delete(outputNameSize, output.Length()-1);
        }

    fs.Close();

    TRequestStatus status;
    CDcfRep* rep = CDcfRep::NewL();
    CleanupStack::PushL(rep);
    rep->RefreshDcf(status);
    User::WaitForRequest( status );
    CleanupStack::PopAndDestroy( rep );

    delete files;

    return (aEncryptedCount*aMultiplier + aRightsCount + aMessagesProcessed);
    }

void DeleteRdbL()
    {
    RDRMRightsClient client;

    User::LeaveIfError(client.Connect());
    client.DeleteAll();
    client.Close();
    }




// -----------------------------------------------------------------------------
// GetCafDataL
// -----------------------------------------------------------------------------
//
ContentAccess::CManager* GetCafDataL( TAgent& aAgent )
    {
    TPtr8 ptr(NULL, 0, 0);
    RArray<TAgent> agents;
    TRequestStatus status;
    TInt i;

    CleanupClosePushL( agents );
    CManager* manager = CManager::NewLC();

    manager->ListAgentsL( agents );

    for (i = 0; i < agents.Count(); i++)
        {
        if (agents[i].Name().Compare(KWmDrmAgentName) == 0)
            {
            aAgent = agents[i];
            break;
            }
        }
    CleanupStack::Pop( manager );
    CleanupStack::PopAndDestroy(); // agents
    return manager;
    }


// -----------------------------------------------------------------------------
// DeleteWmDrmRdbL
//-----------------------------------------------------------------------------
//

void DeleteWmDrmRdbL()
    {
    // get the data part
    ContentAccess::CManager* manager = NULL;
    ContentAccess::TAgent agent;
    TPtr8 ptr(NULL, 0, 0);
    TPtrC8 ptr2;

    // Find the caf agent and create manager
    manager = GetCafDataL( agent );
    CleanupStack::PushL( manager );

    User::LeaveIfError(
        manager->AgentSpecificCommand( agent,
                                       (TInt)DRM::EWmDrmDeleteRights,
                                       ptr2,
                                       ptr) );
    CleanupStack::PopAndDestroy( manager );
    }



void GetDrmClockL()
    {
    RDRMClockClient client;

    TTime drmTime;
    TDateTime date;
    TInt aTimeZone;
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    TBuf< 80 > buf;



    User::LeaveIfError( client.Connect() );

    client.GetSecureTime(drmTime, aTimeZone, secLevel);

    client.Close();

    date = drmTime.DateTime();

    if(secLevel == DRMClock::KSecure)
        {
        _LIT(KFormatTxt,"DRMClock Time:\n%d/%d/%d\n%d:%d:%d\nNitz available");
        buf.Format( KFormatTxt,
                date.Day()+1,
                TInt(date.Month()+1),
                date.Year(),
                date.Hour(),
                date.Minute(),
                date.Second());
        }
    else
        {
        _LIT(KFormatTxt,"DRMClock Time:\n%d/%d/%d\n%d:%d:%d\nNitz unavailable");
        buf.Format( KFormatTxt,
                date.Day()+1,
                TInt(date.Month()+1),
                date.Year(),
                date.Hour(),
                date.Minute(),
                date.Second());
        }

    CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
    informationNote->ExecuteLD(buf);


    }

void SetDrmClockL()
    {
    RDRMClockClient client;

    TTime drmTime (_L("20000111:200600.000000"));
    TTime aDate (_L("20040000:"));
    TDateTime date;
    TInt aTimeZone;
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;

    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);

    client.GetSecureTime(drmTime, aTimeZone, secLevel);

    aDate = drmTime;

    CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL(aDate, drmTime);
    if ( dlg->ExecuteLD( R_DRM_TIME_QUERY ) )
        {
        TTime aTime = aDate.Int64() + drmTime.Int64();
        client.UpdateSecureTime(aTime, aTimeZone);
        CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
        informationNote->ExecuteLD(_L("DRM time changed"));
        }
    else
        {
        //User pressed cancel on confirmation screen
        CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
        informationNote->ExecuteLD(_L("DRM time not changed"));
        }
    CleanupStack::PopAndDestroy();
    }

void BackupWmDrmDbL()
    {
    RProcess process;
    TFullName name;
    TFindProcess wmDrmServerFinder( _L( "*wmdrmserver*" ) );
    if ( wmDrmServerFinder.Next( name ) == KErrNone && process.Open( name ) == KErrNone )
        {
        process.Kill( -1 );
        process.Close();
        }
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );
    User::LeaveIfError( fileMan->Copy( KWmdrmBd, KWmdrmBdBackup, CFileMan::EOverWrite ) );
    CleanupStack::PopAndDestroy( 2, &fs ); //fs, fileMan
    }

void RestoreWmDrmDbL()
    {
    RProcess process;
    TFullName name;
    TFindProcess wmDrmServerFinder( _L( "*wmdrmserver*" ) );
    if ( wmDrmServerFinder.Next( name ) == KErrNone && process.Open( name ) == KErrNone )
        {
        process.Kill( -1 );
        process.Close();
        }
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );
    User::LeaveIfError( fileMan->Copy( KWmdrmBdBackup, KWmdrmBd, CFileMan::EOverWrite ) );
    CleanupStack::PopAndDestroy( 2, &fs ); //fs, fileMan
    }

// End of File
