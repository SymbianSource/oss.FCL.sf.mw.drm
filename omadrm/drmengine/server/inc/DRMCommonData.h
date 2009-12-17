/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef DRMCOMMONDATA_H
#define DRMCOMMONDATA_H

// INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// TYPE DEFINITIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  CDRMCommonData implements a datatype for the common data for a single
*  content identifier
*
*  @lib RightsServer.dll
*  @since 3.0
*/

class CDRMCommonData : public CBase 
    {
    
    public: // Constructors and destructor
        
        /**
        * NewLC
        *
        * Creates an instance of the CDRMCommonData class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since  3.0
        * @param  aContentID : Content Identifier
        * @param  aContentHash : Content Hash
        * @param  aRightsIssuer : Rights Issuer for this content
        * @return Functional CDRMCommonData object, Function leaves if an error 
        *         occurs.
        */
        static CDRMCommonData* NewLC( const TDesC8& aContentID,
                                      const TDesC8& aContentHash,
                                      const TDesC8& aRightsIssuer,
                                      const TDesC& aContentName,
                                      const TDesC8& aAuthenticationSeed );
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMCommonData class and returns a pointer 
        * to it.
        *
        * @since  3.0
        * @param  aContentID : Content Identifier
        * @param  aContentHash : Content Hash
        * @param  aRightsIssuer : Rights Issuer for this content
        * @return Functional CDRMCommonData object, Function leaves if an error 
        *         occurs.
        */
        static CDRMCommonData* NewL( const TDesC8& aContentID,
                                     const TDesC8& aContentHash,
                                     const TDesC8& aRightsIssuer,
                                     const TDesC& aContentName,
                                     const TDesC8& aAuthenticationSeed );
                                     
        /**
        * NewLC
        *
        * Creates an empty instance of the CDRMCommonData class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since  3.0
        * @return Functional CDRMCommonData object, Function leaves if an error 
        *         occurs.
        */
        static CDRMCommonData* NewLC();                                     
                                     
        /**
        * NewL
        *
        * Creates an empty instance of the CDRMCommonData class and returns a pointer 
        * to it.
        *
        * @since  3.0
        * @return Functional CDRMCommonData object, Function leaves if an error 
        *         occurs.
        */
        static CDRMCommonData* NewL();                                     

          
        /**
        * Destructor
        */
        virtual ~CDRMCommonData();
        
        
        /**
        * ContentID
        * 
        * Returns a constant HBufC8 pointer to the content id
        *
        * @since    3.0
        * @return   a pointer to the content id or null
        */        
        const TDesC8& ContentID() const;
        
        /**
        * ContentHash
        * 
        * Returns a constant HBufC8 pointer to the content hash
        *
        * @since    3.0      
        * @return   a pointer to the content hash or null
        */        
        const TDesC8& ContentHash() const;
        
        /**
        * RightsIssuer
        * 
        * Returns a constant HBufC8 pointer to the rights issuer
        *
        * @since    3.0            
        * @return   a pointer to the rights issuer or null
        */         
        const TDesC8& RightsIssuer() const;

        /**
        * CotentName
        *
        * Return a constant HBufC pointer to the content name
        *
        * @since    3.0
        * @return   a pointer to the content name or null 
        */
        const TDesC& ContentName() const;

        /**
        * AuthenticationSeed
        * 
        * Returns a constant HBufC8 pointer to the authentication seed
        *
        * @since    3.0            
        * @return   a pointer to the authentication seed or null
        */         
        const TDesC8& AuthenticationSeed() const;

        /**
        * SetContentIDL
        * 
        * Sets the content id, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aContentID : Content Identifier        
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */        
        void SetContentIDL( const TDesC8& aContentID );
        
        /**
        * SetContentHashL
        * 
        * Sets the content hash, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aContentHash : Content Hash             
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */        
        void SetContentHashL( const TDesC8& aContentHash );
        
        /**
        * SetRightsIssuerL
        * 
        * Sets the rights issuer, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aRightsIssuer : Rights Issuer for this content         
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */         
        void SetRightsIssuerL( const TDesC8& aRightsIssuer );
        
        
        /**
        * SetContentNameL
        *
        * Sets the content name, deletes the previous one if it exists
        *
        * @since    3.0
        * @param    aContentName : The name that is given to the content
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        void SetContentNameL( const TDesC& aContentName );

        /**
        * SetAuthenticationSeedL
        * 
        * Sets the authentication seed, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aAuthenticationSeed : Rights Issuer for this content         
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */         
        void SetAuthenticationSeedL( const TDesC8& aAuthenticationSeed );

        
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
        void ExternalizeL( RWriteStream& aStream ) const;
        
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
        void InternalizeL( RReadStream& aStream );   
       
        
        /**
        * Size
        *
        * Returns the size of the externalized object
        *
        * @since    3.0
        * @return   the amount of bytes the externalize object occupies
        */           
        TInt Size() const;
        
    private:
        /**
        * Default Constructor - First phase.
        */
        CDRMCommonData(); 
    
        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since  3.0
        * @param  aContentID : Content Identifier
        * @param  aContentHash : Content Hash
        * @param  aRightsIssuer : Rights Issuer for this content
        * @return Functional CDRMCommonData object, Function leaves if an error 
        *         occurs.
        */  
        void ConstructL( const TDesC8& aContentID,
                         const TDesC8& aContentHash,
                         const TDesC8& aRightsIssuer,
                         const TDesC& aContentName,
                         const TDesC8& aAuthenticationSeed );

        /**
        * Assignment operator - Prevented
        */
        CDRMCommonData& operator =( const CDRMCommonData& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMCommonData( const CDRMCommonData& );                


    private: // Internal Data
    
        // Content identifier of the content
        HBufC8* iContentID;
        
        // Hash of the content DCF
        HBufC8* iContentHash;
        
        // Issuer of the rights
        HBufC8* iRightsIssuer;
        
        // Content name
        HBufC* iContentName;
        
        // Authentication seed
        HBufC8* iAuthenticationSeed;
    };

#endif      // DRMCOMMONDATA_H   
            
// End of File
