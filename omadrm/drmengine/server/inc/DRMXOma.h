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
* Description:  Datatype for the X-Oma header data
*
*/


#ifndef DRMXOMA_H
#define DRMXOMA_H

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
*  CDRMXOma implements a datatype for the x-oma data for a single
*  content identifier
*
*  @lib RightsServer.dll
*  @since 2.6
*/

class CDRMXOma : public CBase 
    {
    
    public: // Constructors and destructor
        
        /**
        * NewLC
        *
        * Creates an instance of the CDRMXOma class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since    3.0
        * @param    aContentID : Content Identifier
        * @param    aTimeStamp : Time stamp the the object
        * @param    aWaitTime : The wait time in seconds to wait for the rights object
        * @return   Functional CDRMXOma object, Function leaves if an error 
        *           occurs.
        */
        static CDRMXOma* NewLC( const TDesC8& aContentID,
                                const TTime& aTimeStamp,
                                const TTimeIntervalSeconds& aWaitTime );
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMXOma class and returns a pointer 
        * to it.
        *
        * @since    3.0
        * @param    aContentID : Content Identifier
        * @param    aTimeStamp : Time stamp the the object
        * @param    aWaitTime : The wait time in seconds to wait for the rights object
        * @return   Functional CDRMXOma object, Function leaves if an error 
        *           occurs.
        */
        static CDRMXOma* NewL( const TDesC8& aContentID,
                               const TTime& aTimeStamp,
                               const TTimeIntervalSeconds& aWaitTime );
          

        
        /**
        * NewLC
        *
        * Creates an instance of the CDRMXOma class and returns a pointer 
        * to it. The function leaves the object into the cleanup stack
        *
        * @since    3.0
        * @return   Functional CDRMXOma object, Function leaves if an error 
        *           occurs.
        */
        static CDRMXOma* NewLC();
        
        /**
        * NewL
        *
        * Creates an instance of the CDRMXOma class and returns a pointer 
        * to it.
        *
        * @since    3.0
        * @return   Functional CDRMXOma object, Function leaves if an error 
        *           occurs.
        */
        static CDRMXOma* NewL();

        /**
        * Destructor
        */
        virtual ~CDRMXOma();
        
        
        /**
        * ContentID
        * 
        * Returns a constant TDesC8 reference to the content id
        *
        * @since    3.0
        * @return   a pointer to the content id or null
        */        
        const TDesC8& ContentID() const;
        
        
        /**
        * TimeStamp
        * 
        * Returns a constant TTime reference to the time stamp
        *
        * @since    3.0            
        * @return   a pointer to the rights issuer or null
        */         
        const TTime& TimeStamp() const;

        /**
        * WaitTime
        *
        * Return a constant TTimeIntervalSeconds reference to the wait time
        *
        * @since    3.0
        * @return   a pointer to the content name or null 
        */
        const TTimeIntervalSeconds& WaitTime() const;

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
        * SetTimeStampL
        * 
        * Sets the rights issuer, deletes the previous one if one
        * exits
        *
        * @since    3.0
        * @param    aTimeStamp : Timestamp of the insertion time         
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */         
        void SetTimeStampL( const TTime& aTimeStamp );
        
        
        /**
        * SetWaitTimeL
        *
        * Sets the wait time, overwrites the previous one
        *
        * @since    3.0
        * @param    aWaitTime : the wait time to replace the existing one
        * @return   The function leaves with Symbian OS error code if an
        *           error occurs
        */
        void SetWaitTimeL( const TTimeIntervalSeconds& aWaitTime );
        
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
        CDRMXOma(); 
    
        /**
        * Default Constructor - First phase.
        * @param    aTimeStamp : Time stamp the the object
        * @param    aWaitTime : The wait time in seconds to wait for the rights object        
        */
        CDRMXOma( const TTime& aTimeStamp, 
                  const TTimeIntervalSeconds& aWaitTime ); 
    
        /**
        * ConstructL
        *
        * Second phase constructor
        *
        * @since	3.0
        * @param    aContentID : Content Identifier
        * @param    aFileName : File name of the content
        * @return Functional CDRMXOma object, Function leaves if an error 
        *         occurs.
        */  
        void ConstructL( const TDesC8& aContentID );

        /**
        * Assignment operator - Prevented
        */
        CDRMXOma& operator =( const CDRMXOma& );    
    
        /**
        * Copy constructor - Prevented
        */
        CDRMXOma( const CDRMXOma& );                


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
    
        // Content identifier of the content
        HBufC8* iContentID;
        
        // Time stamp of insertion of the object
        TTime iTimeStamp;
        
        // Wait time
        TTimeIntervalSeconds iWaitTime;
    };

#endif      // DRMXOMA_H   
            
// End of File
