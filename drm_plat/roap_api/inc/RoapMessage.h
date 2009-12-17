/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef ROAPMESSAGE_H
#define ROAPMESSAGE_H


//  INCLUDES
//#include <?include_file>
// #include <DrmRightsClient.h>

class CBufFlat;

namespace Roap
{

// CONSTANTS
const TInt KMaxRoapVersionLength = 10;
const TInt KDeviceNonceLength = 32;
const TInt KDomainIdLength = 20;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
enum TRoapStatus
    {
    ESuccess,
    EUnknownError,
    EAbort,
    ENotSupported,
    EAccessDenied,
    ENotFound,
    EMalformedRequest,
    EUnknownRequest,
    EUnknownCriticalExtension,
    EUnsupportedVersion,
    EUnsupportedAlgorithm,
    ENoCertificateChain,
    EInvalidCertificateChain,
    ETrustedRootCertificateNotPresent,
    ESignatureError,
    EDeviceTimeError,
    ENotRegistered,
    EInvalidDCFHash,
    EInvalidDomain,
    EDomainFull,
	EDomainAccessDenied,
	ERightsExpired,
    EMalformedMeteringReport,
    EUnableToDecryptMeteringReport,
    EUnableToValidateMeteringReportMAC,
    EUnknownStatus = -1,
    };

//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CRoapMessage : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRoapMessage* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRoapMessage();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        virtual HBufC8* MessageAsXmlL(void);
        void SetXmlData(HBufC8*);
        static HBufC8* TimeToIso8601L(TTime& aTime);
        static TTime Iso8601ToTime(TDesC8& aTimeString);
        static void BufAppendL(CBufFlat* aBuffer, const TDesC8& aConst);
        static void BufAppendBase64L(CBufFlat* aBuffer, const TDesC8& aConst);
        static void BufAppendTimeL(CBufFlat* aBuffer, TTime& aTime);

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

        /**
        * C++ default constructor.
        */
        CRoapMessage();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:

        // Prohibit copy constructor if not deriving from CBase.
        // CRoapMessage( const CRoapMessage& );
        // Prohibit assigment operator if not deriving from CBase.
        // CRoapMessage& operator=( const CRoapMessage& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        HBufC8* iXmlData;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
}

#endif      // ROAPMESSAGE_H   
            
// End of File
