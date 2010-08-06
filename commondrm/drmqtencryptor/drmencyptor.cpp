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
* Description:  DRM Encryption tool.
*
*/

#include "drmencryptor.h"
//#include "ui_dialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QMenu>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QPalette>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>

#include <Oma1DcfCreator.h>
#include <s32file.h>
#include <DRMMessageParser.h>
#include <DcfRep.h>
#include <DcfEntry.h>
#include <DRMRights.h>
//#include <drmbrowserlauncher.h>

#include "drmserviceapi.h"
#include "DRMTypes.h"

const TInt KBufferSize = 20000;

DRMEncryptor::DRMEncryptor()
{   
    QWidget *widget = new QWidget();
    setCentralWidget(widget);
    
    setBackgroundRole( QPalette::Window );

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignCenter);
   
    QLabel *mainWindowLabel = new QLabel("DRM Encryptor");
    layout->addWidget(mainWindowLabel);
    
    //layout->addStretch();

    //ui->setupUi(this);
    //iOptionsMenu = menuBar()->addMenu(tr("&Options"));
    
    // Create menu options
    iSetDRMClockAct = new QAction(tr("&Set DRM Clock"), this);
    iSetDRMClockAct->setStatusTip(tr("Set DRM secure clock time."));
    
    iGetDRMClockAct = new QAction(tr("&Get DRM Clock Time"), this);
    iGetDRMClockAct->setStatusTip(tr("Get DRM secure clock time."));
    
    iEncryptAct = new QAction(tr("&Encrypt"), this);
    iEncryptAct->setStatusTip(tr("Encrypt"));
    
    iLaunchBrowserAct = new QAction(tr("&Launch Browser"), this);
    iLaunchBrowserAct->setStatusTip(tr("Launch Browser"));
        
    iDeleteWMDRMDBAct = new QAction(tr("&Delete WMDRM DB"), this);
    iDeleteWMDRMDBAct->setStatusTip(tr("Delete WMDRM Database rights."));
    
    iDeleteOMADRMDBAct = new QAction(tr("&Delete OMADRM DB"), this);
    iDeleteOMADRMDBAct->setStatusTip(tr("Delete OMADRM Database rights."));
    
    menuBar()->addAction(iSetDRMClockAct);
    menuBar()->addAction(iGetDRMClockAct);
    menuBar()->addAction(iEncryptAct);
    menuBar()->addAction(iDeleteWMDRMDBAct);
    menuBar()->addAction(iDeleteOMADRMDBAct);
    menuBar()->addAction(iLaunchBrowserAct);
    
    connect(iSetDRMClockAct, SIGNAL(triggered()), this, SLOT(setDRMClock()));
    connect(iGetDRMClockAct, SIGNAL(triggered()), this, SLOT(getDRMClock()));
    connect(iEncryptAct, SIGNAL(triggered()), this, SLOT(startEncrypt()));
    connect(iDeleteWMDRMDBAct, SIGNAL(triggered()), this, SLOT(deleteWmDrmDB()));
    connect(iDeleteOMADRMDBAct, SIGNAL(triggered()), this, SLOT(deleteOmaDrmDB()));
    connect(iLaunchBrowserAct, SIGNAL(triggered()), this, SLOT(launchBrowser()));
    
    //setDRMClockButton = new QPushButton("Set DRM Clock", this);
    //connect(setDRMClockButton, SIGNAL(clicked()), this, SLOT(setDRMClock()));
    //setDRMClockButton->show();
    
/*  ui->pushButton->setEnabled(false);

    ui->lineEdit->setValidator(new QDoubleValidator(this));
    ui->lineEdit_2->setValidator(new QDoubleValidator(this));
*/
    //connect(ui->mainMenu,SIGNAL(onChanged()),this,SLOT(onLaske()));
    //connect(ui->lineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
    //connect(ui->mainMenu,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));

    widget->setLayout(layout);
}

DRMEncryptor::~DRMEncryptor()
{
    //delete ui;
}

void DRMEncryptor::setDRMClock()
{
    DateTimeDialog *dateTimeDialog = new DateTimeDialog();
    dateTimeDialog->show();
}

