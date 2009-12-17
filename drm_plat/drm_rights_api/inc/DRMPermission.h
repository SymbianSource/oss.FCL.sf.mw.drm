/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for the DRM Rights database
*
*/


#ifndef DRMPERMISSION_H
#define DRMPERMISSION_H

// INCLUDES

#include <e32base.h>
#include <caf/caf.h>
#include "DRMTypes.h"

using namespace ContentAccess;
// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CDRMConstraint;


// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMPermission implements the permission for OMA DRM usage
*
*  @lib DrmRights.dll
*  @since S60 3.0
*/

class CDRMPermission : public CBase
    {
public:
    enum TExportMode
        {
        ECopy,
        EMove
        };
    
public: // CBase

    /**
    * NewLC
    *
    * Creates a new CDRMPermission object and returns a pointer to it.
    * The function leaves the object to the cleanup stack.
    *
    * @since    3.0
    * @return   a functional CDRMPermission object, The function leaves
    *           with SymbianOS error code if an error occurs.
    */
    IMPORT_C static CDRMPermission* NewLC();

    /**
    * NewL
    *
    * Creates a new CDRMPermission object and returns a pointer to it.
    *
    * @since    3.0
    * @return   a functional CDRMPermission object, The function leaves
    *           with SymbianOS error code if an error occurs.
    */
    IMPORT_C static CDRMPermission* NewL();
    
    /**
    * Destructor
    */
    IMPORT_C virtual ~CDRMPermission();

public: // New functions    
    /**
    * ExternalizeL
    * 
    * Writes the data of the object into the stream
    *
    * @since    3.0
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * InternalizeL
    * 
    * Reads the data of the object from the stream
    *
    * @since    3.0
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
    * Stateful() 
    *
    * Tells if the rights object is stateful or not
    *
    * @since    3.0
    * @return   returns ETrue if the rights object is stateful
    *                   EFalse if it is not    
    */
	IMPORT_C TBool Stateful() const;
	
	/**
	* Child()
	*
	* Tells if the rights object is a child or not
	*
    * @since    3.0
    * @return   returns ETrue if the rights object is child
    *                   EFalse if it is not
    */
    IMPORT_C TBool Child() const;

    /**
    * Size
    *
    * returns the size of the externalized object
    *
    * @since    3.0
    * @return   returns the size of the constraint
    */   
    IMPORT_C TInt Size() const;
    
    /**
    * NULL or actual constraints.
    */
    IMPORT_C CDRMConstraint* ConstraintForIntent( const ContentAccess::TIntent aIntent );
    
    /**
    * NULL or actual constraint.
    */
    IMPORT_C CDRMConstraint* TopLevelConstraint(  );

    /**
    * Note: Top level constraint is always consumed if it exists.
    */
    IMPORT_C void ConsumeRights( const ContentAccess::TIntent aIntent, const TTime& aCurrentTime );


    /**
    * ImportL
    * 
    * Reads the data of the object from the descriptor and initialized the 
    * object accordingly
    *
    * @since    3.0
    * @param    aBuffer : the buffer containing the data of the imported object
    * @return   None, The function leaves with Symbian OS error code if an
    *           error occurs
    */
    IMPORT_C void ImportL( const TDesC8& aBuffer );
    
    /**
    * ExportL
    * 
    * Writes the data of the object into an HBufC8 and returns a pointer
    * to it. The caller is responsible for deleting the object.
    *
    * @since    3.0
    * @return   Returns an HBufC8* containing the data of the object,
    *           The function leaves with Symbian OS error code if an
    *           error occurs
    */
    IMPORT_C HBufC8* ExportL() const;

    /**
    * DuplicateL
    * 
    * Copies the data of the permission into the current one.
    *
    * @since    3.0
    * @param    aPermission : the permission to duplicate into this one
    * @return   None. The function leaves with Symbian OS error code if an
    *           error occurs
    */    
    IMPORT_C void DuplicateL( const CDRMPermission& aPermission );
    
    /**
    * Expired   
    *
    * Checks if the permission has expired
    *
    * @since	3.0
    * @param	aTime : The current secure time
    *                   if the time is Time::NullTTime() time based rights
    *                   will be ignored
    * @return   ETrue if the permission has expired
    * 			EFalse if it is still valid or has future rights 
    */
    IMPORT_C TBool Expired( const TTime& aTime );
    
    
    /**
    * Valid
    *
    * returns if the permission is valid/usable at this time
    *
    * If no second parameter is given all the constraints will
    * be checked for validity and if any of them is invalid
    * the permission is considered to be invalid.
    *
    * For ERightsPlay, ERightsDisplay, ERightsExecute, ERightsPrint
    * also the toplevel constraint will be checked for validity
    *
    * @since    3.0
    * @param	aTime : current secure time, if the time is
    *			        Time::NullTTime() time based constraints
    *					will be considered to be invalid
    * @param    aIndividual : the imsi of the phone
    * @param    aRejection : return value for why the content was rejected
    * @param	aConstraint : the constraint to check validity on
    *							
    * @return   returns ETrue if the constraint is valid
    *					EFalse if the constraint is not valid
    */ 
    IMPORT_C TBool Valid( const TTime& aTime,
                          const RPointerArray<HBufC8>& aIndividual, 
                          TUint32& aRejection,
                          const TRightsType aType = ERightsAll ) const;
                          
    /**
    * Merge permission with another permission
    *
    * @since	3.0
    * @param	aPermission permission to merge with
    */
    IMPORT_C void Merge( const CDRMPermission& aPermission );
    
    /**
    * Checks if the permission has software constaints
    *
    * @since	3.0
    * @return   ETrue if the permission has software constaints, EFalse otherwise
    */
    IMPORT_C TBool SoftwareConstrained() const;
    
    
public:

    // synchronizing marker
    TInt32 iSyncMark;
   
    // Version number
    TInt32 iVersion;
    
    // Unique ID of the permission
    TDRMUniqueID iUniqueID;
    
    // The original insertion time
    TTime iOriginalInsertTime;
    
    // Top level constraint
    CDRMConstraint* iTopLevel;
    
    // Play constraint
    CDRMConstraint* iPlay;
    
    // Display constraint
    CDRMConstraint* iDisplay;
    
    // Execute constraint
    CDRMConstraint* iExecute;
    
    // Print constraint
    CDRMConstraint* iPrint;
    
    // Export constraint
    CDRMConstraint* iExport;
    
    // Export mode
    TExportMode iExportMode;
    
    // Content ID of the parent rights object
    HBufC8* iParentUID;
    
    // Rights Object ID of the rights delivery container
    HBufC8* iRoID;
    
    // Domain identifier
    HBufC8* iDomainID;
    
    // Available rights
    TUint16 iAvailableRights;	// Bitmask
    
    // Version number of the rights object
    TROVersion iRightsObjectVersion;
    
    // Additional information bitvector
    TInt32 iInfoBits;
    
    // Right issuer identifier
    TBuf8<KRiIdSize> iRiId;
    
    // URL to send HTTP GET on expiration of permission 
    HBufC8* iOnExpiredUrl;

protected:

    /**
    * Default constructor
    */
    IMPORT_C CDRMPermission();

private:

    /**
    * 2nd phase constructor
    */
    void ConstructL();

    /**
    * WriteInt64L
    * 
    * Writes the 64 bit integer to the stream
    *
    * @since    3.0
    * @param    aWrite : the 64 bit integer to write
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void WriteInt64L( const TInt64& aWrite, RWriteStream& aStream ) const;

    /**
    * ReadInt64L
    * 
    * Reads the 64 bit integer from the stream
    *
    * @since    3.0
    * @param    aRead : the 64 bit integer read
    * @param    aStream : the output stream to write to
    * @return   The function leaves with Symbian OS error code if an
    *           error occurs
    */
    void ReadInt64L( TInt64& aRead, RReadStream& aStream );    

    };

#endif      // DRMPERMISSION_H   
            
// End of File
