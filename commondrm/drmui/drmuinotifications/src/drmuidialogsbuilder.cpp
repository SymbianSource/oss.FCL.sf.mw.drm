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
#include "drmuidialogsbuilder.h"
#include <hbdocumentloader.h>
#include "drmuidialogpluginkeys.h"
#include <drmuidialogids.h> 
#include <hbaction.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <QString>
#include <HbPopup.h>

static const QString DOCML_NO_RIGHTS_OBJECT_DIALOG_QUERY =
        ":/xml/norightsobjectquery.docml";
static const QString DOCML_SIM_NOT_ALLOWED_QUERY =
        ":/xml/simnotallowedquery.docml";
static const QString DOCML_RIGHTS_OBJECT_EXPIRED_QUERY =
        ":/xml/rightsobjectexpiredquery.docml";
static const QString DOCML_WAITING_FOR_RIGHTS_OBJECT_CONF =
        ":/xml/waitingforrightsobjconf.docml";
static const QString DOCML_COPY_OBJECTS_QUERY =
        ":/xml/copyobjectsquery.docml";
static const QString DOCML_DELETED_CONF = ":/xml/filedeletedconf.docml";
static const QString DOCML_DELETED_DRM_FILE_QUERY =
        ":/xml/deletedrmfilequery.docml";
static const QString DOCML_MOVE_OBJECTS_QUERY =
        ":/xml/moveobjectsquery.docml";
static const QString DOCML_NO_COUNT_QUERY = ":/xml/nocountquery.docml";
static const QString DOCML_NO_USAGE_QUERY = ":/xml/nousagequery.docml";
static const QString DOCML_OBJECT_LOCKED_CONF =
        ":/xml/objectlockedconf.docml";
static const QString DOCML_UNABLE_TO_COPY_CONF =
        ":/xml/unabletocopyconf.docml";
static const QString DOCML_UNABLE_TO_MOVE_CONF =
        ":/xml/unabletomoveconf.docml";
static const QString DOCML_UNSUPPORTED_CONF = ":/xml/unsupportedconf.docml";
static const QString DOCML_DEREGISTERED_CONF = ":/xml/deregisteredconf.docml";
static const QString DOCML_FILEDELETED_CONF = ":/xml/filedeletedconf.docml";
static const QString DOCML_FORWARD_PROTECTED_CONF =
        ":/xml/forwardprotectedconf.docml";
static const QString DOCML_LICENCE_EXPIRED_CONF =
        ":/xml/licenceexpiredconf.docml";
static const QString DOCML_NOT_RECOGNIZED_CONF =
        ":/xml/notrecognizedconf.docml";
static const QString DOCML_REGISTERED_CONF = ":/xml/registeredconf.docml";
static const QString DOCML_REGISTER_FAILURE_CONF =
        ":/xml/registerfailureconf.docml";
static const QString DOCML_UNABLE_TO_DEREGISTER_CONF =
        ":/xml/unabletoderegisterconf.docml";
static const QString DOCML_UNABLE_TO_SEND_CONF =
        ":/xml/unabletosendconf.docml";
static const QString DOCML_UPDATED_CONF = ":/xml/updatedconf.docml";
static const QString DOCML_UPDATE_FAILURE_CONF =
        ":/xml/updatefailureconf.docml";
static const QString DOCML_VIEW_RESTRICTION_CONF =
        ":/xml/viewrestrictionconf.docml";
static const QString DOCML_ACCOUNT_UPDATE_QUERY =
        ":/xml/accountupdatequery.docml";
static const QString DOCML_GET_LICENCE_QUERY =
        ":/xml/getnewlicencequery.docml";
static const QString DOCML_RETRY_FOR_LICENCE_QUERY =
        ":/xml/retryforlicencequery.docml";
static const QString DOCML_UNABLE_TO_SEND_QUERY =
        ":/xml/unabletosendquery.docml";
static const QString DOCML_RIGHTS_OBJECTS_RECIEVED_NOTE =
        ":/xml/rightsobjectsrecievednote.docml";
