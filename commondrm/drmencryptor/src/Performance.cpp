/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <caf/caf.h>
#include <caf/cafplatform.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32file.h>
#include <Oma1DcfCreator.h>
#include <DRMMessageParser.h>
#include <e32math.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include "Base64.h"
#include "Performance.h"
#include "DRMEncryptor.hrh"

_LIT(KLogDir, "DRM");
_LIT(KLogName, "Performance.log");
#include "flogger.h"
#define LOG(string) \
    RFileLogger::Write(KLogDir, KLogName, \
        EFileLoggingModeAppend, string);
#define LOG2(string, a) \
    RFileLogger::WriteFormat(KLogDir, KLogName, \
        EFileLoggingModeAppend, string, a);
#define LOGHEX(buffer) \
    RFileLogger::HexDump(KLogDir, KLogName, \
        EFileLoggingModeAppend, _S(""), _S(""), \
        buffer.Ptr(), buffer.Length());

_LIT8(KMidiContent,
"TVRoZAAAAAYAAQAGAHhNVHJrAAAAGQD/WAQEAhgIAP9ZAgAAAP9RAwehIAD/LwBN\n\
VHJrAAAKsgD/IQEAALkHaQCZLmQAM2QAI2QOIwAAMwAALgAuM2QOMwAuKGQAM2QO\n\
MwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAu\n\
M2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuI2QAM2QO\n\
MwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuMWQAM2QO\n\
MwAAMQAuLmQAM2QAI2QOIwAAMwAALgAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAu\n\
I2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QO\n\
MwAAIwAuI2QAM2QOMwAAIwAuMmQAM2QAKGQOKAAAMwAAMgAuMmQAI2QAM2QOMwAA\n\
IwAAMgAuMGQAI2QAM2QOMwAAIwAAMAAuMGQAI2QAM2QOMwAAIwAAMAAuMmQAM2QA\n\
KGQOKAAAMwAAMgAQMmQOMgAQMGQAM2QOMwAAMAAQLWQOLQAQMWQALmQAM2QAI2QO\n\
IwAAMQAALgAAMwAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAu\n\
I2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuM2QAKGQOKAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuM2QAKGQOKAAAMwAuMWQAM2QOMwAAMQAuLmQAM2QAI2QOIwAAMwAALgAu\n\
M2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAu\n\
KGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQO\n\
KAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQO\n\
KAAAMwAuMWQAM2QOMwAAMQAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuMmQAM2QA\n\
KGQOKAAAMwAAMgAuMmQAI2QAM2QOMwAAIwAAMgAuMGQAI2QAM2QOMwAAIwAAMAAu\n\
MGQAI2QAM2QOMwAAIwAAMAAuMmQAM2QAKGQOKAAAMwAAMgAQMmQOMgAQMGQAM2QO\n\
MwAAMAAQLWQOLQAQMWQALmQAM2QAI2QOIwAAMQAALgAAMwAuM2QOMwAuKGQAM2QO\n\
MwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAu\n\
M2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuI2QAM2QO\n\
MwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuMWQAM2QO\n\
MwAAMQAuLmQAM2QAI2QOIwAAMwAALgAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAu\n\
I2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuMWQALmQA\n\
M2QAI2QOIwAAMQAALgAAMwAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QO\n\
MwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAu\n\
I2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAu\n\
I2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuMWQAM2QOMwAAMQAuLmQAM2QAI2QOIwAA\n\
MwAALgAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAu\n\
M2QAKGQOKAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAu\n\
M2QAKGQOKAAAMwAuMWQAM2QOMwAAMQAuMWQALmQAM2QAI2QOIwAAMQAALgAAMwAu\n\
M2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAu\n\
KGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQO\n\
KAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQO\n\
KAAAMwAuMWQAM2QOMwAAMQAuLmQAM2QAI2QOIwAAMwAALgAuM2QOMwAuKGQAM2QO\n\
MwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAu\n\
M2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuMmQAM2QAKGQOKAAAMwAAMgAu\n\
MmQAI2QAM2QOMwAAIwAAMgAuMGQAI2QAM2QOMwAAIwAAMAAuMGQAI2QAM2QOMwAA\n\
IwAAMAAuMmQAM2QAKGQOKAAAMwAAMgAQMmQOMgAQMGQAM2QOMwAAMAAQLWQOLQAQ\n\
MWQALmQAM2QAI2QOIwAAMQAALgAAMwAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAu\n\
I2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QO\n\
MwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQOKAAAMwAuMWQAM2QOMwAAMQAuLmQAM2QA\n\
I2QOIwAAMwAALgAuM2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAu\n\
I2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuM2QAKGQOKAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuM2QAKGQOKAAAMwAuMWQAM2QOMwAAMQAuI2QAM2QOMwAAIwAuI2QAM2QO\n\
MwAAIwAuMmQAM2QAKGQOKAAAMwAAMgAuMmQAI2QAM2QOMwAAIwAAMgAuMGQAI2QA\n\
M2QOMwAAIwAAMAAuMGQAI2QAM2QOMwAAIwAAMAAuMmQAM2QAKGQOKAAAMwAAMgAQ\n\
MmQOMgAQMGQAM2QOMwAAMAAQLWQOLQAQMWQALmQAM2QAI2QOIwAAMQAALgAAMwAu\n\
M2QOMwAuKGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAu\n\
KGQAM2QOMwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQO\n\
KAAAMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuM2QAKGQO\n\
KAAAMwAuMWQAM2QOMwAAMQAuLmQAM2QAI2QOIwAAMwAALgAuM2QOMwAuKGQAM2QO\n\
MwAAKAAuM2QOMwAuI2QAM2QOMwAAIwAuI2QAM2QOMwAAIwAuKGQAM2QOMwAAKAAu\n\
M2QOMwAA/y8ATVRyawAABTMA/yEBAADAIwCwB2kAkCRkHSQAHyRkDiQALiRkHSQA\n\
HyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQA\n\
HyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQA\n\
HyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQA\n\
HyRkDiQALiRkHSQAHyRkDiQALiRkHSQAWyRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAWyRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQA\n\
HyRkDiQALiRkHSQAWyRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALitkHSsAHytkDisALitkHSsAHytkDisALitkHSsAHytkDisA\n\
LitkHSsAHytkDisALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALilkHSkAHylkDikALilkHSkAHylkDikALilkHSkAHylkDikA\n\
LilkHSkAHylkDikALitkHSsAHytkDisALitkHSsAHytkDisALitkHSsAHytkDisA\n\
LitkHSsAHyRkDiQALilkHSkAHylkDikALilkHSkAHylkDikALilkHSkAHylkDikA\n\
LilkHSkAHy1kDi0ALitkHSsAHytkDisALitkHSsAHytkDisALitkHSsAHytkDisA\n\
LitkHSsAHytkDisALilkHSkAHylkDikALilkHSkAHylkDikALilkHSkAHylkDikA\n\
LilkHSkAHylkDikALitkHSsAHytkDisALitkHSsAHytkDisALitkHSsAHytkDisA\n\
LitkHSsAHytkDisALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LiRkHSQAWyRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQA\n\
WyRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
LitkHSsAHytkDisALitkHSsAHytkDisALitkHSsAHytkDisALitkHSsAHytkDisA\n\
LiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQALiRkHSQAHyRkDiQA\n\
AP8vAE1UcmsAAAEeAP8hAQAAwQGOCJFTZABUZA5UAABTAAFPZABRZA5RAABPAAFM\n\
ZABNZA5NAABMAAFIZABKZA5KAABIAAFFZABHZA5HAABFAAFBZABDZA5DAABBAAE+\n\
ZABAZA5AAAA+AAE8ZA48AJ0JU2QAVGQOVAAAUwABT2QAUWQOUQAATwABTGQATWQO\n\
TQAATAABSGQASmQOSgAASAABRWQAR2QORwAARQABQWQAQ2QOQwAAQQABPmQAQGQO\n\
QAAAPgABPGQOPAC7CVNkAFRkDlQAAFMAAU9kAFFkDlEAAE8AAUxkAE1kDk0AAEwA\n\
AUhkAEpkDkoAAEgAAUVkAEdkDkcAAEUAAUFkAENkDkMAAEEAAT5kAEBkDkAAAD4A\n\
ATxkDjwAAP8vAE1UcmsAAAQ9AP8hAQAAwh4AsgdkjwCSTGQANGQAMGR3MAAANAAA\n\
TAABNGQAMGQdMAAANAAfLmQPRmQsRgAALgABSmQANGQAL2Q7LwAANAAASgABSmQA\n\
NGQAL2Q7LwAANAAASgA9SGQATGQAMGQANGR3NAAASAAATAAAMACCLS5kOy4AAS9k\n\
Oy8AAUxkADBkADRkdzQAADAAAEwAATRkADBkHTAAADQAH0hkAC5kOy4AAEgAAVFk\n\
ADlkADVkOzUAADkAAFEAAVBkADlkADVkOzUAADkAAFAAAUxkHUwAAU1kHU0AAVRk\n\
AExkADdkADRkdzQAADcAAEwAgyRUAAFMZAA0ZAAwZHcwAAA0AABMAAE0ZAAwZB0w\n\
AAA0AB8uZA9GZCxGAAAuAAFKZAA0ZAAvZDsvAAA0AABKAAFKZAA0ZAAvZDsvAAA0\n\
AABKAD1IZABMZAAwZAA0ZHc0AABIAABMAAAwAIMlR2QAQ2QAO2QAN2R3NwAARwAA\n\
QwAAOwABQ2QAR2QAO2QAN2QdNwAAQwAARwAAOwAfSmQAPmQ7PgAASgABSGQAPGR3\n\
PAAASAABT2QAN2QAO2Q7OwAANwAATwA9VGSDX1QAATlkADVkdzUAADkAPUhkO0gA\n\
AUdkd0cAPUVkO0UAAUdkd0cAgXEwZHcwAAE5ZAA1ZHc1AAA5AD1IZDtIAAFKZHdK\n\
AD1IZDtIAAFHZDtHAIMpOWQANWR3NQAAOQA9SGQ7SAABSmR3SgA9SGQ4NGQAN2QA\n\
T2QATGQDSAA4TwAANAAANwAATAA9NGQAN2QAT2QATGQ7TAAANAAANwAATwA9MmQA\n\
SmQ7SgAAMgABMGQASGQ7SAAAMAABL2QAR2Q7RwAALwABMGQASGQ7MACHQEgAAUxk\n\
ADRkADBkdzAAADQAAEwAATRkADBkHTAAADQAHy5kD0ZkLEYAAC4AAUpkADRkAC9k\n\
Oy8AADQAAEoAAUpkADRkAC9kOy8AADQAAEoAPUhkAExkADBkADRkdzQAAEgAAEwA\n\
ADAAgi0uZDsuAAEvZDsvAAFMZAAwZAA0ZHc0AAAwAABMAAE0ZAAwZB0wAAA0AB9I\n\
ZAAuZDsuAABIAAFRZAA5ZAA1ZDs1AAA5AABRAAFQZAA5ZAA1ZDs1AAA5AABQAAFM\n\
ZB1MAAFNZB1NAAFUZABMZAA3ZAA0ZHc0AAA3AABMAIMkVAABTGQANGQAMGR3MAAA\n\
NAAATAABNGQAMGQdMAAANAAfLmQPRmQsRgAALgABSmQANGQAL2Q7LwAANAAASgAB\n\
SmQANGQAL2Q7LwAANAAASgA9SGQATGQAMGQANGR3NAAASAAATAAAMACDJUdkAENk\n\
ADtkADdkdzcAAEcAAEMAADsAAUNkAEdkADtkADdkHTcAAEMAAEcAADsAH0pkAD5k\n\
Oz4AAEoAAUhkADxkdzwAAEgAAU9kADdkADtkOzsAADcAAE8APVRkg19UAAD/LwBN\n\
VHJrAAAAWAD/IQEAAMNQALMHeK0AkzVkg181AAE3ZINfNwABNWSDXzUAATdkg183\n\
AAE1ZINfNQABN2SDXzcAATBkhz8wAAEwZJY/MAABN2SDXzcAATBkg18wAAD/LwA=\n");

_LIT8(KContentHeader, "--boundary\r\nContent-type: audio/midi\r\nContent-Transfer-Encoding: base64\r\n\r\n");
_LIT8(KCdStartEndHeader,
"--boundary\r\n\
Content-Type: application/vnd.oma.drm.rights+xml\r\n\
Content-Transfer-Encoding: binary\r\n\
\r\n\
<o-ex:rights\
   xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\"\
   xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\"\
   xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"\
>\
<o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context>\
<o-ex:agreement><o-ex:asset><o-ex:context>\
<o-dd:uid>cid:content0000@localhost</o-dd:uid>\
</o-ex:context></o-ex:asset>\
<o-ex:permission><o-dd:play><o-ex:constraint>\
<o-dd:datetime>\
<o-dd:end>2020-01-01T00:00:00</o-dd:end><o-dd:start>1980-01-01T00:00:00</o-dd:start>\
</o-dd:datetime>\
</o-ex:constraint></o-dd:play></o-ex:permission>\
</o-ex:agreement></o-ex:rights>\r\n\
\r\n");
_LIT8(KCdCountHeader,
"--boundary\r\n\
Content-Type: application/vnd.oma.drm.rights+xml\r\n\
Content-Transfer-Encoding: binary\r\n\
\r\n\
<o-ex:rights\
   xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\"\
   xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\"\
   xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"\
>\
<o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context>\
<o-ex:agreement><o-ex:asset><o-ex:context>\
<o-dd:uid>cid:content0001@localhost</o-dd:uid>\
</o-ex:context></o-ex:asset>\
<o-ex:permission><o-dd:play><o-ex:constraint>\
<o-dd:count>1000000</o-dd:count>\
</o-ex:constraint></o-dd:play></o-ex:permission>\
</o-ex:agreement></o-ex:rights>\r\n\
\r\n");
_LIT8(KFooter, "\r\n--boundary--\r\n");

const TInt KFileNumber = 50;
const TInt KOpenCount = 10;

#ifdef __WINS__
_LIT(KFilesDir, "c:\\data\\others\\DrmTest\\");
#else
#ifdef RD_MULTIPLE_DRIVE
_LIT(KFilesDir, "%c:\\others\\DrmTest\\");
#else
_LIT(KFilesDir, "e:\\others\\DrmTest\\");
#endif
#endif
_LIT(KFlFileNameBase, "-fl");
_LIT(KPlainFileNameBase, "-plain.mid");
_LIT(KCdStartEndFileNameBase, "-cd-se");
_LIT(KCdCountFileNameBase, "-cd-count");
_LIT(KFileSuffix, ".dcf");
_LIT(KBigFilePlain, "bigfile.txt");
_LIT(KBigFileEncrypted, "bigfile.dcf");

enum ETestFileType
    {
    EFl,
    ECdStartEnd,
    ECdCount,
    EPlain
    };

void SetupDirectoryL(const TDesC& aDir)
    {
    RFs fs;
    TFileName fileName;
    CFileMan* fm = NULL;

    LOG(_L("SetupDirectoryL"));
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fm = CFileMan::NewL(fs);
    CleanupStack::PushL(fm);
    fm->RmDir(aDir);
    fs.MkDirAll(aDir);
    CleanupStack::PopAndDestroy(2); // fm, fs
    }

void CreateFileL(RFs& aFs, CDRMMessageParser* aParser, ETestFileType aType, TInt aNumber)
    {
    TFileName fileName;
    RFileWriteStream out;
    HBufC8* buffer = NULL;

    fileName.AppendNum(aNumber);
    switch (aType)
        {
        case EPlain:
            fileName.Append(KPlainFileNameBase);
            break;
        case EFl:
            fileName.Append(KFlFileNameBase);
            fileName.Append(KFileSuffix);
            break;
        case ECdStartEnd:
            fileName.Append(KCdStartEndFileNameBase);
            fileName.Append(KFileSuffix);
            break;
        case ECdCount:
            fileName.Append(KCdCountFileNameBase);
            fileName.Append(KFileSuffix);
            break;
        }
    out.Create(aFs, fileName, EFileWrite);
    CleanupClosePushL(out);
    if (aType != EPlain)
        {
        aParser->InitializeMessageParserL(out);
        switch (aType)
            {
            case ECdStartEnd:
                aParser->ProcessMessageDataL(KCdStartEndHeader);
                break;
            case ECdCount:
                aParser->ProcessMessageDataL(KCdCountHeader);
                break;
            }
        aParser->ProcessMessageDataL(KContentHeader);
        aParser->ProcessMessageDataL(KMidiContent);
        aParser->ProcessMessageDataL(KFooter);
        aParser->FinalizeMessageParserL();
        }
    else
        {
        buffer = Base64DecodeL(KMidiContent);
        CleanupStack::PushL(buffer);
        out.WriteL(*buffer);
        CleanupStack::PopAndDestroy(); // buffer
        }
    CleanupStack::PopAndDestroy(); // out
    }

void GenerateFilesL(const TDesC& aDir, TInt aCount)
    {
    CDRMMessageParser* parser = NULL;
    TInt i;
    RFs fs;

    LOG(_L("GenerateFilesL"));
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fs.SetSessionPath(aDir);
    parser = CDRMMessageParser::NewL();
    CleanupStack::PushL(parser);
    for (i = 0; i < aCount; i++)
        {
        CreateFileL(fs, parser, EPlain, i);
        CreateFileL(fs, parser, EFl, i);
        CreateFileL(fs, parser, ECdStartEnd, i);
        CreateFileL(fs, parser, ECdCount, i);
        }
    CleanupStack::PopAndDestroy(2); // parser, fs
    }


void GenerateBigFilesL(const TDesC& aDir)
    {
    RFs fs;
    RFileWriteStream out;
    COma1DcfCreator* creator = NULL;
    HBufC8* buffer = NULL;
    TPtr8 ptr(NULL, 0);
    TInt i;

    LOG(_L("GenerateBigFilesL"));
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fs.SetSessionPath(aDir);
    creator = COma1DcfCreator::NewL();
    CleanupStack::PushL(creator);

    buffer = HBufC8::NewMax(50 * 1024);
    CleanupStack::PushL(buffer);
    ptr.Set(buffer->Des());
    ptr.Fill('0');

    LOG(_L("Creating plain file"));
    fs.Delete(KBigFilePlain);
    out.Create(fs, KBigFilePlain, EFileWrite);
    CleanupClosePushL(out);
    for (i = 0; i < 40; i++)
        {
        out.WriteL(ptr);
        }
    CleanupStack::PopAndDestroy(); // out

    LOG(_L("Creating encrypted file"));
    fs.Delete(KBigFileEncrypted);
    out.Create(fs, KBigFileEncrypted, EFileWrite);
    CleanupClosePushL(out);
    creator->EncryptInitializeL(out, _L8("text/plain"), NULL);
    for (i = 0; i < 40; i++)
        {
        creator->EncryptUpdateL(ptr);
        }
    creator->EncryptFinalizeL();
    CleanupStack::PopAndDestroy(4); // out, creator, buffer, fs
    }


void TestDecryptionSpeedL(const TDesC& aDir, TBool aRandom, TInt aType)
    {
    TInt i;
    TInt j;
    HBufC8* buffer = NULL;
    RFs fs;
    TPtr8 ptr(NULL, 0);
    CData* data = NULL;
    RFile file;
    TInt blockSize[5] = {64, 512, 1024, 2048, 4096};
    TFileName fileName;
    TInt pos = 0;

    LOG(_L("TestDecryptionSpeedL"));
    if (aRandom)
        {
        LOG(_L("Random Reading"));
        }
    else
        {
        LOG(_L("Sequential Reading"));
        }
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fs.SetSessionPath(aDir);

    buffer = HBufC8::NewL(4096);
    CleanupStack::PushL(buffer);
    ptr.Set(buffer->Des());
    for (j = 0; j < 5; j++)
        {
        LOG2(_L("Block size: %d"), blockSize[j]);

        switch (aType)
            {
            case 1: //Plain file with RFile
                {
                LOG(_L("Reading plain file (RFile)"));
                User::LeaveIfError(file.Open(fs, KBigFilePlain, EFileRead));
                CleanupClosePushL(file);
                for (i = 0; i < 40 * 50 * 1024 / blockSize[j]; i++)
                    {
                    if (aRandom)
                        {
                        pos = Abs(Math::Random() % (40 * 50 * 1024 - 2 * blockSize[j]));
                        file.Seek(ESeekStart, pos);
                        }
                    file.Read(ptr, blockSize[j]);
                    }
                CleanupStack::PopAndDestroy(); // file
                LOG2(_L("Reading plain file (RFile) done (%d blocks)"), i);
                }
                break;
            case 2: //Plain file with CAF
                {
                LOG(_L("Reading plain file (CAF)"));
                fileName.Copy(aDir);
                fileName.Append(KBigFilePlain);
                data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                EPeek, EContentShareReadOnly);
                CleanupStack::PushL(data);
                for (i = 0; i < 40 * 50 * 1024 / blockSize[j]; i++)
                    {
                    if (aRandom)
                        {
                        pos = Abs(Math::Random() % (40 * 50 * 1024 - 2 * blockSize[j]));
                        data->Seek(ESeekStart, pos);
                        }
                    data->Read(ptr, blockSize[j]);
                    }
                CleanupStack::PopAndDestroy(); // data
                LOG2(_L("Reading plain file (CAF) done (%d blocks)"), i);
                }
                break;
            case 3: //DRM Protected on server side
                {
                LOG(_L("Reading encrypted file (server decryption)"));
                fileName.Copy(aDir);
                fileName.Append(KBigFileEncrypted);
                __UHEAP_MARK;
                data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                EPeek, EContentShareReadOnly);
                CleanupStack::PushL(data);
                data->ExecuteIntent(EView); //!!!!!!!!!!!!!
                for (i = 0; i < 40 * 50 * 1024 / blockSize[j]; i++)
                    {
                    if (aRandom)
                        {
                        pos = Abs(Math::Random() % (40 * 50 * 1024 - 2 * blockSize[j]));
                        data->Seek(ESeekStart, pos);
                        }
                    data->Read(ptr, blockSize[j]);
                    }

                CleanupStack::PopAndDestroy(); // data
                __UHEAP_MARKEND;
                LOG2(_L("Reading encrypted file done (%d blocks)"), i);

                // DRM protected on client side
                LOG(_L("Reading encrypted file (client decryption)"));
                fileName.Copy(aDir);
                fileName.Append(KBigFileEncrypted);
                __UHEAP_MARK;
                data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                    EPeek, EContentShareReadOnly);
                CleanupStack::PushL(data);
                data->ExecuteIntent(EView);
                for (i = 0; i < 40 * 50 * 1024 / blockSize[j]; i++)
                    {
                    if (aRandom)
                        {
                        pos = Math::Random() % (40 * 50 * 1024 - blockSize[j]);
                        data->Seek(ESeekStart, pos);
                        }
                    data->Read(ptr, blockSize[j]);
                    }
                CleanupStack::PopAndDestroy(); // data
                __UHEAP_MARKEND;
                LOG2(_L("Reading encrypted file done (%d blocks)"), i);
                }
                break;
            }
        }
    CleanupStack::PopAndDestroy(); // buffer

    CleanupStack::PopAndDestroy(); // fs
    }

