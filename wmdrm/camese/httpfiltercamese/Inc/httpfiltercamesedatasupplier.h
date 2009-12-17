/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Data Supplier for responses
*
*/


#ifndef HTTPFILTERCAMESEDATASUPPLIER_H
#define HTTPFILTERCAMESEDATASUPPLIER_H

//  INCLUDES
#include <http/mhttpdatasupplier.h>

// CLASS DECLARATION

/**
 *  Data supplier to supply response body
 *
 *  @since 5.0
 */
NONSHARABLE_CLASS(CHttpFilterCameseDataSupplier) : 
    public CBase, public MHTTPDataSupplier
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aOriginalDataSupplier The original data supplier
    * @param aData the content to be delivered to the http client
    * @return an instance of the data supplier
    */
    static CHttpFilterCameseDataSupplier* NewL(
        MHTTPDataSupplier* aOriginalDataSupplier,
        const TDesC8& aData );

    /**
    * Destructor.
    */
    virtual ~CHttpFilterCameseDataSupplier();

public: // Functions from base classes

    /**
    * @see MHTTPDataSupplier
    */
    virtual void ReleaseData();

    /**
    * @see MHTTPDataSupplier
    */
    virtual TInt OverallDataSize();
    
    /**
    * @see MHTTPDataSupplier
    */
    virtual TBool GetNextDataPart( TPtrC8& aDataPart );

    /**
    * @see MHTTPDataSupplier
    */
    virtual TInt Reset();

private:

    /**
    * second-phase constructor
    * @param aData the content to be delivered to the http client
    */
    void ConstructL( const TDesC8& aData );

    /**
    * C++ constructor.
    * @param aOriginalDataSupplier The original data supplier
    */
    CHttpFilterCameseDataSupplier( MHTTPDataSupplier* aOriginalDataSupplier );

private:    // Data

    HBufC8* iData;                              // owned

    // Original data supplier
    MHTTPDataSupplier* iOriginalDataSupplier;   // not owned

};

#endif      // HTTPFILTERCAMESEDATASUPPLIER_H   

// End of File
