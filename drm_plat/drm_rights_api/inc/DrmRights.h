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
* Description:  Declaration of class DrmRights for accessing common
*                DRM functions
*
*/


#ifndef DRMRIGHTS_H
#define DRMRIGHTS_H

#include <e32base.h>
#include <caf/caf.h>
#include "DcfCommon.h"
#include "DrmAsset.h"
#include "DrmPermission.h"
#include "DrmConstraint.h"

using namespace ContentAccess;
// TEST DEFINITION

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CDRMRightsConstraints: Encapsulates rights constaints for a specific right.
*
*  @lib    DrmRights.dll
*  @since  2.5
*/
class CDRMRightsConstraints: public CBase
    {
public:     // Data Types
    enum TConstraintPriority
        {
        ENoConstraints = 8,
        ETime = 7,
        ETimeInterval = 6,
        ETimeIntervalCount = 5,
        ETimeCount = 4,
        EInterval = 3,
        EIntervalCount = 2,
        ECount = 1
        };
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CDRMRightsConstraints* NewL(void);
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDRMRightsConstraints(void);
    
public: // New functions
    
    /**
    * FullRights
    * 
    * The FullRights function checks if the object contains any restrictions 
    * or if usage is unconstrained.
    *
    * @since  2.5
    * @return ETrue if this object represents unconstrained rights, EFalse if
    *         there are some constraints set
    */
    IMPORT_C TBool FullRights(void);
    
    /**
    * IsPreview
    * 
    * The function checks if the object represents a preview constraint.
    *
    * @since  2.5
    * @return ETrue if this object represents a preview constraint, EFalse
    *         otherwise
    */
    IMPORT_C TBool IsPreview(void);

    /**
    * GetConstraintInfo
    * 
    * The fucntion retunrs expiration and constraint details of the object  
    *
    * @since  2.5
    * @param  aExpiration: return parameter for the expiration detail of the
    *         object. EValid, EExpired, EFuture
    * @param  aConstType: return parameter for the constraint details of the
    *         object. A bitmask combination of ETimeBased, EInActiveInterval,
    *         ECountBased. Or only ENoConstraints 
    * @return Priority value of the object, if the details were returned
    *         succesfully. Priority value defines, which constraint is used
    *         during decryption. The highest one is consumed first.
    *         EInvalidRights if details cannot be retrieved. 
    */
    IMPORT_C TInt GetConstraintInfo(
        TUint32& aExpiration,
        TUint32& aConstType);

    /**
    * GetExpirationDetails
    * 
    * The function returns the end time and amount of counts left of the object.
    * 
    * @since  2.5
    * @param  aEndTime: return parameter for the end time value of the object.
    *         If end time restriction is not defined, parameter is set to 0.
    * @param  aCountsLeft: return parameter for amount of counts left. If count
    *         restriction is not defined, parameter is set to -1.          
    * @return EOk if details were retrieved successfully.
    * 
    */
    IMPORT_C TInt GetExpirationDetails(
        TTime& aEndTime,
        TInt& aCountsLeft);
	
    /**
    * GetCounters
    * 
    * This function returns the current value of the count restriction and
    * original value of the count restriction(value when the rights object
        * was added to the rights database)
    *
    * @since  2.5
    * @param  aCounter: out parameter for the current counter value
    * @param  aOriginalCounter: out parameter for the original counter value
    * @return EOk if the counters were returned successfully, ENoRights if 
    *         the object does not contain count based restrictions
    */
    IMPORT_C TInt GetCounters(
        TUint32& aCounter, 
        TUint32& aOriginalCounter);
    
    /**
    * GetTimedCounters
    * 
    * This function returns the current value of the timed count restriction and
    * original value of the count restriction(value when the rights object
        * was added to the rights database)
    *
    * @since  3.0
    * @param  aCounter: out parameter for the current timed counter value
    * @param  aOriginalCounter: out parameter for the original counter value
    * @return EOk if the counters were returned successfully, ENoRights if 
    *         the object does not contain count based restrictions
    */
    IMPORT_C TInt GetTimedCounters(
        TUint32& aCounter, 
        TUint32& aOriginalCounter);
    
    /**
    * GetStartTime
    * 
    * This function returns the start time constraint.
    *
    * @since  2.5
    * @param  aStartTime: out parameter for the start time restriction
    * @return EOk if the time restriction were returned successfully, 
    *         ENoRights if the object does not contain a start time
    *         restrictions
    */
    IMPORT_C TInt GetStartTime(
        TTime& aStartTime);
    
    /**
    * GetEndTime
    * 
    * This function returns the end time constraint.
    *
    * @since  2.5
    * @param  aEndTime: out parameter for the end time restriction
    * @return EOk if the time restriction were returned successfully, 
    *         ENoRights if the object does not contain an end time
    *         restrictions
    */
    IMPORT_C TInt GetEndTime(
        TTime& aEndTime);
    
    /**
    * GetInterval
    * 
    * This function returns interval time constraint
    *
    * @since  2.5
    * @param  anInterval: out parameter for the interval restriction
    * @return EOk if the interval time restriction was returned successfully, 
    *         ENoRights if the object does not contain interval based restrictions
    */
    IMPORT_C TInt GetInterval(
        TTimeIntervalSeconds& anInterval);
    
    /**
    * GetIntervalStart
    * 
    * This function returns the start time of an activated interval
    *
    * @since  2.5
    * @param  anInterval: out parameter for the start of an activated interval 
    * @return EOk if the interval start time was returned successfully, 
    *         ENoRights if the interval has not been started yet
    */
    IMPORT_C TInt GetIntervalStart(
        TTime& aStartTime);
    
    /**
    * GetAccumulated
    * 
    * This function returns accumulated time constraint
    *
    * @since  3.0
    * @param  aAccumulated: out parameter for the accumulated time restriction
    * @return EOk if the accumulated time restriction was returned successfully, 
    *         ENoRights if the object does not contain accumulated time based restrictions
    */
    IMPORT_C TInt GetAccumulated(
        TTimeIntervalSeconds& anInterval);
    
    /**
    * SetCounters
    * 
    * Sets the counter instance variables of the rights constraint
    *
    * @since  2.5
    * @param  aCounter: the current counter value
    * @param  aOriginalCounter: the original counter value
    * @return EOk if the counters were set successfully
    */
    IMPORT_C TInt SetCounters(
        TUint32 aCounter, 
        TUint32 aOriginalCounter);
    
    /**
    * SetStartTime
    * 
    * Sets the start time constraint.
    *
    * @since  2.5
    * @param  aStartTime: the start time restriction
    * @return EOk if the time restriction was set successfully, 
    *         ENoRights if the object does not contain a start time
    *         restrictions
    */
    IMPORT_C TInt SetStartTime(
        TTime aStartTime);
    
    /**
    * SetEndTime
    * 
    * Sets the end time constraint.
    *
    * @since  2.5
    * @param  aEndTime: the end time restriction
    * @return EOk if the time restriction was set successfully, 
    */
    IMPORT_C TInt SetEndTime(
        TTime aEndTime);
    
    /**
    * SetInterval
    * 
    * Sets the interval time constraint
    *
    * @since  2.5
    * @param  anInterval: the interval restriction
    * @return EOk if the interval time restriction was set successfully.
    */
    IMPORT_C TInt SetInterval(
        TTimeIntervalSeconds anInterval);
    
    /**
    * SetIntervalStart
    * 
    * Sets the interval start time constraint
    *
    * @since  2.5
    * @param  anInterval: the interval start restriction
    * @return EOk if the interval start restriction was set successfully.
    */
    IMPORT_C TInt SetIntervalStart(
        TTime aStartTime);
    
    /**
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C CDRMConstraint& GetConstraint(void);

    /**
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C void SetConstraint(CDRMConstraint&);

    /**
    * operator=
    * 
    * Assignment operator.
    *
    * @since  2.5
    * @param  aConstraint: constraint to copy data from
    * @return a copy of the aConstraint parameter.
    */
    IMPORT_C CDRMRightsConstraints& operator=(
        CDRMRightsConstraints& aConstraint);

protected:
    
    /**
    * Default constructor.
    */
    IMPORT_C CDRMRightsConstraints(void);
    
    /**
    * 2nd phase constructor.
    */
    IMPORT_C void ConstructL(void);
    
    
protected: // Data

    CDRMConstraint* iConstraint;    
    };