void DRMEncryptor::getDRMClock()
{
	TTime time;
    TInt timeZone;
    TDateTime date;
    DRMClock::ESecurityLevel level;
    TInt error( KErrNone );
    TBuf< 80 > buf;
    
    DRM::CDrmServiceApi* service = NULL;
    
    TRAPD(err, service = DRM::CDrmServiceApi::NewL());
    if(err!=KErrNone)
    	{
    	QMessageBox::information(this, tr("DRM Clock"),tr("Error creating CDrmServiceApi."));
    	buf.AppendNum(error);
        QString string((QChar*)buf.Ptr(),buf.Length());
        QMessageBox::information(this, tr("DRM Clock"),string);
    	delete service; // service
    	return;
    	}
    
    error = service->GetSecureTime( time, timeZone, level );
    if(!error==KErrNone)
        {
        QMessageBox::information(this, tr("DRM Clock"),tr("Error getting secure time."));
        
        buf.AppendNum(error);
        QString string((QChar*)buf.Ptr(),buf.Length());
        QMessageBox::information(this, tr("DRM Clock"),string);
        delete service;
        return;
        }
    
    date = time.DateTime();
    
    if(level == DRMClock::KSecure)
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
    
    QGridLayout *layout = new QGridLayout;
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
        
    
    QString datetimeString((QChar*)buf.Ptr(),buf.Length());
    
    QMessageBox *drmClockTime = new QMessageBox(this);
    drmClockTime->setWindowTitle(tr("DRM Clock"));
    drmClockTime->setText(datetimeString);
    
    layout->addWidget(drmClockTime);
    drmClockTime->show();
    
}

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

void DRMEncryptor::ProcessMessageL(const TDesC& aFile, const TDesC& aOutput)
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

void DRMEncryptor::startEncrypt()
    {
        TUint result = 0;
        TUint aEncryptedCount = 0;
        TUint aRightsCount = 0;
        TUint aMessagesProcessed = 0;
        TBuf<128> buffer;
        
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
        
        QGridLayout *layout = new QGridLayout;
        layout->setAlignment(Qt::AlignCenter);
        setLayout(layout);
               
        QString encryptString((QChar*)buffer.Ptr(),buffer.Length());
        QMessageBox *encrpytMessage = new QMessageBox(this);
        encrpytMessage->setWindowTitle(tr("Encrypt"));
        encrpytMessage->setText(encryptString);
          
        layout->addWidget(encrpytMessage);
        encrpytMessage->show();
           
    }

TUint DRMEncryptor::EncryptL(TUint& aEncryptedCount, TUint& aRightsCount, TUint& aMessagesProcessed)
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

void DRMEncryptor::DeleteWmDrmRdbL()
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

void DRMEncryptor::deleteWmDrmDB()
    {
    TRAPD(err, DeleteWmDrmRdbL());
    if(!err)
        {
        QMessageBox::information(this, tr("WMDRM DB"),tr("DRM rights deleted."));
        }
    else
        {
        QMessageBox::information(this, tr("WMDRM DB"),tr("Error deleting WMDRM rights."));
        }
    }

void DRMEncryptor::deleteOmaDrmDB()
    {
    RDRMRightsClient client;
    User::LeaveIfError(client.Connect());
    client.DeleteAll();
    client.Close();
    }



void DRMEncryptor::launchBrowser()
    {
    /*_LIT( KTestDrmFile, "c:\\data\\others\\sd_water003.dcf" );
    TInt urlLength = 256;
    TFileName fileName(KTestDrmFile);
    RFile file;
    RFs fs;
    
    User::LeaveIfError(fs.Connect());
    TInt result = 0;
    
    result = file.Open(fs, fileName, EFileRead | EFileShareReadersOrWriters);    
    
    CData* content = CData::NewLC( file, KDefaultContentObject, EPeek );
    
    HBufC* rightsIssuerBuf = HBufC::NewLC( urlLength ); // Content issuer max URL length
    TPtr rightsIssuer(const_cast<TUint16*>(rightsIssuerBuf->Ptr()), 0, urlLength);
    
    // Get value of rights-issuer header field
    TInt error = content->GetStringAttribute( ERightsIssuerUrl, rightsIssuer );
    rightsIssuer.TrimAll();
    
    rightsIssuer.Insert(0, _L("http://"));
    
    HBufC* finalUrl = rightsIssuer.AllocLC();
    
    DRM::CDrmBrowserLauncher::LaunchUrlL(*finalUrl);

    CleanupStack::PopAndDestroy(3); // finalUrl, rightsIssuerBuf, content;*/
    }

