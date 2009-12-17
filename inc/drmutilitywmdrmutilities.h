/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WM DRM specific methods for DRM Rights Manager
*
*/


#ifndef DRMUTILITYWMDRMUTILITIES_H
#define DRMUTILITYWMDRMUTILITIES_H

// CONSTANTS

// FORWARD DECLARATIONS
class CDRMRightsConstraints;

// CLASS DECLARATION
/**
*  DRMUtilityWmDrmUtilities
*
*  Collection of WM DRM specific methods
*/
class DrmUtilityWmDrmUtilities
{
    public:
        /**
        * Parse WM DRM time based rights
        * @param aLex                 Lexicographic representation of a WM DRM 
        *                             rights 
        *                             string  
        * @param aStartTime           Parsed start time from the string    
        * @param aEndTime             Parsed end time from the string                         
        */
        IMPORT_C static void ParseWmDrmTimeRightsL( TLex& aLex, 
                                                    TTime& aStartTime,
                                                    TTime& aEndTime );       
    
        /**
         * Parse WM DRM count based rights
         * @param aLex                 Lexicographic representation of a WM DRM 
         *                             rights string
         * @param aCounts              The amount of count rights  
         * @return                     Whether count information was found or not                         
         */
         IMPORT_C static TBool ParseWmDrmCountRightsL( TLex& aLex, 
                                                       TUint32& aCounts );
       
        /**
         * Parse WM DRM duration rights
         * @param aLex                 Lexicographic representation of a WM DRM 
         *                             rights string  
         * @param aDuration            The duration of the rights
         * @return                     Whether duration information was found 
         *                             or not                       
         */
         IMPORT_C static TBool ParseWmDrmDurationRightsL( TLex& aLex,
            TTimeIntervalSeconds& aDuration );
    
        /**
         * Parse a WM DRM rights string to a rights constraint. Caller is responsible
         * for freeing the memory reserved for the constraint.
         * @param aRights              String to be parsed
         * @param aRightsConstraint    Rights constraint containing parsed rights
         */
         IMPORT_C static void ParseWmDrmStringL( ContentAccess::CRightsInfo& aRights,
            CDRMRightsConstraints*& aRightsConstraint );
       
        /** Checks the detailed rights of the WMDRM protected file from the
         * given array parameter. The other function parameter values are updated
         * based on the rights.
         * @param aUnconstrained       Whether the file has unlimited rights or not
         * @param aTime                Time left to use the file
         * @param aCounts              Counts left to use the file
         * @param aRights              WM DRM rights string which needs to parsed
         */
         IMPORT_C static void CheckWmDrmRightsL( TBool& aUnconstrained, 
                                                 TTimeIntervalSeconds& aTime, 
                                                 TInt& aCounts, 
                                                 ContentAccess::CRightsInfo& aRights );
                                           
    private:
                                     
};

#endif

// End of File