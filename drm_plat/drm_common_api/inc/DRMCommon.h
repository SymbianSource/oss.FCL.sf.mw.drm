/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class DRMCommon for accessing common
*                DRM functions
*
*/


#ifndef DRMCOMMON_H
#define DRMCOMMON_H

#include <e32base.h>
#include <DRMRights.h>
#include <caf/caf.h>

const TUint KDrmMaxContentInfoSize = 512;

// FORWARD DECLARATIONS
class TDataType;

// CLASS DECLARATION

/**
*  DRMCommon: provides common DRM functions.
*
*  @lib    DRMCommon.dll
*  @since  2.0
*/
class DRMCommon: public CBase
    {
public:     // Data Types
    enum
        {
        EPlay = ContentAccess::EPlay,
        EDisplay = ContentAccess::EView, 
        EExecute = ContentAccess::EExecute, 
        EPrint = ContentAccess::EPrint, 
        EInstall = ContentAccess::EInstall, 
        };

    enum TConsumeAction
        {
        EStart,
        EFinish,
        };
    
    enum
        {
        EOk = KErrNone, 
        EGeneralError = -30100, 
        EUnknownMIME = -30101, 
        EVersionNotSupported = -30102, 
        ESessionError = -30103, 
        ENoRights = KErrCANoRights, 
        ERightsDBCorrupted = -30105, 
        EUnsupported = KErrCANotSupported, 
        ERightsExpired = KErrCANoPermission, 
        EInvalidRights = -30108,
        EPaddingFailed = -30109,
        EFileError = -30110
        };
    
    enum TContentProtection
        {
        ENoDCFFile, 
        EForwardLocked, 
        ESuperDistributable
        };
    
    enum TOMALevel
        {
        EOMA_None, 
        EOMA_1_0,
        EOMA_2_0
        };
    
    enum
        {
        EForwardLock = 0x0001, 
        ECombinedDelivery = 0x0002, 
        ESeparateDelivery = 0x0004, 
        ESuperDistribution = 0x0008
        };
    
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static DRMCommon* NewL(void);
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~DRMCommon(void);
    
    public: // New functions
    
    /**
    * Version
    *
    * Returns the version of this API.
    *
    * @since  2.0
    * @return Version of the API
    */
    IMPORT_C static TVersion Version(void);
    
    /**
    * ServerVersion
    * 
    * Returns the version of the DRM Engine.
    *
    * @since  2.0
    * @return Version of the DRM Engine
    */
    IMPORT_C static TVersion ServerVersion(void);
    
    /**
    * Connect
    *
    * Connect to the DRM Engine.
    *
    * @since  2.0
    * @return EOk if the connection has been made
    */
    IMPORT_C TInt Connect(void);
    
    /**
    * Disconnect
    *
    * Disconnect the current connection.
    *
    * @since  2.0
    * @return EOk if the connection has been disconnected
    */
    IMPORT_C TInt Disconnect(void);
    
    /**
    * CheckContentRights
    * 
    * Takes a Content URI and a rights specification.
    * Checks if the specified rights can be exercised with this content 
    * by looking up the content URI in the rights database and checking
    * the stored rights.
    *
    * @since  2.0
    * @param  aContentURI: the content URI of the content to be tested
    * @param  aRightsSpec: The rights to be checked defined as a bitmask
    *         combination of EPlay, EDisplay, EExecute and EPrint
    * @return EOk if all of the given rights can be exercised, otherwise
    *         or if the content URI cannot be found, ENoRights
    */
    IMPORT_C TInt CheckContentRights(
        const TDesC8& aContentURI, 
        TUint32 aRightsSpec);
    
    /**
    * CheckFileRights
    *
    * Takes a file name and a rights specification. Opens the given file
    * and checks if the specified rights can be exercised with this file 
    * by extracting the content URI from the file, looking up the content
    * URI in the rights database and checking the stored rights.
    *
    * @since  2.0
    * @param  aFileName: the name of the file to be tested.
    * @param  aRightsSpec: The rights to be checked defined as a bitmask
    *         combination of EPlay, EDisplay, EExecute and EPrint
    * @return EOk if all of the given rights can be exercised, otherwise
    *         or if the file is not a DCF file or if the content URI cannot
    *         be found, ENoRights
    */
    IMPORT_C TInt CheckFileRights(
        const TDesC& aFileName, 
        TUint32 aRightsSpec);

    /**
    * CheckFileRights
    *
    * Takes a file name and a rights specification. Opens the given file
    * and checks if the specified rights can be exercised with this file 
    * by extracting the content URI from the file, looking up the content
    * URI in the rights database and checking the stored rights.
    *
    * @since  3.0
    * @param  aFileHandle: file handle to the file.
    * @param  aRightsSpec: The rights to be checked defined as a bitmask
    *         combination of EPlay, EDisplay, EExecute and EPrint
    * @return EOk if all of the given rights can be exercised, otherwise
    *         or if the file is not a DCF file or if the content URI cannot
    *         be found, ENoRights
    */
    IMPORT_C TInt CheckFileRights(
        RFile& aFileHandle, 
        TUint32 aRightsSpec);
    
    /**
    * IsProtectedContent
    *
    * Takes a memory buffer containing a file and checks if it is a DCF file
    * or a regular file.
    *
    * @since  2.0
    * @param  aContent: memory buffer containing a file
    * @param  aProtection: return parameter, true if the file is a DCF file
    * @return EOk if the content could be checked.
    */
    IMPORT_C TInt IsProtectedContent(
        const TDesC8& aContent, 
        TBool& aProtection);
    
    /**
    * IsProtectedFile
    *
    * Takes a file name and checks if it is a DCF file or a regular file.
    *
    * @since  2.0
    * @param  aFileName: the name of the file to be tested.
    * @param  aProtection: return parameter, true if the file is a DCF file
    * @return EOk if the content could be checked.
    */
    IMPORT_C TInt IsProtectedFile(
        const TDesC& aFileName, 
        TBool& aProtection);

    /**
    * IsProtectedFile
    *
    * Takes a file name and checks if it is a DCF file or a regular file.
    *
    * @since  3.0
    * @param  aFileHandle: handle to the file to be tested.
    * @param  aProtection: return parameter, true if the file is a DCF file
    * @return EOk if the content could be checked.
    */
    IMPORT_C TInt IsProtectedFile(
        RFile& aFileHandle, 
        TBool& aProtection);
    
    /**
    * GetContentInfo
    *
    * Takes a memory buffer containing a DCF(or at least the header part
        * of it) and returns the protection for this content, the MIME type, 
    * Content URI and encrypted data length in out parameters.
    *
    * @since  2.0
    * @param  aContent: memory buffer containing a DCF
    * @param  aProtection: return parameter for the type of protection for
    *         this DCF
    * @param  aMIMEType: return parameter for the MIME type header field
    * @param  aContentURI: return parameter for the content URI header field
    * @param  aDataLength: return parameter for the length of the encrypted
    *         data
    * @return EOk if the information could be extracted from the DCF
    */
    IMPORT_C TInt GetContentInfo(
        const TDesC8& aContent, 
        TContentProtection& aProtection, 
        HBufC8*& aMIMEType, 
        HBufC8*& aContentURI, 
        TUint& aDataLength);
    
    /**
    * GetFileInfo
    * 
    * Takes the name of a DCF file, opens it and returns the protection for
    * this content, the MIME type, Content URI and encrypted data length in
    * out parameters.
    *
    * @since  2.0
    * @param  aFileName: the name of the file to be inspected
    * @param  aProtection: return parameter for the type of protection for
    *         this DCF
    * @param  aMIMEType: return parameter for the MIME type header field
    * @param  aContentURI: return parameter for the content URI header field
    * @param  aDataLength: return parameter for the length of the encrypted
    *         data
    * @return EOk if the information could be extracted from the DCF
    */
    IMPORT_C TInt GetFileInfo(
        const TDesC& aFileName, 
        TContentProtection& aProtection, 
        HBufC8*& aMIMEType, 
        HBufC8*& aContentURI, 
        TUint& aDataLength);

    /**
    * GetFileInfo
    * 
    * Takes the name of a DCF file, opens it and returns the protection for
    * this content, the MIME type, Content URI and encrypted data length in
    * out parameters.
    *
    * @since  3.0
    * @param  aFileHandle: handle to the file to be inspected
    * @param  aProtection: return parameter for the type of protection for
    *         this DCF
    * @param  aMIMEType: return parameter for the MIME type header field
    * @param  aContentURI: return parameter for the content URI header field
    * @param  aDataLength: return parameter for the length of the encrypted
    *         data
    * @return EOk if the information could be extracted from the DCF
    */
    IMPORT_C TInt GetFileInfo(
        RFile& aFileHandle, 
        TContentProtection& aProtection, 
        HBufC8*& aMIMEType, 
        HBufC8*& aContentURI, 
        TUint& aDataLength);
    
    /**
    * GetContentHeader
    *
    * Takes a memory buffer containing a DCF(or at least the header part of
    * it) and a header name and returns the value of the header field in an
    * out parameter. Return value specifies if the header has been found.
    * Search is case insensitive.
    *
    * @since  2.0
    * @param  aContent: memory buffer containing a DCF
    * @param  aHeaderName: Name of the header to look for
    * @param  aHeaderValue: return parameter for value of the header
    * @return EOk if the header was found
    */
    IMPORT_C TInt GetContentHeader(
        const TDesC8& aContent, 
        const TDesC8& aHeaderName, 
        HBufC8*& aHeaderValue);
    
    /**
    * GetFileHeader
    *
    * Takes the name of a DCF file and a header name, opens the DCF file and
    * returns the value of the header field in an out parameter. Return value
    * specifies if the header has been found. Search is case insensitive.
    *
    * @since  2.0
    * @param  aFileName: the name of the DCF file to be opened
    * @param  aHeaderName: Name of the header to look for
    * @param  aHeaderValue: return parameter for value of the header
    * @return EOk if the header was found
    */
    IMPORT_C TInt GetFileHeader(
        const TFileName& aFileName, 
        const TDesC8& aHeaderName, 
        HBufC8*& aHeaderValue);
 
    /**
    * GetFileHeader
    *
    * Takes the name of a DCF file and a header name, opens the DCF file and
    * returns the value of the header field in an out parameter. Return value
    * specifies if the header has been found. Search is case insensitive.
    *
    * @since  3.0
    * @param  aFileHandle: handle to the DCF file
    * @param  aHeaderName: Name of the header to look for
    * @param  aHeaderValue: return parameter for value of the header
    * @return EOk if the header was found
    */
    IMPORT_C TInt GetFileHeader(
        RFile& aFileHandle, 
        const TDesC8& aHeaderName, 
        HBufC8*& aHeaderValue);
    
    /**
    * SetContentHeader
    *
    * Takes a memory buffer containing a DCF(or at least the header part of
        * it), a header name and the value for the header field. Sets the header
    * field in the optional DCF header. If the header field has not existed
    * before, it is created. Any previous value of the header field will be
    * overwritten.
    *
    * @since  2.0
    * @param  aContent: memory buffer containing a DCF
    * @param  aHeaderName: Name of the header to set
    * @param  aHeaderValue: New value of the heade
    * @return EOk if the header was set.
    */
    IMPORT_C TInt SetContentHeader(
        HBufC8*& aContent, 
        const TDesC8& aHeaderName, 
        const TDesC8& aHeaderValue);
    
    /**
    * SetFileHeader
    *
    * Takes the name of a DCF file, a header name and the value for the header
    * field. Opens the DCF file, sets the header field in the optional DCF
    * header and writes the DCF file. If the header field has not existed
    * before, it is created. Any previous value of the header field will be
    * overwritten.
    *
    * @since  2.0
    * @param  aFileName: the name of the DCF file to be opened
    * @param  aHeaderName: Name of the header to set
    * @param  aHeaderValue: New value of the heade
    * @return EOk if the header was set.
    */
    IMPORT_C TInt SetFileHeader(
        const TDesC16& aFileName, 
        const TDesC8& aHeaderName, 
        const TDesC8& aHeaderValue);

    /**
    * SetFileHeader
    *
    * Takes the name of a DCF file, a header name and the value for the header
    * field. Opens the DCF file, sets the header field in the optional DCF
    * header and writes the DCF file. If the header field has not existed
    * before, it is created. Any previous value of the header field will be
    * overwritten.
    *
    * @since  3.0
    * @param  aFileHandle: handle to the DCF file
    * @param  aHeaderName: Name of the header to set
    * @param  aHeaderValue: New value of the heade
    * @return EOk if the header was set.
    */
    IMPORT_C TInt SetFileHeader(
        RFile& aFileHandle, 
        const TDesC8& aHeaderName, 
        const TDesC8& aHeaderValue);

    /**
    * GetSingleRightsObject
    *
    * Takes a content URI, a local ID and a rights object out parameter.
    * Looks up the rights object using the content URI and the local ID
    * and returns it in the out parameter.
    *
    * @since  2.5
    * @param  aContentURI: the content URI of the content to be tested.
    * @param  aLocalID: local ID of the rights object
    * @param  aRightsObject: Out parameter for the rights object. It has
    *         to be released by the caller.
    * @return EOk if the rights object could be retreived, ENoRights if
    *         no rights object exists.
    */
    IMPORT_C TInt GetSingleRightsObject(
        const TDesC8& aContentURI, 
        TUint32 aLocalID, 
        CDRMRights*& aRightsObject);
    
    /**
    * GetDetailedContentRights
    *
    * Takes a Content URI and an array out parameter. Fills the rights
    * specification array with the available rights by looking up the content
    * URI in the rights database and retrieving the stored rights. The array
    * parameter will contain a list of pointers to all rights object for the 
    * given content URI. The rights objects themselves can be distinguished by
    * their local ID.
    *
    * @since  2.5
    * @param  aContentURI: the content URI of the content to be tested.
    * @param  aRightsList: Out parameter for the available rights. It has to
    *         be released by the caller.
    * @return EOk if the rights could be retreived, ENoRights if no rights 
    *         object exists.
    */
    IMPORT_C TInt GetDetailedContentRights(
        const TDesC8& aContentURI, 
        RPointerArray<CDRMRights>*& aRightsList);
    
    /**
    * GetDetailedFileRights
    *
    * Takes a file name and a rights specification. Opens the given file and 
    * fills the rights specification array with the available rights by
    * looking up the content URI in the rights database and retrieving the
    * stored rights. The array parameter will contain a list of pointers to
    * all rights object for the given content URI. The rights objects
    * themselves can be distinguished by their local ID.
    *
    * @since  2.5
    * @param  aFileName: the name of the file to be tested.
    * @param  aRightsList: Out parameter for the available rights.
    * @return EOk if the rights could be retreived, if the file is not a DCF
    *         file or if the content URI cannot be found, ENoRights.
    */
    IMPORT_C TInt GetDetailedFileRights(
        const TDesC& aFileName, 
        RPointerArray<CDRMRights>*& aRightsList);

    /**
    * GetDetailedFileRights
    *
    * Takes a file name and a rights specification. Opens the given file and 
    * fills the rights specification array with the available rights by
    * looking up the content URI in the rights database and retrieving the
    * stored rights. The array parameter will contain a list of pointers to
    * all rights object for the given content URI. The rights objects
    * themselves can be distinguished by their local ID.
    *
    * @since  3.0
    * @param  aFileHandle: handle to the file to be tested.
    * @param  aRightsList: Out parameter for the available rights.
    * @return EOk if the rights could be retreived, if the file is not a DCF
    *         file or if the content URI cannot be found, ENoRights.
    */
    IMPORT_C TInt GetDetailedFileRights(
        RFile& aFileHandle, 
        RPointerArray<CDRMRights>*& aRightsList);
    
    /**
    * GetActiveRights
    *
    * Takes a Content URI and a rights specification. Finds all rights objects
    * associated with given URI and returns a possible active one. The out 
    * parameters will contain a pointer to the found rights object. The found
    * rights object can be expired. The details about expiration and constraints
    * of the rights can be retrieved with the CDRMRights::GetRightsInfo function. 
    *
    * @since  2.5
    * @param  aContentURI: the content URI of the content to be tested.
    * @param  aRightsSpec: The rights to be checked defined as a bitmask
    *		  combination of EPlay, EDisplay, EExecute and EPrint
    * @param  aRightsObject: Out parameter for the rights object. It has
    *         to be released by the caller.
    * @return EFullRights, ERestrictedRights, EPreviewRights if a possble active
    *         rights object was found. ENoRights if no rights object for given 
    *         content URI or permissions.
    */
    IMPORT_C TInt GetActiveRights(
        const TDesC8& aContentURI,
        TUint32 aRightsSpec,
        CDRMRights*& aRightsObject);
    
    /**
    * GetContentURIList
    *
    * Returns a list of all content URIs that have rights in the rights 
    * database.
    *
    * @since  2.5
    * @param  aURIList: Out parameter for the URI list.
    * @return Ok if the URI list could be retreived.
    */
    IMPORT_C TInt GetContentURIList(
        RPointerArray<HBufC8>*& aURIList);
    
    /**
    * DataTypesCount
    *
    * Returns the number of previously registered datatypes which are
    * supported by the DRM system and associated applications.
    *
    * @since  2.0
    * @param  aCount: Out parameter for the datatype count
    * @return EOk if the number of datatypes has been returned correctly
    */
    IMPORT_C TInt DataTypesCount(
        TInt& aCount);
    
    /**
    * SupportedDataType
    *
    * Returns a specific datatype which has been registered before by giving an index
    *
    * @since  2.0
    * @param  aIndex: Number of the datatype to return
    * @param  aDataType: Out parameter for the datatype
    * @return EOk if the datatype was returned correctly
    */
    
    IMPORT_C TInt SupportedDataType(
        const TInt aIndex, TDataType& aDataType);
    
    /**
    * StaticDataTypesCount
    *
    * Returns the number of default registered datatypes which are
    * supported by the DRM system and associated applications.
    *
    * @since  2.0
    * @param  aCount: Out parameter for the static datatype count
    * @return EOk if the number of datatypes has been returned correctly
    */
    IMPORT_C TInt StaticDataTypesCount(
        TInt& aCount);
    
    /**
    * RegisterDataType
    *
    * Registers a datatype as a supported datatype.
    *
    * @since  2.0
    * @param  aDataType: Datatype to register
    * @return EOk if the datatype has been registered
    */
    
    IMPORT_C TInt RegisterDataType(
        const TDataType& aDataType);
    
    /**
    * UnRegisterDataType
    *
    * Unregisters a datatype via its index.
    *
    * @since  2.5
    * @param  aIndex: Number of the datatype to unregister
    * @return EOk if the datatype has been unregistered
    */
    
    IMPORT_C TInt UnRegisterDataType(
        const TInt aIndex);
    
    /**
    * SupportedDRMMethods
    *
    * Returns the supported DRM protection methods and the OMA compliance
    * level of the DRM engine
    *
    * @since  2.0
    * @param  aDRMMethod: Out parameter, bit mask containing any 
    *         combination of EForwardLock, ECombinedDelivery, 
    *         ESeparateDelivery etc.
    * @param  aOMALevel: OMA compliance of the DRM engine
    * @return DRMCommon error code
    */
    
    IMPORT_C TInt SupportedDRMMethods(
        TInt& aDRMMethod, TOMALevel& aOMALevel);
        
    /**
    * MergeParentAndChild
    *
    * Merge rights with their parent rights
    *
    * @since  3.0
    * @param  aRights Rights to be merged with their parent rights
    */

    IMPORT_C void MergeParentAndChild(CDRMRights* aRights);
        
    
    
protected:
    
    /**
    * C++ default constructor.
    */
    IMPORT_C DRMCommon(void);
    
    /**
    * 2nd phase constructor.
    */
    IMPORT_C void ConstructL(void);
    
    /**
    * MapErrorCode
    *
    * Maps the error codes returned by the core engine to DRMCommon
    * error codes.
    *
    * @since  2.5
    * @param  aCode: core error code
    * @return DRMCommon error code.
    */
    IMPORT_C TInt MapErrorCode(
        const TInt aCode);
    
    };

#endif      // DRMCOMMON_H   

// End of File