/*
void DRMEncryptorsetDRMClock()
{
   
  double value1 = ui->lineEdit->text().toDouble(&bOK1);
   double value2 = ui->lineEdit_2->text().toDouble(&bOK2);

   ui->lineEdit_3->setText("");

   if(bOK1 && bOK2)
   {
       if(value2 != 0)
       {
           double result = value1 / value2;
           ui->lineEdit_3->setText(QString::number(result));
       }
       else
       {
           box.setText("Et voi jakaa nollalla");
           box.exec();
       }
   }
   else
   {
       box.setText("Virheellinen syöte");
       box.exec();
   }
}

 void DRMEncryptoronDateTimeChanged(const QDateTime & datetime )
    {
    
    }
*/

void DRMEncryptor::onTextChanged(const QString &)
{
    //ui->pushButton->setEnabled(ui->lineEdit->text() != "" && ui->lineEdit_2->text() != "");
}

DateTimeDialog::DateTimeDialog()
    {
    //iLayout = new QHBoxLayout;
    QGridLayout *layout = new QGridLayout;
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
    
    iDateEdit = new QDateTimeEdit(QDate::currentDate());
    iDateEdit->setAlignment(Qt::AlignHCenter);
    iDateEdit->setDisplayFormat("yyyy.mm.dd hh.mm.ss");
    iDateEdit->setWindowTitle("Set DRM Clock yyyy.MM.dd hh.mm.ss");
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    
    layout->addWidget(iDateEdit);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);
    
    connect(okButton, SIGNAL(clicked()), this, SLOT(setDRMClock()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    
    //this->setLayout(iLayout);
    okButton->show();
    cancelButton->show();
    this->show();
    }

DateTimeDialog::~DateTimeDialog()
    {
    
    }

void DateTimeDialog::setDRMClock()
    {
    DRM::CDrmServiceApi* service = NULL;
    TBuf<5> buf;
    TRAPD(err, service = DRM::CDrmServiceApi::NewL());
    if(err!=KErrNone)
        {
        QMessageBox::information(this, tr("DRM Clock"),tr("Error creating CDrmServiceApi."));
        buf.AppendNum(err);
        QString string((QChar*)buf.Ptr(),buf.Length());
        QMessageBox::information(this, tr("DRM Clock"),string);
        delete service; // service
        return;
        }
       
    QString datetimeString = iDateEdit->dateTime().toString("yyyy.mm.dd hh.mm.ss");
    
    // datetime in format YYYYMMDD:HHMMSS for TTime
    datetimeString.remove(QChar('.'));
    datetimeString.replace(" ", ":");
    datetimeString.append(".000000");
    
    TPtrC datetimePtr(reinterpret_cast<const TText*>(datetimeString.constData()));
    HBufC *datetimeBuf = NULL;
    TBuf<5> errorBuf;
    TRAPD(errorCode, datetimeBuf = datetimePtr.AllocL());
    if(errorCode!=KErrNone)
        {
        QMessageBox::information(this, tr("DRM Clock"),tr("Error in datetimePtr.AllocL."));
        errorBuf.AppendNum(errorCode);
        QString string((QChar*)buf.Ptr(),buf.Length());
        QMessageBox::information(this, tr("DRM Clock"),string);
        delete service;
        delete datetimeBuf;
        return;
        }
        
    
    TTime inputTime(*datetimeBuf);
    TInt timezone = 0;
    delete datetimeBuf;
    
    // Get secure time from service api to get timezone
    DRMClock::ESecurityLevel level;
    TInt error( KErrNone );

    TTime currentTime;
    error = service->GetSecureTime(currentTime, timezone, level);
    
    if(error != KErrNone)
        {
        return;
        }
    
    error = service->UpdateSecureTime(inputTime, timezone);
    
    if(error != KErrNone)
        {
        return;
        }
    
    delete service;
    
    QMessageBox::information(this, tr("DRM Clock"),tr("DRM Clock Set"));
    }

