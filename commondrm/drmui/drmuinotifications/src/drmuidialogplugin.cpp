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
 * Description:  DrmUIDialogPlugin class implementation.
 *
 */

#include "DrmUIDialogPlugin.h"
#include <QtPlugin>
#include <hbdevicedialog.h>
#include "drmuidialogsbuilder.h"
#include "drmuidialogpluginkeys.h"

Q_EXPORT_PLUGIN2(DrmUIDialogPlugin, DrmUIDialogPlugin)

const int NmMaxDialogCount = 10;
static const QString drmUiDialog = "com.nokia.hb.drmuidialog/1.0";

/*! 
 DrmUIDialogPlugin Constructor
 */
DrmUIDialogPlugin::DrmUIDialogPlugin()
    {

    }

/*!
 Destructor
 */
DrmUIDialogPlugin::~DrmUIDialogPlugin()
    {

    }

/*! 
 Check if client is allowed to use device dialog widget
 */
bool DrmUIDialogPlugin::accessAllowed(const QString &deviceDialogType,
        const QVariantMap &parameters, const QVariantMap &securityInfo) const
    {
    Q_UNUSED (deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
    }

/*! 
 From interface class.
 Use the dialog type in the parameter to create widget.    
 */
HbDeviceDialogInterface *DrmUIDialogPlugin::createDeviceDialog(
        const QString &deviceDialogType, const QVariantMap &parameters)
    {
    Q_UNUSED(deviceDialogType)
    return new DrmUiDialogsBuilder(parameters);
    }

/*! 
 Return information of device dialog the plugin creates
 Currently only supporting 1 device dialog type, so no need to check the type.
 */
bool DrmUIDialogPlugin::deviceDialogInfo(const QString &deviceDialogType,
        const QVariantMap &parameters, DeviceDialogInfo *info) const
    {
    Q_UNUSED(parameters)
    Q_UNUSED(deviceDialogType)
    // set return values
    info->group = GenericDeviceDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;
    return true;
    }

/*! 
 Return device dialog types this plugin implements
 Function will work fine (unchanged) when new dialogs are added.
 */
QStringList DrmUIDialogPlugin::deviceDialogTypes() const
    {
    QStringList types;
    types.append(drmUiDialog);
    return types;

    }

/*! 
 Return plugin flags
 */
HbDeviceDialogPlugin::PluginFlags DrmUIDialogPlugin::pluginFlags() const
    {
    return NoPluginFlags;
    }

/*! 
 Return last error
 */
int DrmUIDialogPlugin::error() const
    {
    return 0;
    }

