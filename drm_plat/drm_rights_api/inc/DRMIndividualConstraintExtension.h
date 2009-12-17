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
* Description:  Defines an interface to extend DRM to support and validate non 
*                standard DRM individual constraints (i.e. constraints other than 
*                "IMSI:" or "WIM:")
*
*/


#ifndef __DRMINDIVIDUALCONSTRAINTEXTENSION_H__
#define __DRMINDIVIDUALCONSTRAINTEXTENSION_H__

// INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT(KDRMIndividualConstraintExtensionDll,"z:\\sys\\bin\\DRMIndividualConstraintExtension.dll");

// FORWARD DECLARATIONS
class RMobilePhone;

// CLASS DECLARATION

/**
*  Defines an interface to extend DRM to support and validate non standard DRM 
*  individual constraints (i.e. constraints other than "IMSI:" or "WIM:")
*
*  Must be implemented within a DLL named KDRMIndividualConstraintExtensionDll 
*  which exports a non leaving factory method at ordinal 1 returning 
*  MDRMIndividualConstraintExtension* or NULL
*
*  The DRM engine will only load and use this DLL if it exists.
*
*  @since Series60_3.1
*/
class MDRMIndividualConstraintExtension
    {
    public: // New functions

        /**
        * The method should be coded to append additional valid individual
        * constraint strings to the supplied array.
        * 
        * Constraints must be of the format "<alpha string>:<digit string>"
        *
        * @since Series60_3.1
        * @param aConstraint array of individual constraint strings
        * @param aMobilePhone optional connected session to RMobilePhone, if
        *        not supplied then a new session will be created
        * @return void
        */
        virtual void AppendConstraintsL(RPointerArray<HBufC8>& aConstraint,
            RMobilePhone* aMobilePhone = NULL) =0;

        /**
        * This method should be coded to Release any resources. CBase derived classes
        * should call delete this.
        * @since Series60_3.1
        * @return void
        */
        virtual void Release() =0;
    };

#endif //__DRMINDIVIDUALCONSTRAINTEXTENSION_H__

// End of File