void TestFileOpeningSpeedL(const TDesC& aDir, TInt aType)
    {
    TFileName fileName;
    TInt j;
    TInt i;
    CData* data = NULL;
    RFs fs;
    RFile file;

    LOG2(_L("TestFileOpeningSpeedL (%d files)"), KFileNumber * KOpenCount);

    switch( aType)
        {
        case 1: // With RFile
            {
            LOG(_L("Opening plain files (with RFs::Connect)"));
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KFlFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    User::LeaveIfError(fs.Connect());
                    CleanupClosePushL(fs);
                    User::LeaveIfError(file.Open(fs, fileName, EFileRead));
                    file.Close();
                    CleanupStack::PopAndDestroy();
                    }
                }
            LOG(_L("Opening plain files done"));

            LOG(_L("Opening plain files (without RFs::Connect)"));
            User::LeaveIfError(fs.Connect());
            CleanupClosePushL(fs);
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KFlFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    User::LeaveIfError(file.Open(fs, fileName, EFileRead));
                    file.Close();
                    }
                }
            CleanupStack::PopAndDestroy();
            LOG(_L("Opening plain files done"));
            }
            break;

        case 2: //With CAF
            {

            LOG(_L("Opening plain files (CAF)"));
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KPlainFileNameBase);
                for (j = 0; j < KOpenCount; j++)
                    {
                    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                    EPlay, EContentShareReadOnly);
                    delete data;
                    }
                }


            LOG(_L("Opening plain files done"));

            User::After(3000000);

            LOG(_L("Opening plain files CAF with Filehandle"));

            User::LeaveIfError(fs.Connect());
            CleanupClosePushL(fs);
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KPlainFileNameBase);
                for (j = 0; j < KOpenCount; j++)
                    {
                    User::LeaveIfError(file.Open(fs, fileName, EFileRead | EFileShareAny));
                    CleanupClosePushL(file);
                    data = CData::NewL(file, KDefaultContentObject, EPlay );
                    delete data;
                    CleanupStack::PopAndDestroy(); // file
                    }
                }
            CleanupStack::PopAndDestroy();


            LOG(_L("Opening plain files (CAF with filehandle) done"));
            }
            break;

        case 3: //With DRM
            {

            LOG(_L("Opening FL files"));
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KFlFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                    EPlay, EContentShareReadOnly);
                    delete data;
                    }
                }
            LOG(_L("Opening FL files done"));

            User::After(3000000);

            LOG(_L("Opening DRM FL files (CAF with filehandle)"));

            User::LeaveIfError(fs.Connect());
            CleanupClosePushL(fs);
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KFlFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    User::LeaveIfError(file.Open(fs, fileName, EFileRead | EFileShareAny));
                    CleanupClosePushL(file);
                    data = CData::NewL(file, KDefaultContentObject, EPlay );
                    delete data;
                    CleanupStack::PopAndDestroy();
                    }
                }
            CleanupStack::PopAndDestroy();

            LOG(_L("Opening DRM FL files (CAF with filehandle) done"));

            User::After(3000000);

            LOG(_L("Opening CD (Start/End) files"));
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KCdStartEndFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                    EPlay, EContentShareReadOnly);
                    delete data;
                    }
                }
            LOG(_L("Opening CD (Start/End) files done"));

            User::After(3000000);

            LOG(_L("Opening CD (Count) files"));
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KCdCountFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                    EPlay, EContentShareReadOnly);
                    delete data;
                    }
                }
            LOG(_L("Opening CD (Count) files done"));

            }
            break;
        case 4:
            {
            LOG(_L("Opening CD (Count) files with Consumption"));
            for (i = 0; i < KFileNumber; i++)
                {
                fileName.Copy(aDir);
                fileName.AppendNum(i);
                fileName.Append(KCdCountFileNameBase);
                fileName.Append(KFileSuffix);
                for (j = 0; j < KOpenCount; j++)
                    {
                    data = CData::NewL(TVirtualPathPtr(fileName, KDefaultContentObject),
                    EPlay, EContentShareReadOnly);
                    data->ExecuteIntent(EPlay);
                    delete data;
                    }
                }
            LOG(_L("Opening CD (Count) files with Consumption done"));
            }
            break;
        }
    }
