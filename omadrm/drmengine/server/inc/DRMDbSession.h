/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class declares the interface of class CDRMDbSession.
*
*/


#ifndef CDRMDBSESSION_H
#define CDRMDBSESSION_H

//  INCLUDES
#include <e32base.h>
#include <caf/caf.h>
#include "DRMTypes.h"
#include "DrmConstraint.h"
#include "DRMEngineClientServer.h"
#include "drmcommonclientserver.h"
#include "drmenginetypedefs.h"
#include "RoapStorageClient.h"

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CDRMParentStorage;

class CDRMConsume;
class CDRMPermission;
class CDRMNotifier;
class CDRMObsoleteFinder;

// DATA TYPES
// CLASS DECLARATION

/**
*  Server side instance of DRM Rights Database session.
*  This class is the server side instance of C/S communications of a certain
*  session.
*
*  @lib RightsServer.exe
*  @since S60Rel2.5
*/
NONSHARABLE_CLASS( CDRMDbSession ) : public CSession2
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @since S60Rel2.6
    * @return New session instance.
    */
    static CDRMDbSession* NewL();

   /**
    * Destructor.
    * @since S60Rel2.5
    */
   virtual ~CDRMDbSession();

public: // Functions from base classes

    /**
    * From CSession2: Handles the service request event.
    * @since S60Rel2.6
    * @param aMessage The message related to the event.
    * @exception Method leaves with appropriate exception value
    *            if any errors occured.
    */
    void ServiceL( const RMessage2& aMessage );

    /**
    * From CSession2: Handles the errors from ServiceL if encountered.
    * @since S60Rel3.0
    * @param aMessage The message related to the event.
    * @param aError   Error code.
    */
    virtual void ServiceError( const RMessage2& aMessage,
                               TInt aError );

public: // New functions

   void AsyncOperationDone();

   void AsyncOperationDone( TFileName* aFileName );

    /**
    * This method tries to find a suitable rights object
    * from the given list which allows the specified intent.
    * @since S60Rel3.0
    * @param aIntent Intent.
    * @param aRights List of rights objects.
    * @param aParent Out-parameter: related parent permission. The caller
    *                owns the parent.
    * @param aUpdate Out-parameter: whether to update the rights object
    * (if one is found).
    * @return Symbian error or CAF error. KErrNone if everything goes
    *         properly
    */
    TInt FindRightsObject( const ContentAccess::TIntent aIntent,
                           const TDesC8& aURI,
                           CDRMPermission*& aChild,
                           HBufC8*& aUsedURI,
                           TUint32& aReason );

    /**
    * Check whether the identified permission allows the caller to get
    * decrypted data. Enforces e.g. capability checking
    *
    * @param aContentId Content ID of the content, can be NULL
    * @param aPermission Permission which grants access, can be NULL
    * @param aIntent Consumption intent
    * @return KErrAccessDenied if the caller does not have sufficient
    *         credentials, KErrNone if access is granted, or other
    *         Symbian errors
    */
    TInt VerifyCredentials( HBufC8* aContentId,
                            CDRMPermission* aPermission,
                            TIntent aIntent );

    /**
    * Return the SID of the client
    * @return SID of the client
    */
    TUid GetClientSid();

    /**
    * Return the VID of the client
    * @return VID of the client
    */
    TUid GetClientVid();

    /**
    * Return if the client has sufficient DRM capability
    * @return ETrue if the capability is available, EFalse otherwise
    */
    TBool ClientHasCapability();

    /**
    * This method returns handle to the Roap storage client.
    *
    * @since S60 3.2
    * @return Handle to the Roap storage client
    */
    Roap::RRoapStorageClient& RoapClient();

    /**
    * This method connects the roap client, does nothing if it's already connected
    *
    * @since S60 3.2
    * @return Handle to the Roap storage client
    */
    TInt ConnectRoapClient();

protected:  // New functions

protected:  // Functions from base classes

