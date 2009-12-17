/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides the functionalities which are needed
*                for implementing configurable OMA DRM rights storing
*
*/


// INCLUDE
#include <centralrepository.h>
#include <f32file.h>
#include <driveinfo.h>
#include "drmrightsstoringlocation.h"
#include "drmstoringinternalcrkeys.h"
#include "drmutilityinternaltypes.h"

// LOCAL CONSTANTS AND MACROS
const TInt KStringMaxSize = 1024;

// ----------------------------------------------------------------------------
// DrmRightsStoringLocation::CheckDrmRightsStorageDriveL
// Checks if the storing location of DRM Rights is configured in the
// Central Repository key.
// ----------------------------------------------------------------------------
EXPORT_C TBool DrmRightsStoringLocation::CheckDrmRightsStorageDriveL( RFs &aFs,
    TDrmScheme& aDrmScheme, TChar& aDriveLetter  )
    {
    TInt err( KErrNotFound );
    CRepository* repository( NULL );
    TBuf<KStringMaxSize> string;
    TBool configStoringLocationFound( EFalse );
    TInt driveNumber( -1 );
    TInt systemDriveNumber( -1 );
    TDriveInfo driveInfo;
    TInt loc( 0 );
    TDriveList driveList;
    
    aDriveLetter = ' ';

    // Check the Central Repository key for configurable storing location of
    // the given DRM scheme
    switch( aDrmScheme )
        {
        case EDrmSchemeOmaDrm:
            {
            break;
            } 
        case EDrmSchemeWmDrm:
            {
            TRAP( err, repository =
                CRepository::NewL( KCrUidWmDrmRightsServer ) );
            if ( !err )
                {
                // Read the string which should contain the drive letter
                err = repository->Get( KConfigWmDrmStoringLocation, string );
                delete repository;
                repository = NULL;
                }       
            break;
            }      
        default: 
            {
            break;
            } 
        }
      
    if ( !err )
        {    
        
        // Find out the first alphabet character of the string. It is expected 
        // that it corresponds to the drive letter.
        while ( ( loc < string.Length() ) && ( !aDriveLetter.IsAlpha() ) ) 
            {
            aDriveLetter = string[ loc ];
            loc++;
            }
        
        aFs.CharToDrive( aDriveLetter, driveNumber );
        aFs.Drive( driveInfo, driveNumber );
        
        // Check if the drive actually exists
        User::LeaveIfError( aFs.DriveList( driveList ) );
        
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, systemDriveNumber );
        
        // Configured storing location is the same as the default system drive.
        // This is not considered as an actual configuration. Use system drive also if
        // the drive for the configured storing location of rights does not exist
        if ( ( systemDriveNumber == driveNumber ) || ( !driveList[ driveNumber ] ) )
            {
            return configStoringLocationFound;
            }
        
        // Check that the given drive exists and it is not RAM drive (D:)
        if ( aDriveLetter.IsAlpha() && ( ( aDriveLetter != 'd' )
            && ( aDriveLetter != 'D' ) ) ) 
            {
                
            // Do not accept substed or ROM drive
            if ( ( driveInfo.iDriveAtt & KDriveAttLocal ) &&
                !( driveInfo.iDriveAtt & KDriveAttRom ) &&
                !( driveInfo.iDriveAtt & KDriveAttSubsted ) &&
                !( driveInfo.iDriveAtt & KDriveAttRemote ) )
                {
                configStoringLocationFound = ETrue;
                }
            }   
        }
           
    if ( !configStoringLocationFound )
        {
        // Central repository key was not found or was found not to be
        // alphabetic -> use the default path for storing OMA DRM rights
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );
        aFs.DriveToChar( driveNumber, aDriveLetter );
        }
            
    return configStoringLocationFound;   
    }
    


