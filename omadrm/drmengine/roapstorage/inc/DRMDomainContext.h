/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Datatype for the Common Rights Database Data
*
*/


#ifndef DRMDOMAINCONTEXT_H
#define DRMDOMAINCONTEXT_H

// INCLUDES

#include <e32base.h>
#include "RoapMessage.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CRSAPublicKey;

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

using namespace Roap;

/**
*  CDRMDomainContext implements a datatype for the common data for a single
*  content identifier
*
*  @lib RightsServer.dll
*  @since 3.0
*/

class CDRMDomainContext : public CBase
    {

    public: // Constructors and destructor

        /**
        * NewLC
        *
        * Creates an instance of the CDRMDomainContext class and returns a pointer
        * to it. The function leaves the object into the cleanup stack
        *
        * @since    3.0
        * @param    aDomainID : Domain Identifier
        * @param    aExpiryTime : Domain expiration time
        * @param    aHashChained : Does the domain support hash chaining
        * @param    aDomainKeys : a pointer array of the domain keys
        * @param    aRiId : the rights issuer ID
        * @param    aRightsIssuerURL : the rights issuer URL
        * @return   Functional CDRMDomainContext object, Function leaves if an error
        *           occurs.
        */
        IMPORT_C static CDRMDomainContext* NewLC( const TDesC8& aDomainID,
                                         const TTime& aExpiryTime,
                                         const TBool aHashChained,
                                         const RPointerArray< HBufC8 >& aDomainKeys,
                                         const TDesC8& aRiId,
                                         const TDesC8& aRightsIssuerURL );

        /**
        * NewL
        *
        * Creates an instance of the CDRMDomainContext class and returns a pointer
        * to it.
        *
        * @since    3.0
        * @param    aDomainID : Domain Identifier
        * @param    aExpiryTime : Domain expiration time
        * @param    aHashChained : Does the domain support hash chaining
        * @param    aDomainKeys : a pointer array of the domain keys
        * @param    aRiId : the rights issuer ID
        * @param    aRightsIssuerURL : the rights issuer URL
        * @return   Functional CDRMDomainContext object, Function leaves if an error
        *           occurs.
        */
        IMPORT_C static CDRMDomainContext* NewL( const TDesC8& aDomainID,
                                        const TTime& aExpiryTime,
                                        const TBool aHashChained,
                                        const RPointerArray< HBufC8 >& aDomainKeys,
                                        const TDesC8& aRiId,
                                        const TDesC8& aRightsIssuerURL );


        /**
        * NewLC
        *
        * Creates an instance of the CDRMDomainContext class and returns a pointer
        * to it. The function leaves the object into the cleanup stack
        *
        * @since  3.0
        * @return Functional CDRMDomainContext object, Function leaves if an error
        *         occurs.
        */
        IMPORT_C static CDRMDomainContext* NewLC();

        /**
        * NewL
        *
        * Creates an instance of the CDRMDomainContext class and returns a pointer
        * to it.
        *
        * @since  3.0
        * @return Functional CDRMDomainContext object, Function leaves if an error
        *         occurs.
        */
        IMPORT_C static CDRMDomainContext* NewL();

        /**
        * Default Constructor - First phase.
        * Can be used by itself to generate an empty object
        */
        CDRMDomainContext();

        /**
        * Destructor
        */
        IMPORT_C virtual ~CDRMDomainContext();


        /**
        * DomainID
        *
        * Returns a constant TDesC8& reference to the domain id
        *
        * @since    3.0
        * @return   a pointer to the content id or null
        */
        IMPORT_C const TPtrC8 DomainID() const;

        /**
        * ExpiryTime
        *
        * Returns a constant TTime reference to the expiry time
        *
        * @since    3.0
        * @return   a pointer to the content hash or null
        */
        IMPORT_C const TTime ExpiryTime() const;

        /**
        * HashChained
        *
        * Returns a constant TBool reference to the hash chained boolean
        *
        * @since    3.0
        * @return   a pointer to the rights issuer or null
        */
        IMPORT_C TBool HashChained() const;

        /**
        * DomainKeys
        *
        * Return a constant RPointerArray reference to the domain keys
        *
        * @since    3.0
        * @return   a pointer to the content name or null
        */
        IMPORT_C const RPointerArray< HBufC8 >& DomainKeys() const;

        /**
        * DomainKeyL
        *
        * Return a pointer of the domain key specified by the parameter
        * Note: pointer ownership goes to caller. I.e. Caller is responsible
        * of freeing memory reserved for the pointer.
        *
        * @since    3.0
        * @leave    KErrAgument if aGeneration is not valid
        * @return   a reference to the key
        */
        IMPORT_C HBufC8* DomainKeyL( const TInt aGeneration ) const;

        /**
        * Generation
        *
        * Returns a constant TInt reference to the domain generation
        *
        * @since    3.0
        * @return   a pointer to the rights issuer or null
        */
        IMPORT_C TInt DomainGeneration() const;


        /**
        * PublicKey
        *
        * Returns a constant reference to rights issuer ID
        *
        * @since    3.0
        * @return   the rights issuer ID
        */
        IMPORT_C const TDesC8& RightsIssuerID() const;

        /**
        * RightsIssuerURL
        *
        * Returns a constant TDesC8& reference to the rights issuer url
        *
        * @since    3.0
        * @return   a pointer to the content id or null
        */
        IMPORT_C const TDesC8& RightsIssuerURL() const;



        /**
        * SetDomainIDL
        *
        * Sets the domain id, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aDomainID : Domain identifier
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void SetDomainIDL( const TDesC8& aDomainID );

        /**
        * SetExpiryTimeL
        *
        * Sets the expiration time
        *
        * @since    3.0
        * @param    aExpiryTime : expiration time
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void SetExpiryTimeL( const TTime& aExpiryTime );

        /**
        * SetHashChainedL
        *
        * Sets the hash chained status
        *
        * @since    3.0
        * @param    aHashChained : hash chained status boolean value
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void SetHashChainedL( const TBool& aHashChained );


        /**
        * SetDomainKeysL
        *
        * Sets the domain keys, deletes the old ones if they exist
        *
        * @since    3.0
        * @param    aDomainKeys : pointer array with the domain keys
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void SetDomainKeysL( const RPointerArray< HBufC8 >& aDomainKeys );

        /**
        * SetPublicKeyL
        *
        * Sets the public key, deletes the old one if one exist
        *
        * @since    3.0
        * @param    aRightsIsserID : rights issuer ID
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void SetRightsIssuerIDL( const TDesC8& aRightsIssuerID );

        /**
        * SetRightsIssuerURLL
        *
        * Sets the rights issuer url, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aRightsIssuerURL : rights issuer url
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        IMPORT_C void SetRightsIssuerURLL( const TDesC8& aRightsIssuerURL );


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
        * Size
        *
        * Returns the size of the externalized object
        *
        * @since    3.0
        * @return   the amount of bytes the externalize object occupies
        */
        IMPORT_C TInt Size() const;

    protected:
        /**
        * Constructor
        */
        CDRMDomainContext( const TTime& aExpiryTime,
                           const TBool aHashChained );

    private:


        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since  3.0
        * @param  aContentID : Content Identifier
        * @param  aContentHash : Content Hash
        * @param  aRightsIssuer : Rights Issuer for this content
        * @return Functional CDRMDomainContext object, Function leaves if an error
        *         occurs.
        */
        void ConstructL( const TDesC8& aDomainID,
                         const RPointerArray< HBufC8 > aDomainKeys,
                         const TDesC8& aRightsIssuerID,
                         const TDesC8& aRightsIssuerURL );

        /**
        * Assignment operator - Prevented
        */
        CDRMDomainContext& operator =( const CDRMDomainContext& );

        /**
        * Copy constructor - Prevented
        */
        CDRMDomainContext( const CDRMDomainContext& );

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

    private: // Internal Data

        // Domain identifier
        TBuf8< KDomainIdLength > iDomainID;

        // Expiry time
        TTime iExpiryTime;

        // Has chained domain key support
        TBool iHashChained;

        // Domain keys
        RPointerArray< HBufC8 > iDomainKeys;

        TBuf8< SHA1_HASH > iRightsIssuerID;

        // Rights issuer URL
        HBufC8* iRightsIssuerURL;
    };

#endif      // DRMCOMMONDATA_H

// End of File
