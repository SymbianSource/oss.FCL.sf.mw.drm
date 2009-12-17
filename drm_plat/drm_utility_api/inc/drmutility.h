/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Utility base class
 *
*/


#ifndef CDRMUTILITY_H
#define CDRMUTILITY_H

#include <e32base.h>
#include <f32file.h>
#include <drmutilitytypes.h>

namespace DRM
    {


    /**
     *  DRM Utility base class
     *
     *  Provides essential information about the DRM protection of a file
     *  and the name of the CAF agent to be used without using CAF.
     *
     *  @lib drmutility.lib
     *  @since S60 v5.0
     */
NONSHARABLE_CLASS( CDrmUtility ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     * Creates a new CDrmUtility -object and leaves it in the cleanup stack
     *
     * @since S60 5.0
     * @return A functional CDrmUtility -object
     * @leave System wide or DRM specific error code
     */
    IMPORT_C static CDrmUtility* NewLC();

    /**
     * Two-phased constructor.
     * Creates a new CDrmUtility -object
     *
     * @since S60 5.0
     * @return A functional CDrmUtility -object
     * @leave System wide or DRM specific error code
     */
    IMPORT_C static CDrmUtility* NewL();


    /**
     * Destructor.
     */
    virtual ~CDrmUtility();

    /**
     * Checks whether the given file belongs to a DRM protected
     * content file
     *
     * @since S60 5.0
     * @param[in]    aFileHandle Open filehandle to the file to be examined
     * @param[out]   aAgent      Name of the agent that handles the file or
     *                           KNullDesC if no agent is found
     * @param[out]   aProtected  EProtected, if the default content object is
     *                           protected
     *                           EUnprotected, if the default content object
     *                           is not protected
     *
     * @return   ETrue, if the content is handled by a DRM agent
     *           EFalse, if the content is not handled by a DRM agent
     *
     * @leave    KErrBadHandle if the file handle is invalid
     *           System wide or DRM specific error code
     */
    IMPORT_C TBool GetDrmInfoL( RFile& aFileHandle,
            TPtrC& aAgent,
            TDrmProtectionStatus& aProtected ) const;

    /**
     * Checks whether the given buffer belongs to a DRM protected
     * content file
     *
     * @since S60 5.0
     * @param[in]    aContent    Memory buffer containing the content (or part
     *                           of it)
     * @param[out]   aAgent      Name of the agent that handles the file or
     *                           KNullDesC if no agent is found
     * @param[out]   aProtected  EProtected, if the default content object is
     *                           protected
     *                           EUnprotected, if the default content object
     *                           is not protected
     *                           EUnknown, if there is not enough data to
     *                           determine the protection status
     *
     * @return   ETrue, if the content is handled by a DRM agent
     *           EFalse, if the content is not handled by a DRM agent
     *
     * @leave    KErrArgument if there is not sufficient amount of data
     *           provided to the function to determine if the buffer has drm
     *           data
     *           System wide or DRM specific error code
     */
    IMPORT_C TBool GetDrmInfoL( const TDesC8& aContent,
            TPtrC& aAgent,
            TDrmProtectionStatus& aProtected) const;

    /**
     * Checks whether the given file is a drm content file and if it's default
     * content object is protected or not
     *
     * @since S60 5.0
     * @param[in]    aFileHandle Open filehandle to the file to be examined
     *
     * @return   ETrue, if the default content object is a drm content and
     *           is protected otherwise EFalse
     *
     * @leave    KErrBadHandle if the file handle is invalid
     *           System wide or DRM specific error code
     */
    IMPORT_C TBool IsProtectedL( RFile& aFileHandle ) const;


    /**
     * Checks whether the given buffer is a drm content and if it's default
     * content object is protected or not
     *
     * @since S60 5.0
     * @param[in]    aContent    Memory buffer containing the content (or part
     *                           of it)
     *
     * @return   ETrue, if the default content object is a drm content and
     *           is protected otherwise EFalse
     *
     * @leave    KErrArgument, if there is not sufficient amount of data
     *           provided to the function to determine if the buffer has drm
     *           data and the protection status
     *           System wide or DRM specific error code
     */
    IMPORT_C TBool IsProtectedL( const TDesC8& aContent ) const;


    /**
     * Checks what agent handles the given drm content file
     *
     * @since S60 5.0
     * @param[in]    aFileHandle Open filehandle to the file to be examined
     * @param[out]   aAgent      Name of the agent that handles the file
     *
     * @return   ETrue, if the content is handled by a DRM agent
     *           EFalse, if the content is not handled by a DRM agent
     *
     * @leave    KErrBadHandle if the file handle is invalid
     *           System wide or DRM specific error code
     */
    IMPORT_C TBool GetAgentL( RFile& aFileHandle,
            TPtrC& aAgent ) const;

    /**
     * Checks what agent handles the given drm content buffer
     *
     * @since S60 5.0
     * @param[in]    aContent    Memory buffer containg the file (or part of it)
     * @param[out]   aAgent      Name of the agent that handles the file
     *
     * @return   ETrue, if the content is handled by a DRM agent
     *           EFalse, if the content is not handled by a DRM agent
     *
     * @leave    KErrArgument, if there is not sufficient amount of data
     *           provided to the function to determine if the buffer has drm
     *           data
     *           System wide or DRM specific error code
     */
    IMPORT_C TBool GetAgentL( const TDesC8& aContent,
            TPtrC& aAgent ) const;

    /**
     * Checks if the given file handle is valid
     *
     * @since S60 5.0
     * @param[in]   aFileHandle Filehandle to be examined
     *
     * @return   None
     *
     * @leave KErrBadHandle if the filehandle is invalid
     */
    IMPORT_C void CheckFileHandleL( RFile& aFileHandle ) const;

private:
    /**
     * C++ default constructor.
     */
    CDrmUtility();

    // Oma based extensions    
    void FetchOmaBasedInfoL();
    
    void ConstructL();
      
    HBufC* iOmaBasedAgentName;
    HBufC8* iOmaBasedMimeType;

    };
    }


#endif // CDRMUTILITY_H
