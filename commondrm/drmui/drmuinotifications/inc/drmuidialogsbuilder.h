/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef DRUMUIDIALOGSBUILDER_H
#define DRUMUIDIALOGSBUILDER_H

#include <QObject>
#include <QVariantMap>
#include <hbdevicedialoginterface.h>

class HbDocumentLoader;
class HbDialog;

class DrmUiDialogsBuilder : public QObject, public HbDeviceDialogInterface
    {
Q_OBJECT
public:
    DrmUiDialogsBuilder(const QVariantMap &parameters);
    ~DrmUiDialogsBuilder();

public:
    // from HbDeviceDialogInterface
    virtual bool setDeviceDialogParameters(const QVariantMap &parameters);
    virtual int deviceDialogError() const;
    virtual void closeDeviceDialog(bool byClient);
    virtual HbPopup *deviceDialogWidget() const;
    virtual QObject *signalSender() const;


public slots:
    void  cancelDialog();
    void  confirmDialog();
    
private:
    bool constructDialog(const QVariantMap &parameters);

signals:
   // signlas needed by HbDeviceDialogInterface
    void deviceDialogClosed();
    void deviceDialogData(QVariantMap data);

private:
    HbDocumentLoader *mLoader;
    HbDialog * mDialog;

    Q_DISABLE_COPY(DrmUiDialogsBuilder)
    };

#endif /* DRUMUIDIALOGSBUILDER_H */
