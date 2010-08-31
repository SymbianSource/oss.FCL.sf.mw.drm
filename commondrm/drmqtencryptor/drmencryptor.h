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

#ifndef DRMENCRYPTOR_H
#define DRMENCRYPTOR_H

#include <QtGui/QDialog>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QHBoxLayout>
#include <wmdrmagent.h>
#include <caf/manager.h>

/*namespace Ui
{
    class mainWindow;
    class QPushButton;
}*/

//forward declarations
class DateTimeDialog;

class DRMEncryptor : public QMainWindow
{
    Q_OBJECT

public:
    DRMEncryptor();
    ~DRMEncryptor();

private:
    void ProcessMessageL(const TDesC& aFile, const TDesC& aOutput);
    //ContentAccess::CManager* GetCafDataL( ContentAccess::TAgent& aAgent );
    void DeleteWmDrmRdbL();
            
    
signals:
    
private slots:
    void onTextChanged(const QString &);
    void setDRMClock();
    void getDRMClock();
    void startEncrypt();
    void deleteWmDrmDB();
    void deleteOmaDrmDB();
    void launchBrowser();
    TUint EncryptL(TUint& aEncryptedCount, TUint& aRightsCount, TUint& aMessagesProcessed);
    
    //void onDateTimeChanged(const QDateTime & datetime );

private:
    QMenu *iOptionsMenu;
    QPushButton *iSetDRMClockButton;
    
    // Menu options
    QAction *iSetDRMClockAct;
    QAction *iGetDRMClockAct;
    QAction *iEncryptAct;
    QAction *iDeleteWMDRMDBAct;
    QAction *iDeleteOMADRMDBAct;
    QAction *iLaunchBrowserAct;
    
    DateTimeDialog *iDateTimeDialog;
    QDateTime iDatetime;
};

class DateTimeDialog : public QDialog
{
    Q_OBJECT

public:
    DateTimeDialog();
    ~DateTimeDialog();

private slots:
    void setDRMClock();
    
private:
    QHBoxLayout *iLayout;
    QDateTimeEdit *iDateEdit;
    /*QDateTimeEdit *iDateEdit;
    QPushButton *iOkButton;
    QPushButton *iCancelButton;*/
       
};


#endif // DRMENCRYPTOR_H