static const QString DOCML_WAITING_FOR_LICENCE_CONF =
        ":/xml/waitingforlicenceconf.docml";
static const QString DOCML_ACQUIRING_LICENCE_PROG =
        ":/xml/acquiringlicenceprog.docml";
static const QString DOCML_REGISTERING_PROG =
        ":/xml/Registeringprog.docml";
static const QString DOCML_UPDATING_PROG =
        ":/xml/updatingprog.docml";
static const QString DOCML_DEREGISTERING_PROG =
        ":/xml/deregisteringprog.docml";

// Notes have not been defined in the XLS document yet hence commented out
/*
 static const QString DOCML_NO_CONNECTION_DEFINED =
 ":/xml/noconnectiondefined.docml";
 static const QString DOCML_INVALID_OR_NO_AP = ":/xml/invalidornoap.docml";
 static const QString DOCML_OPENING_FAIL_PERM = ":/xml/openingfailperm.docml";
 */

/*!
 class Constructor
 */
DrmUiDialogsBuilder::DrmUiDialogsBuilder(const QVariantMap &parameters) :
    QObject()
    {
    constructDialog(parameters);
    }

DrmUiDialogsBuilder::~DrmUiDialogsBuilder()
    {
    delete mLoader;
    mLoader = NULL;
    }

/*!
 Get error, implementation of interface
 */
int DrmUiDialogsBuilder::deviceDialogError() const
    {
    return 0;
    }

/*!
 Set parameters, implementation of interface
 Invoked when HbDeviceDialog::update calls.
 */
bool DrmUiDialogsBuilder::setDeviceDialogParameters(
        const QVariantMap &parameters)
    {
    constructDialog(parameters);
    return true;
    }

/*!
 Close notification, implementation of interface
 */
void DrmUiDialogsBuilder::closeDeviceDialog(bool byClient)
    {
    Q_UNUSED(byClient);
    emit deviceDialogClosed();
    }
/*!
 Return display widget, implementation of interface
 */
QObject *DrmUiDialogsBuilder::signalSender() const
    {
    return const_cast<DrmUiDialogsBuilder*> (this);
    }

/*!
 Return display widget, implementation of interface
 */
HbPopup* DrmUiDialogsBuilder::deviceDialogWidget() const
    {
    return mDialog;
    }

/*!
 Construct display widget
 */