private:

    enum TProtectionType
        {
        ENoProtection,
        EPublicKey,
        EDomainKey,
        };

    enum TCredentialsCheckType
        {
        ENotChecked,
        ECheckedAndAllowed,
        ECheckedAndDenied,
        };

    /**
    * C++ constructor.
    * @since S60Rel2.6
    */
    CDRMDbSession();

    /**
    * Second phase constructor.
    * @since S60Rel2.6
    */
    void ConstructL();

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void AddRecordL( const RMessage2& aMessage,
                     TProtectionType aProtection );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void GetRecordL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void GetEntryListL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void DeleteL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void DeleteRecordL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void ExportCIDsL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void GetKeyL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void CheckRightsL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since S60Rel2.5
    * @param aMessage The message associated to the event.
    */
    void CountL( const RMessage2& aMessage );

    /**
    * Implementation of DeleteAll() command.
    * @since 2.5
    * @param aMessage The message associated to the event.
    */
    void DeleteAllL( const RMessage2& aMessage );

    /**
    * Implementation of ConsumeL() command.
    * @since 2.5
    * @param aMessage The message associated to the event.
    */
    void ConsumeL( const RMessage2& aMessage );

    /**
    * Check if Consume is possible
    * @since 3.0
    * @param aMessage The message associated to the event.
    */
    void CheckConsumeL( const RMessage2& aMessage );

    /**
    * Calculate the padding using a given data block
    * @since 2.5
    * @param aMessage The message associated to the event.
    */
    void CalculatePaddingL( const RMessage2& aMessage );

    /**
    * Give secure time & trust level to the client.
    * @since 2.6
    * @params aMessage The message associated to the event.
    */
    void SecureTimeL( const RMessage2& aMessage );

    /**
    * Encrypt data using the session key
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void EncryptL( const RMessage2& aMessage );

    /**
    * Decrypt data using the session key
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void DecryptL( const RMessage2& aMessage );

    /**
    * Initialize the session key
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void InitializeKeyL( const RMessage2& aMessage );

    /**
    * Initialize the session key from a group key
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void InitializeGroupKeyL( const RMessage2& aMessage );

    /**
    * Return the prepared data to the client.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void GetPreparedDataL( const RMessage2& aMessage );

    /**
    * Return the prepared data to the client.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void AddDomainROL( const RMessage2& aMessage );

    /**
    * Return the prepared data to the client.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void GetDomainROL( const RMessage2& aMessage );

    /**
    * Return the prepared data to the client.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void DeleteDomainROL( const RMessage2& aMessage );

    /**
    * Return the domain RO for a given content ID.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void GetDomainRosForCidL( const RMessage2& aMessage );

    /**
    * Check whether the RO is in Replay Cache.
    * @since 3.0
    * @pareams aMessage The message associated to the event.
    */
    void IsInCacheL( const RMessage2& aMessage );

    /**
    * Add the RO entry to Replay Cache.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void AddToCacheL( const RMessage2& aMessage );

    /**
    * Delete expired permission if we have secure time.
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void DeleteExpiredPermissionsL( const RMessage2& aMessage );

    /**
    * Sets the estimated arrival for a Content URI
    * @since 2.6
    * @params aMessage The message associated to the event.
    */
    void SetEstimatedArrivalL( const RMessage2& aMessage );

    /**
    * Gets the estimated arrival for a Content URI
    * @since 2.6
    * @params aMessage The message associated to the event.
    */
    void GetEstimatedArrivalL( const RMessage2& aMessage );

    /**
    * Gets the udt data
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void GetUdtDataL( const RMessage2& aMessage );

    /**
    * Initiates the UDT
    * @since 3.0
    * @params aMessage The message associated to the event.
    */
    void InitiateUdtL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since 3.0
    * @param aMessage The message associated to the event.
    */
    void InitExportOrphanedCIDsL( const RMessage2& aMessage );

    /**
    * Helper function for ServiceL().
    * @since 3.0
    * @param aMessage The message associated to the event.
    */
    void ExportOrphanedCIDsL( const RMessage2& aMessage );

    /**
    * Unwrap MAC and REK which are wrapped either with
    * device public key or domain key
    * @since 3.1
    * @param aMessage The message associated to the event.
    * @param aDomainRo ETrue if a domain key is used for wrapping
    *                  EFalse if device public key is used wrapping
    */
    void UnwrapMacAndRekL( const RMessage2& aMessage, TBool aDomainRo );

    /**
    * This methods converts the given list into a permanent file
    * representation.
    * @since S60Rel2.5
    * @param aList List to be converted into a file.
    * @param aFile Output file name.
    */
    void ListToFileL( RPointerArray< CDRMPermission >& aList,
                      TFileName& aFile );

    /**
    * Tries to locate the best permission candidates from the given list based
    * on the given usage intent.
    * @return Index of the best child in aList, >= 0 always. Leaves if no
    *         suitable child is found.
    */
    TInt FindBestROsL( RDRMPermissionList& aList,
                       const TDesC8& aURI,
                       const ContentAccess::TIntent aIntent,
                       HBufC8*& aUsedURI,
                       TUint32& aReason );

    /**
    * Returns ETrue if might be possible to use the permission
    * as a child permission in the first place.
    */
    TBool IsValidPermissionL( CDRMPermission& aPermission,
                              const ContentAccess::TIntent aIntent,
                              const TTime& aTime,
                              TUint32& aReason );


    void Normalize( CDRMPermission& aPermission,
                    CDRMConstraint& aNormalized,
                    const ContentAccess::TIntent aIntent );

    TBool BetterPermission( const CDRMConstraint& aNewOne,
                            const CDRMConstraint& aOldOne );


    TInt GetBestROL( RDRMPermissionList& aList,
                     const TIntent aIntent,
                     TUint32& aReason );

    /**
    * This method checks whether the constraint has expired.
    *
    * @since S60Rel2.5
    * @param aConstraint Rights constraint.
    * @return Truth value.
    */
    inline TBool Expired( const CDRMConstraint& aConstraint );

    /**
    * finds the parent permissions and does not add unusable parents and removes unusable childred
    *
    * @param aList      List of child permissions
    * @param aIntent    Intent for use
    * @param aDrmTime   Drm time of the device
    * @param aReason    Out-parameter: a bitvector of why some prmissions were disqualified
    */
    void FindParentsAndRemoveUnusableL( RDRMPermissionList& aList,
                                        const TDesC8& aURI,
                                        CDRMPointerArray<HBufC8>& aCids,
                                        const TIntent aIntent,
                                        const TTime& aDrmTime,
                                        TUint32& aReason );

    /**
    * This method checks whether the constraint is invalid
    * (count < 1 || interval <= 0 ).
    *
    * @since S60Rel2.6
    * @param aConstraint Rights constraint.
    * @return Truth value.
    */
    inline TBool Invalid( const CDRMConstraint& aConstraint );

    /**
    * This method updates the trusted time and its trust level from DRM server.
    * Since S60Rel2.5
    */
    inline void UpdateSecureTime();

    /**
    * Unwrap a CEK which is wrapped with a REK
    *
    * @param aProtectedCek Wrapped CEK
    * @return Unwrapped CEK
    */
    HBufC8* UnwrapProtectedCekL( const TDesC8& aProtectedCek );

    /**
    * Unwrap a CEK which is wrapped with a domain key.
    *
    * @param aProtectedCek Wrapped CEK
    * @param aDomainId ID of the domain key
    * @return Unwrapped CEK
    */
    HBufC8* UnwrapDomainCekL( const TDesC8& aProtectedCek,
                              const TDesC8& aDomainId );

    /**
    * Set the authentication seed for a content ID
    *
    * @param aMessage IPC message
    */
    void SetAuthenticationSeedL( const RMessage2& aMessage );

    /**
    * Return the authentication seed for a content ID
    *
    * @param aMessage IPC message
    */
    void GetAuthenticationSeedL( const RMessage2& aMessage );

    /**
    * Integrity protection for protected ROs
    *
    * @param aMessage IPC message
    */
    void VerifyMacL( const RMessage2& aMessage );

    /**
    *
    *
    * @param aMessage IPC message
    */
    void GetSupportedIndividualsL( const RMessage2& aMessage );

    /**
    * Stop watching the DCF repository server and the rights DB
    *
    * @param aMessage IPC message
    */
    void StopWatchingL( const RMessage2& aMessage );

    /**
    * Return the current content ID
    *
    * @return content ID
    */
    const TDesC8& GetContentId();

    /**
    * Remove permissions which are currently invalid (e.g. domain context
    * not available)
    *
    * @param aList List of permissions
    */
    void RemoveInvalidPermissionsL( CDRMPermissionList* aList );

    void SetNameL( const RMessage2& aMessage );

    void GetNameL( const RMessage2& aMessage );

    void GetWideDataL( const RMessage2& aMessage );

    void Cancel( const RMessage2& aMessage );

    void GetFLUriL( const RMessage2& aMessage );

    void EncodeRightsIssuerL( const RMessage2& aMessage );

    void DecodeRightsIssuerL( const RMessage2& aMessage );

    void GetRandomDataL( const RMessage2& aMessage );

    void GetFlURI( TDes8& aURI );

    void AesEncryptL( const TDesC8& aKey,
                      const TDesC8& aIV,
                      const TBool aAddPadding,
                      TDes8& aData );

    void AesDecryptL( const TDesC8& aKey,
                      const TDesC8& aIV,
                      const TBool aRemovePadding,
                      TDes8& aData );

    void GenerateIVL( TDes8& aData );

    TBool PendingRights(const TDesC8& aCid, TBool aRemoval = ETrue);

    /**
    * This method tries to find a suitable rights object
    * from the given list which allows the specified intent.
    * @since S60Rel3.0
    * @param aIntent Intent.
    * @param aURI    Content URI
    * @param aChild  Out-parameter: related child permissino. The caller
    *                owns the child.
    * @param aParent Out-parameter: related parent permission. The caller
    *                owns the parent.
    * @param aReason A bitvector of reasons for rights object rejection
    */
    void FindRightsObjectL( const ContentAccess::TIntent aIntent,
                            const TDesC8& aURI,
                            CDRMPermission*& aChild,
                            HBufC8*& aUsedURI,
                            TUint32& aReason );

    /**
    * Check if the deletion of the mentioned content id is allowed:
    */
    TBool DeleteAllowedL( const TDesC8& aContentId );

    // Prohibit copy constructor.
    CDRMDbSession( const CDRMDbSession& );
    // Prohibit assigment operator.
    CDRMDbSession& operator=( const CDRMDbSession& );

    void GetMeteringDataL( const RMessage2& aMessage );

    void DeleteMeteringDataL( const RMessage2& aMessage );

public:     // Data

protected:  // Data

private:    // Data

    TTime iTrustedTime;
    TBool iSecureTime;
    HBufC8* iPreparedData;
    HBufC* iWidePreparedData;
    CActive* iPendingRequest;
    CDRMConsume* iConsume;

    // Decryption key for this session
    TBuf8< KDRMKeyLength > iCek;

    // Rights encyrption key for this session, used for adding protected keys and
    // the authentication seed
    TBuf8< KDRMKeyLength > iRek;

    TBuf8< KDRMKeyLength > iMac;

    // Have the credentials of the caller been checked for this session?
    TCredentialsCheckType iCredentialsChecked;

    // Filename used for getting obsolete Contents
    TFileName* iFileName;

    // Client handle
    RThread iClient;

    // ID of the currently handled content
    HBufC8* iContentId;

    // Roap client
    TBool iRoapClientConnected;
    Roap::RRoapStorageClient iRoapClient;

public:     // Friend classes
protected:  // Friend classes
private:    // Friend classes
    };

#include "DRMDbSession.inl"


#endif      // CDRMDBSESSION_H

// End of File
