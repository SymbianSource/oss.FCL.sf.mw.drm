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
* Description:  Client side implementation
*
*/



#ifndef RDCFREPCLI_H
#define RDCFREPCLI_H

//  INCLUDES
#include <e32base.h>
// CONSTANTS
// MACROS
// DATA TYPES
typedef struct TPair // interface structure
    {
    HBufC8* iCid; // Caller owns it, initial value must be given (for example: NULL)
    HBufC8* iTtid; // Caller owns it, initial value must be given (for example: NULL)
    }TPair;
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class RFile;


// CLASS DECLARATION

/**
*  RDcfRepCli
*  Client of DCF Repository
*
*  @lib 
*  @since Series 60 3.0
*/
class RDcfRepCli : public RSessionBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C RDcfRepCli();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~RDcfRepCli();

    public: // New functions
        
        
        /**
        * AddFile
        * The function add a file into dababase. server will check file info and add them into database
        * @since Series 60 3.0 
        * @param aFile filename
        * @return KErrNotSupported if file is not protected file
        */
        IMPORT_C TInt AddFile(const TDesC& aFile);
        
        /**
        * Connect
        * The function try to connect DcfRepSrv server.
        * @since Series 60 3.0 
        * @param 
        * @return errors if failed to connect to server 
        */
        IMPORT_C TInt Connect();
        
        
        /**
        * Close
        * This function closes the connection between the client and the server.
        * It is safe to call this method even if connection is not established.
        * @since Series 60 3.0 
        * @param 
        * @return errors if failed to connect to server 
        */
        IMPORT_C void Close();    
     
          
        /**
        * Version 
        * Server version
        * @since Series 60 3.0
        * @param 
        * @return version of server
        */        
        IMPORT_C TVersion Version() const;

        /**
        * ScanDcf
        * This function will triger the file scanning on the server side for all DCFs
        * @since Series 60 3.0 
        * @param aStatus status of this request
        * @return 
        */
        IMPORT_C void ScanDcf(TRequestStatus& aStatus);
        
        /**
        * ScanDcf
        * The function will scan Dcfs for a list of content ID, 
        * when all content ID are paired with at least one file, scanning get stopped.
        * @since Series 60 3.0 
        * @param aList a list of content ID
        * @param aStatus status of this request
        * @return 
        */
        IMPORT_C void ScanDcf(  
            RPointerArray<HBufC8>& aList , 
            TRequestStatus& aStatus );
                
        /**
        * SetTtid
        * This function is an asynchronous call. It goes through whole file system 
        * to replace old Ttid pairs(transaction ID and dcf)
        * @since Series 60 3.0 
        * @param aStatus status of this request
        * @return 
        */
        IMPORT_C void SetTtid( 
            RArray<TPair>& aList , 
            TRequestStatus& aStatus 
            );                
        
        /**
        * NotifyServerIdle
        * This function will notify when server becomes idle
        * @since Series 60 3.0 
        * @param aStatus status of this request
        * @return 
        */            
        IMPORT_C void NotifyServerIdle( TRequestStatus& aStatus );

        /**
        * StopWatching
        * Tell the server to stop watching the rights server
        * @since Series 60 3.1
        */
        IMPORT_C void StopWatching();
        
    public: // Functions from base classes
    protected:  // New functions
    protected:  // Functions from base classes
    private:

        // Prohibit copy constructor if not deriving from CBase.
        RDcfRepCli( const RDcfRepCli& );
        // Prohibit assigment operator if not deriving from CBase.
        RDcfRepCli& operator=( const RDcfRepCli& );

    public:     // Data
    protected:  // Data
    private:    // Data
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // RDCFREPCLI_H   
            
// End of File