bool DrmUiDialogsBuilder::constructDialog(const QVariantMap &parameters)
    {
    if (mLoader == 0)
        {
        mLoader = new HbDocumentLoader();
        }
    bool ok = false;
    int mQueryType;
    if (parameters.contains(KDrmUIDialogId))
        {

        mQueryType = parameters.value(KDrmUIDialogId).toInt();
        //switch cases to set load docml files for the parameters
        switch (mQueryType)
            {
            case EQueryNoRightsObj:
                mLoader->load(DOCML_NO_RIGHTS_OBJECT_DIALOG_QUERY, &ok);
                break;

            case EQueryFileLockedForSim:
                mLoader->load(DOCML_SIM_NOT_ALLOWED_QUERY, &ok);
                break;

            case EQueryRightsObjExpired:
                mLoader->load(DOCML_RIGHTS_OBJECT_EXPIRED_QUERY, &ok);
                break;

            case EConfWaitingForLicence:
                mLoader->load(DOCML_WAITING_FOR_LICENCE_CONF, &ok);
                break;

            case EConfDeleted:
                mLoader->load(DOCML_DELETED_CONF, &ok);
                break;

            case EConfObjLocked:
                mLoader->load(DOCML_OBJECT_LOCKED_CONF, &ok);
                break;

            case EConfUnableToCopy:
                mLoader->load(DOCML_UNABLE_TO_COPY_CONF, &ok);
                break;
            case EConfUnableToMove:
                mLoader->load(DOCML_UNABLE_TO_MOVE_CONF, &ok);
                break;

            case EConfUnsupportedObj:
                mLoader->load(DOCML_UNSUPPORTED_CONF, &ok);
                break;

            case EQueryCopyObjects:
                mLoader->load(DOCML_COPY_OBJECTS_QUERY, &ok);
                break;

            case EQueryDeleteDRMFile:
                mLoader->load(DOCML_DELETED_DRM_FILE_QUERY, &ok);
                break;

            case EQueryMoveObjects:
                mLoader->load(DOCML_MOVE_OBJECTS_QUERY, &ok);
                break;

            case EQueryNoCount:
                mLoader->load(DOCML_NO_COUNT_QUERY, &ok);
                break;

            case EQueryNoUsage:
                mLoader->load(DOCML_NO_USAGE_QUERY, &ok);
                break;

            case EQuerySendUnprotectedObjs:
                mLoader->load(DOCML_UNABLE_TO_SEND_QUERY, &ok);
                break;

            case EQueryAccountUpdate:
                mLoader->load(DOCML_ACCOUNT_UPDATE_QUERY, &ok);
                break;

            case EQueryRetryForLicence:
                mLoader->load(DOCML_RETRY_FOR_LICENCE_QUERY, &ok);
                break;

            case EQueryGetNewLicence:
                mLoader->load(DOCML_GET_LICENCE_QUERY, &ok);
                break;

            case EConfLicenceExpired:
                mLoader->load(DOCML_LICENCE_EXPIRED_CONF, &ok);
                break;

            case EConfForwardProtected:
                mLoader->load(DOCML_FORWARD_PROTECTED_CONF, &ok);
                break;

            case EConfUnableToSend:
                mLoader->load(DOCML_UNABLE_TO_SEND_CONF, &ok);
                break;

            case EConfViewRestriction:
                mLoader->load(DOCML_VIEW_RESTRICTION_CONF, &ok);
                break;

            case EConfUnableToDeregister:
                mLoader->load(DOCML_UNABLE_TO_DEREGISTER_CONF, &ok);
                break;

            case EConfPhoneDeregistered:
                mLoader->load(DOCML_DEREGISTERED_CONF, &ok);
                break;

            case EConfRegUpdateFailure:
                mLoader->load(DOCML_UPDATE_FAILURE_CONF, &ok);
                break;

            case EConfAccountNotRecognized:
                mLoader->load(DOCML_NOT_RECOGNIZED_CONF, &ok);
                break;

            case EConfPhoneRegistered:
                mLoader->load(DOCML_REGISTERED_CONF, &ok);
                break;

            case EConfRegistrationUpdated:
                mLoader->load(DOCML_UPDATED_CONF, &ok);
                break;
                
            case EConfRegistrationFailure:
                mLoader->load(DOCML_REGISTER_FAILURE_CONF, &ok);
                break;

            case ENoteRightsObjReceived:
                mLoader->load(DOCML_RIGHTS_OBJECTS_RECIEVED_NOTE, &ok);
                break;

            case EProgDeregisteringPhone:
                mLoader->load(DOCML_DEREGISTERING_PROG, &ok);
                break;

            case EProgAcquiringLicence:
                mLoader->load(DOCML_ACQUIRING_LICENCE_PROG, &ok);
                break;

            case EProgRegisteringPhone:
                mLoader->load(DOCML_REGISTERING_PROG, &ok);
                break;
            case EProgUpdatingRegistration:
                mLoader->load(DOCML_UPDATING_PROG, &ok);
                break;
                           
            //Notes are not defined in the locatization xls hence commented out
            /*case ENoteNoConnectionDefined:
                mLoader->load(DOCML_NO_CONNECTION_DEFINED, &ok);
                break;

            case ENoteInvalidOrNoAP:
                mLoader->load(DOCML_INVALID_OR_NO_AP, &ok);
                break;

            case ENoteOpeningFailPerm:
                mLoader->load(DOCML_OPENING_FAIL_PERM, &ok);
                break;*/
            default:
                break;

            }
        if (ok)
            {
            QGraphicsWidget *widget = mLoader->findWidget(QString("dialog"));
            mDialog = qobject_cast<HbDialog*> (widget);
            switch (mQueryType)
                {
                //Cases for two push button dialogs  
                case EQueryNoRightsObj:
                case EQueryFileLockedForSim:
                case EQueryRightsObjExpired:
                case EQueryCopyObjects:
                case EQueryDeleteDRMFile:
                case EQueryMoveObjects:
                case EQueryNoCount:
                case EQueryNoUsage: 
                case EQuerySendUnprotectedObjs:
                case EQueryAccountUpdate:
                case EQueryRetryForLicence:
                case EQueryGetNewLicence:
                    connect(qobject_cast<HbAction*> (mLoader->findObject(
                            "secondary")), SIGNAL(
                            triggered()), this, SLOT(cancelDialog()));
                    connect(qobject_cast<HbAction*> (mLoader->findObject(
                            "primary")), SIGNAL(
                            triggered()), this, SLOT(confirmDialog()));
                    mDialog->setTimeout(HbPopup::NoTimeout);
                    mDialog->setDismissPolicy(HbPopup::NoDismiss);
                    mDialog->setModal(true);
                    break;
                    
                 //Cases single push button dialogs    
                case EConfWaitingForLicence:
                case EConfDeleted:
                case EConfObjLocked:
                case EConfUnableToCopy:
                case EConfUnableToMove:
                case EConfUnsupportedObj: 
                case EConfLicenceExpired:
                case EConfForwardProtected:
                case EConfUnableToSend:
                case EConfViewRestriction:
                case EConfUnableToDeregister:
                case EConfPhoneDeregistered:
                case EConfRegUpdateFailure:
                case EConfAccountNotRecognized:
                case EConfPhoneRegistered:
                case EConfRegistrationUpdated:
                case EConfRegistrationFailure:
                    connect(qobject_cast<HbAction*> (mLoader->findObject(
                            "primary")), SIGNAL(
                            triggered()), this, SLOT(confirmDialog()));
                    mDialog->setTimeout(HbPopup::NoTimeout);
                    mDialog->setDismissPolicy(HbPopup::NoDismiss);
                    mDialog->setModal(true);
                    break;

                    // Cases for progress bar or wait notes
                case EProgDeregisteringPhone:
                case EProgAcquiringLicence:
                case EProgRegisteringPhone:
                case EProgUpdatingRegistration:
                    connect(qobject_cast<HbAction*> (mLoader->findObject(
                            "primary")), SIGNAL(
                            triggered()), this, SLOT(cancelDialog()));
                    mDialog->setTimeout(HbPopup::NoTimeout);
                    mDialog->setDismissPolicy(HbPopup::NoDismiss);
                    mDialog->setModal(true);
                    break;
                    
                //DrmUi Notes
               //Notes are not defined in the locatization xls hence commented out
              // case ENoteNoConnectionDefined:
              // case ENoteInvalidOrNoAP:
              // case ENoteOpeningFailPerm:
                case ENoteRightsObjReceived:
                    mDialog->setTimeout(HbPopup::StandardTimeout);
                    mDialog->setDismissPolicy(HbPopup::NoDismiss);
                    break;
                default:
                    break;
                  
                }
            }
        else
            {
            qFatal("Unable to load %S");
            return EFalse;
            }
        }

    return ETrue;
    }
/*!
 Handle secondary push button triggered
 */
void DrmUiDialogsBuilder::cancelDialog()
    {
    QVariantMap val;
    QVariant index(ECancelled);
    val.insert("result", index);
    emit deviceDialogData(val);
    emit deviceDialogClosed();
    }
/*!
 Handle Primary push button triggered
 */
void DrmUiDialogsBuilder::confirmDialog()
    {
    QVariantMap val;
    QVariant index(EOk);
    val.insert("result", index);
    emit deviceDialogData(val);
    emit deviceDialogClosed();
    }
