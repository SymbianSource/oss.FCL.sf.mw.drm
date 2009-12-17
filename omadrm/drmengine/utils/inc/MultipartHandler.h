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



#ifndef MULTIPARTHANDLER_H
#define MULTIPARTHANDLER_H

// CLASS DECLARATION

/**
*  TMultipartHandler: parses MIME multipart data
*
*  @lib    -
*  @since  3.0
*/
class TMultipartHandler
    {
public:
    static const TInt KMaxBoundaryLength = 80;

    enum TState
        {
        EPreamble,
        EBodyPartHeaders,
        EBodyPart,
        EEpilog
        };

    TMultipartHandler();
    
    void WriteL(
        const TDesC8& aData);
        
    virtual void HandleBodyHeaderDataL(
        const TDesC8& aData);

    virtual void HandleBodyDataL(
        const TDesC8& aData);

    virtual void StartBodyPartHeadersL();
    
    virtual void EndBodyPartHeadersL();
    
    virtual void StartBodyPartL();
    
    virtual void EndBodyPartL();

protected:

    void HandlePreambleL(
        const TDesC8& aData);

    void HandleBodyPartHeadersL(
        const TDesC8& aData);

    void HandleBodyPartL(
        const TDesC8& aData);

    TBool FindDelimiter(
        const TDesC8& aBuffer,
        TInt& aStart,
        TInt& aEnd);
        
    TBool FindHeaderEnd(
        const TDesC8& aBuffer,
        TInt& aStart,
        TInt& aEnd);
        
    TBool FindDashBoundary(
        const TDesC8& aBuffer, 
        TInt& aStart, 
        TInt& aEnd);
        
protected:
    
    TBuf8<KMaxBoundaryLength> iDelimiter;
    TBuf8<KMaxBoundaryLength> iBuffer;
    TState iState;
    
    };
    
#endif      // MULTIPARTHANDLER_H
            
// End of File
