/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MDRMMSKOBSERVER_H
#define MDRMMSKOBSERVER_H

/**
 *  Observer interface to enable observer to verify currently displayed 
 *  middle softkey label. The inherited method should be called when 
 *  the middle softkey label is in need of updating after a navigation 
 *  event has been detected.
 */
class MDRMMSKObserver
    {

public:

    /**
     * Observer interface to verify currently displayed middle softkey label.
     */
    IMPORT_C virtual void CheckMiddleSoftkeyLabelL() = 0;

    };


#endif // MDRMMSKOBSERVER_H

// End of File
