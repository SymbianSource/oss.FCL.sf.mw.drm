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
* Description:  Implement inline functions for the CameseLog module
*
*/


// INCLUDE FILES
#include <e32svr.h>

// ----------------------------------------------------------------------------
// Write a string to the log - 16 bit version
// ----------------------------------------------------------------------------
//
inline void CameseLog::Printf1(const TDesC& aFmt)
    {
    TInt msgSize = aFmt.Length();
    TInt offSet = 0;
    TPtrC msgPtr;
    while(offSet < msgSize)
        {
        msgPtr.Set(aFmt.Mid(offSet));
        RFileLogger::WriteFormat(KCameseLogFolder(), KCameseLogFile(),
            EFileLoggingModeAppend, msgPtr.Left(100));
        offSet += 100;  //set to 100
        };
    RDebug::Print(aFmt);
    }

// ----------------------------------------------------------------------------
// Write a multiple argument list to the log - 16 bit version
// ----------------------------------------------------------------------------
//
inline void CameseLog::Printf(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KCameseLogFolder(), KCameseLogFile(),
        EFileLoggingModeAppend, aFmt, list);
    TLogOverflowHandler overflow;
    TBuf<KLogBufferSize> tmpBuf;
    tmpBuf.AppendFormatList(aFmt, list, &overflow);
    RDebug::Print(tmpBuf);
    VA_END(list);
    }

// ----------------------------------------------------------------------------
// Write a string to the log - 8 bit version
// ----------------------------------------------------------------------------
//
inline void CameseLog::Printf1(const TDesC8& aFmt)
    {
    TInt msgSize = aFmt.Length();
    TInt offSet = 0;
    TPtrC8 msgPtr;
    while(offSet < msgSize)
        {
        msgPtr.Set(aFmt.Mid(offSet));
        RFileLogger::WriteFormat(KCameseLogFolder(), KCameseLogFile(),
            EFileLoggingModeAppend, msgPtr.Left(100));
        offSet += 100;  //set to 100
        };

    HBufC* buf16 = HBufC::NewLC(aFmt.Length());
    buf16->Des().Copy(aFmt);
    RDebug::Print(*buf16);
    CleanupStack::PopAndDestroy(buf16);
    }

// ----------------------------------------------------------------------------
// Write a multiple argument list to the log - 8 bit version
// ----------------------------------------------------------------------------
//
inline void CameseLog::Printf(TRefByValue<const TDesC8> aFmt,...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KCameseLogFolder(), KCameseLogFile(),
        EFileLoggingModeAppend, aFmt, list);

    TLogOverflowHandler8 overflow;
    TBuf8<KLogBufferSize> tmpBuf;
    tmpBuf.AppendFormatList(aFmt, list, &overflow);

    HBufC* buf16 = HBufC::NewLC(tmpBuf.Length());
    buf16->Des().Copy(tmpBuf);
    RDebug::Print(*buf16);
    CleanupStack::PopAndDestroy(buf16);

    VA_END(list);
    }

// ----------------------------------------------------------------------------
// Write a hex dump to the log
// ----------------------------------------------------------------------------
//
inline void CameseLog::HexDump(
    const TText* aHeader,
    const TText* aMargin,
    const TUint8* aPtr,
    TInt aLen)
    {
    // Hex Dumps only to file.
    RFileLogger::HexDump(KCameseLogFolder(), KCameseLogFile(), EFileLoggingModeAppend,
        aHeader, aMargin, aPtr, aLen);
    }

// ----------------------------------------------------------------------------
// Handle overflow without rising a panic
// ----------------------------------------------------------------------------
//
inline void TLogOverflowHandler::Overflow(TDes16& /*aDes*/)
    {
    return;
    }

inline void TLogOverflowHandler8::Overflow(TDes8& /*aDes*/)
    {
    return;
    }

