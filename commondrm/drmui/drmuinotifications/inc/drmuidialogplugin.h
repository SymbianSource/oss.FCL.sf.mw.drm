/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Plugin error constants
 *
 */

#ifndef DRMUIDIALOGPLUGIN_H
#define DRMUIDIALOGPLUGIN_H

#include <QObject>
#include <QVariantMap>

#include <hbdevicedialogplugin.h>


class DrmUIDialogPlugin : public HbDeviceDialogPlugin

    {
Q_OBJECT

public:

    DrmUIDialogPlugin();
    ~DrmUIDialogPlugin();

    // from base HbDeviceDialogPluginInterface
    virtual HbDeviceDialogInterface *createDeviceDialog(
            const QString &deviceDialogType, const QVariantMap &parameters);

    // from base HbDeviceDialogPlugin
    virtual bool
            accessAllowed(const QString &deviceDialogType,
                    const QVariantMap &parameters,
                    const QVariantMap &securityInfo) const;
    virtual bool deviceDialogInfo(const QString &deviceDialogType,
            const QVariantMap &parameters, DeviceDialogInfo *info) const;
    virtual QStringList deviceDialogTypes() const;
    virtual PluginFlags pluginFlags() const;
    virtual int error() const;

private:
    Q_DISABLE_COPY(DrmUIDialogPlugin)

    };

#endif 