// CLASS DECLARATION

/**
*  CDRMRights: Encapsulates a single rights object.
*
*  @lib    DrmRights.dll
*  @since  2.5
*/
class CDRMRights: public CBase, public MAgentRightsBase
    {
public:     // Data Types
    enum TExpiration
        {
        EValidRights = 100,
        EFutureRights = 10,
        EExpiredRights = 1 
        };
    
    enum TConstraintType
        {
        ENoConstraints = 0x0001,
        ETimeBased = 0x0002,
        EInActiveInterval = 0x0004,
        ECountBased = 0x0008,
        ETimeIsAccumulatedTime = 0x0010
        };
    
    enum TRestriction
        {
        EFullRights = 1, 
        ERestrictedRights,
        EPreviewRights
        };

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
        EInvalidRights = -30108
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
    IMPORT_C static CDRMRights* NewL(void);
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDRMRights(void);
    
public: // New functions
    
    /**
    * GetContentURI
    * 
    * This function returns the content URI of the object
    *
    * @since  2.5
    * @param  aContentURI: out parameter for the content URI.
    * @return EOk if the URI was returned successfully.
    */
    IMPORT_C TInt GetContentURI(
        HBufC8*& aContentURI);
    
    /**
    * GetLocalID
    * 
    * This function returns the local ID of the object
    *
    * @since  2.5
    * @return the local ID.
    */
    IMPORT_C TUint32 GetLocalID(void);
    
    /**
    * GetAddTime
    * 
    * This function returns the time when the rights object was added to the
    * rights database
    *
    * @since  2.5
    * @param  aTime: out parameter for the time.
    */
    IMPORT_C void GetAddTime(
        TTime& aTime);
    
    /**
    * GetRightsInfo
    * 
    * This function returns the restriction details associated with given
    * permission.
    *
    * @since  2.5
    * @param  aConstraintSpec: The permissions to be checked defined as a 
    *         bitmask combination of EPlay, EDisplay, EExecute and EPrint
    * @param  aRestriction: return paramer for the restriction detail of the
    *         object: EFullRights, ERestrictedRights or EPreviewRights
    * @param  aExpiration: return parameter for the expiration detail of the
    *         object: EValid, EExpired, EFuture
    * @param  aConstType: return parameter for the constraint details of the
    *         object. ETimeBased, EInActiveInterval, ECountBased or ENoConstraints 
    * @return Priority value of the object, if the details were returned
    *         succesfully. Priority value defines, which object is used
    *         during decryption. The value is calculated with formula:
    *         TConstraintPriority * TRightsExpiration.
    *         ENoRights if no rights for given permissions.	      
    */
    IMPORT_C TInt GetRightsInfo(
        TUint32 aConstraintSpec,
        TRestriction& aRestriction,
        TExpiration& aExpiration,
        TUint32& aConstType);

    /**
    * GetExpirationDetails
    * 
    * The function returns the end time and amount of counts left of the object
    * associated with given permission. The use of this method is only recommended
    * for objects, which contain a end time or a count restriction. The constraint
    * type information can be retrieved with e.g. GetRightsInfo -function.
    * In cases of full rights and inactive interval rights the aEndTime is set to
    * 0 and the aCountsLeft is set to < 0.
    * 
    * @since  2.5
    * @param  aConstraintSpec: The permissions to be checked defined as a 
    *         bitmask combination of EPlay, EDisplay, EExecute and EPrint.
    * @param  aEndTime: return parameter for the end time value of the object.
    *         If end time restriction is not defined, parameter is set to 0.
    * @param  aCountsLeft: return parameter for amount of counts left. If count
    *         restriction is not defined, parameter is set to -1.          
    * @return EOk if details were retrieved successfully. ENoRights if no rights
    *         for given permissions.	 
    */
    IMPORT_C TInt GetExpirationDetails(
        TUint32 aConstraintSpec,
        TTime& aEndTime,
        TInt& aCountsLeft);
    
    /**
    * GetPlayRight
    * 
    * This function returns the constraints associated with the play right.
    *
    * @since  2.5
    * @param  aConstraint: out parameter for the constraints associated with
    *         this right. It must be deleted by the caller.
    * @return EOk if the right exists and the associated restrictions were 
    *         returned successfully, ENoRights if the object does not contain
    *         the given right.
    */
    IMPORT_C TInt GetPlayRight(
        CDRMRightsConstraints*& aConstraint);
    
    /**
    * GetDisplayRight
    * 
    * This function returns the constraints associated with the display right.
    *
    * @since  2.5
    * @param  aConstraint: out parameter for the constraints associated with
    *         this right. It must be deleted by the caller.
    * @return EOk if the right exists and the associated restrictions were 
    *         returned successfully, ENoRights if the object does not contain
    *         the given right.
    */
    IMPORT_C TInt GetDisplayRight(
        CDRMRightsConstraints*& aConstraint);
    
    /**
    * GetPrintRight
    * 
    * This function returns the constraints associated with the print right.
    *
    * @since  2.5
    * @param  aConstraint: out parameter for the constraints associated with
    *         this right. It must be deleted by the caller.
    * @return EOk if the right exists and the associated restrictions were 
    *         returned successfully, ENoRights if the object does not contain
    *         the given right.
    */
    IMPORT_C TInt GetPrintRight(
        CDRMRightsConstraints*& aConstraint);
    
    /**
    * GetExecuteRight
    * 
    * This function returns the constraints associated with the execute right.
    *
    * @since  2.5
    * @param  aConstraint: out parameter for the constraints associated with
    *         this right. It must be deleted by the caller.
    * @return EOk if the right exists and the associated restrictions were 
    *         returned successfully, ENoRights if the object does not contain
    *         the given right.
    */
    IMPORT_C TInt GetExecuteRight(
        CDRMRightsConstraints*& aConstraint);
    
    /**
    * GetContentURIAndLocalID
    * 
    * This function sets the content URI and local ID of the object.
    *
    * @since  2.5
    * @param  aContentURI: the new content URI.
    * @param  aLocalID: the new local ID.
    * @return EOk if the values were set successfully.
    */
    IMPORT_C TInt SetContentURIAndLocalID(
        HBufC8* aContentURI, 
        TUint32 aLocalID);

    IMPORT_C TInt SetContentURI(
        HBufC8* aContentURI);

    IMPORT_C TInt SetLocalID(
        TUint32 aLocalID);
    
    /**
    * SetAddTime
    * 
    * Sets the add time of the object.
    *
    * @since  2.5
    * @param  aTime: the new add time.
    * @return EOk if the value was set successfully.
    */
    IMPORT_C void SetAddTime(
        TTime aTime);
    
    /**
    * SetPlayRight
    * 
    * This function sets the play constraints.
    *
    * @since  2.5
    * @param  aConstraint: the constraints associated with the play right.
    *         It must be deleted by the caller.
    * @return EOk if the restrictions were set successfully
    */
    IMPORT_C TInt SetPlayRight(
        CDRMRightsConstraints* aConstraint);
    
    /**
    * SetDisplayRight
    * 
    * This function sets the constraints.
    *
    * @since  2.5
    * @param  aConstraint: the constraints associated with the display right.
    *         It must be deleted by the caller. 
    * @return EOk if the restrictions were set successfully
    */
    IMPORT_C TInt SetDisplayRight(
        CDRMRightsConstraints* aConstraint);
    
    /**
    * SetPrintRight
    * 
    * This function sets the print constraints.
    *
    * @since  2.5
    * @param  aConstraint: the constraints associated with the print right.
    *         It must be deleted by the caller.
    * @return EOk if the restrictions were set successfully
    */
    IMPORT_C TInt SetPrintRight(
        CDRMRightsConstraints* aConstraint);
    
    /**
    * SetExecuteRight
    * 
    * This function sets the execute constraints.
    *
    * @since  2.5
    * @param  aConstraint: the constraints associated with the execute right.
    *         It must be deleted by the caller.
    * @return EOk if the restrictions were set successfully
    */
    IMPORT_C TInt SetExecuteRight(
        CDRMRightsConstraints* aConstraint);
    
    /**
    * operator=
    * 
    * Assignment operator.
    *
    * @since  2.5
    * @param  aRight: object to copy data from
    * @return a copy of the aRight parameter.
    */
    IMPORT_C CDRMRights& operator=(
        CDRMRights& aRight);

    /**
    * 
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C CDRMPermission& GetPermission(void);

    /**
    * Permission needs to be freed, doesn't take control
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C void SetPermissionL(CDRMPermission& aPermission);

    /**
    * 
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C CDRMAsset& GetAsset(void);

    /**
    * Asset needs to be freed, doesn't take control
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C void SetAssetL(CDRMAsset& aAsset);
    
    /**
    * 
    *
    * @since  3.0
    * @param  
    * @return 
    */
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

    /**
    * 
    *
    * @since  3.0
    * @param  
    * @return 
    */
	IMPORT_C void InternalizeL(RReadStream& aStream);
    
    /**
    * Merge with another rights object
    *
    * @since  3.0
    * @param  
    * @return 
    */
	IMPORT_C void Merge(const CDRMRights& aRights);
    
protected:
    
    /**
    * Default constructor.
    */
    IMPORT_C CDRMRights(void);
    
    /**
    * 2nd phase constructor.
    */
    IMPORT_C void ConstructL(void);
    
    
protected: // Data

    CDRMAsset* iAsset;
    CDRMPermission* iPermission;
    };

#endif      // DRMRIGHTS_H   

// End of File