void TestDatabasePerformanceL()
    {
    LOG(_L("TestDatabasePerformanceL"));
    }

void TestPerformanceL(TInt aCommand)
    {
    LOG(_L("TestPerformanceL"));

#ifndef RD_MULTIPLE_DRIVE

    switch(aCommand)
    {
    case EDRM_API_SubMenuId_1:        // Generate files
        {
        SetupDirectoryL(KFilesDir);
        GenerateFilesL(KFilesDir, KFileNumber);
        GenerateBigFilesL(KFilesDir);
        break;
        }
    case EDRM_API_SubMenuId_1_1:        // RFile opening
        {
        TestFileOpeningSpeedL(KFilesDir, 1);
        break;
        }
    case EDRM_API_SubMenuId_1_2:        // RFile Sequential
        {
        TestDecryptionSpeedL(KFilesDir, EFalse, 1);
        break;
        }
    case EDRM_API_SubMenuId_1_3:        // RFile Random
        {
        TestDecryptionSpeedL(KFilesDir, ETrue, 1);
        break;
        }
    case EDRM_API_SubMenuId_2_1:        // CAF File Opening
        {
        TestFileOpeningSpeedL(KFilesDir, 2);
        break;
        }
    case EDRM_API_SubMenuId_2_2:        // CAF Sequential
        {
        TestDecryptionSpeedL(KFilesDir, EFalse, 2);
        break;
        }
    case EDRM_API_SubMenuId_2_3:        // CAF Random
        {
        TestDecryptionSpeedL(KFilesDir, ETrue, 2);
        break;
        }
    case EDRM_API_SubMenuId_3_1_1:        // DRM file opening
        {
        TestFileOpeningSpeedL(KFilesDir, 3);
        break;
        }
    case EDRM_API_SubMenuId_3_1_2:        // DRM file opening with consume
        {
        TestFileOpeningSpeedL(KFilesDir, 4);
        break;
        }
    case EDRM_API_SubMenuId_3_1_3:    // Sequential DRM
        {
        TestDecryptionSpeedL(KFilesDir, EFalse, 3);
        break;
        }
    case EDRM_API_SubMenuId_3_1_4:    // Random DRM
        {
        TestDecryptionSpeedL(KFilesDir, ETrue, 3);
        break;
        }
    }

#else //RD_MULTIPLE_DRIVE

    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, driveNumber );

    User::LeaveIfError( fs.Connect() );
    fs.DriveToChar( driveNumber, driveLetter );
    fs.Close();

    TFileName filesDir;
    filesDir.Format( KFilesDir, (TUint)driveLetter );

    switch(aCommand)
    {
    case EDRM_API_SubMenuId_1:        // Generate files
        {
        SetupDirectoryL(filesDir);
        GenerateFilesL(filesDir, KFileNumber);
        GenerateBigFilesL(filesDir);
        break;
        }
    case EDRM_API_SubMenuId_1_1:        // RFile opening
        {
        TestFileOpeningSpeedL(filesDir, 1);
        break;
        }
    case EDRM_API_SubMenuId_1_2:        // RFile Sequential
        {
        TestDecryptionSpeedL(filesDir, EFalse, 1);
        break;
        }
    case EDRM_API_SubMenuId_1_3:        // RFile Random
        {
        TestDecryptionSpeedL(filesDir, ETrue, 1);
        break;
        }
    case EDRM_API_SubMenuId_2_1:        // CAF File Opening
        {
        TestFileOpeningSpeedL(filesDir, 2);
        break;
        }
    case EDRM_API_SubMenuId_2_2:        // CAF Sequential
        {
        TestDecryptionSpeedL(filesDir, EFalse, 2);
        break;
        }
    case EDRM_API_SubMenuId_2_3:        // CAF Random
        {
        TestDecryptionSpeedL(filesDir, ETrue, 2);
        break;
        }
    case EDRM_API_SubMenuId_3_1_1:        // DRM file opening
        {
        TestFileOpeningSpeedL(filesDir, 3);
        break;
        }
    case EDRM_API_SubMenuId_3_1_2:        // DRM file opening with consume
        {
        TestFileOpeningSpeedL(filesDir, 4);
        break;
        }
    case EDRM_API_SubMenuId_3_1_3:    // Sequential DRM
        {
        TestDecryptionSpeedL(filesDir, EFalse, 3);
        break;
        }
    case EDRM_API_SubMenuId_3_1_4:    // Random DRM
        {
        TestDecryptionSpeedL(filesDir, ETrue, 3);
        break;
        }
    }

#endif

    //TestDatabasePerformanceL();
    }
