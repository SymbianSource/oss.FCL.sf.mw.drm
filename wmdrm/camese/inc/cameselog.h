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
* Description:  Log functions used by the Camese subsystem
*
*/


#ifndef CAMESELOG_H
#define CAMESELOG_H

#if defined (_DEBUG)

// INCLUDES
#include <flogger.h>

// CONSTANTS
// MUST DEFINE THE FOLLOWING 2 CONSTANTS
_LIT(KCameseLogFolder,"camese");
_LIT(KCameseLogFile,"camese.txt");

/**
 * Logging class
 * This class allows debugging messages to be logged to file or RDebug port.
 * @since 3.1
 */
NONSHARABLE_CLASS(CameseLog)
    {
    public:
        /**
        * Write to the log
        * @param aFmt string
        */
        inline static void Printf1(const TDesC& aFmt);

        /**
        * Write a mulitple argument list to the log
        * @param aFmt format string
        */
        inline static void Printf(TRefByValue<const TDesC> aFmt, ...);

        /**
        * Write to the log
        * @param aFmt string
        */
        inline static void Printf1(const TDesC8& aFmt);

        /**
        * Write a mulitple argument list to the log
        * @param aFmt format string
        */
        inline static void Printf(TRefByValue<const TDesC8> aFmt, ...);

        /**
        * Hex dump to the log
        * @param aFmt format string
        */
        inline static void HexDump(const TText* aHeader, const TText* aMargin,
            const TUint8* aPtr, TInt aLen);
    };

// MACROS
#define CAMESE_LOG_TDES(fmt)            CameseLog::Printf1(fmt)
#define CAMESE_LOG(fmt)                 CameseLog::Printf1(_L(fmt))
#define CAMESE_LOG1(fmt,v1)             CameseLog::Printf(_L(fmt),v1)
#define CAMESE_LOG2(fmt,v1,v2)          CameseLog::Printf(_L(fmt),v1,v2)
#define CAMESE_LOG3(fmt,v1,v2,v3)       CameseLog::Printf(_L(fmt),v1,v2,v3)
#define CAMESE_HEX_DUMP(h, m, p, l)     CameseLog::HexDump(h, m, p, l)
#define CAMESE_LOG8(fmt)                CameseLog::Printf1(_L8(fmt))
#define CAMESE_LOG8_1(fmt,v1)           CameseLog::Printf(_L8(fmt),v1)
#define CAMESE_LOG8_2(fmt,v1,v2)        CameseLog::Printf(_L8(fmt),v1,v2)
#define CAMESE_LOG8_3(fmt,v1,v2,v3)     CameseLog::Printf(_L8(fmt),v1,v2,v3)

/**
 * Descriptor Overflow Handler for logging.
 * @since 3.1
 */
NONSHARABLE_CLASS(TLogOverflowHandler) : public TDesOverflow
    {
    public:
        inline virtual void Overflow(TDes16 &aDes);
    };

/**
 * Descriptor Overflow Handler for logging (8bit).
 * @since 3.1
 */
NONSHARABLE_CLASS(TLogOverflowHandler8) : public TDes8Overflow
    {
    public:
        inline virtual void Overflow(TDes8 &aDes);
    };

#include "cameselog.inl"

#else

#define CAMESE_LOG_TDES(fmt)
#define CAMESE_LOG(fmt)
#define CAMESE_LOG1(fmt,v1)
#define CAMESE_LOG2(fmt,v1,v2)
#define CAMESE_LOG3(fmt,v1,v2,v3)
#define CAMESE_HEX_DUMP(h, m, p, l)
#define CAMESE_LOG8(fmt)           
#define CAMESE_LOG8_1(fmt,v1)         
#define CAMESE_LOG8_2(fmt,v1,v2)     
#define CAMESE_LOG8_3(fmt,v1,v2,v3)

#endif
#endif // CAMESELOG_H

// End of File

