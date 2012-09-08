#include "stdafx.h"

#if !defined(QT_NO_DEBUG)

#include "UnitTests.h"

void UnitTests_ExecuteAll()
{
    TestCommon testCommon;
    QTest::qExec(&testCommon);
}

void TestCommon::testParseOctalValue()
{
    WORD value;

    QVERIFY(ParseOctalValue(_T("0"), &value));
    QVERIFY(value == 0);
    QVERIFY(ParseOctalValue(_T("000000"), &value));
    QVERIFY(value == 0);
    QVERIFY(ParseOctalValue(_T("177777"), &value));
    QVERIFY(value == 0177777);
    QVERIFY(ParseOctalValue(_T("123456"), &value));
    QVERIFY(value == 0123456);
    QVERIFY(ParseOctalValue(_T("5"), &value));
    QVERIFY(value == 05);
    QVERIFY(ParseOctalValue(_T("17"), &value));
    QVERIFY(value == 017);
    QVERIFY(ParseOctalValue(_T("176"), &value));
    QVERIFY(value == 0176);

    QVERIFY(!ParseOctalValue(_T("8"), &value));
    QVERIFY(!ParseOctalValue(_T("-1"), &value));
    QVERIFY(!ParseOctalValue(_T("x"), &value));
}

void TestCommon::testPrintOctalValue()
{
    TCHAR buffer[10];

    PrintOctalValue(buffer, 0);
    QCOMPARE(buffer, _T("000000"));
    PrintOctalValue(buffer, 0177777);
    QCOMPARE(buffer, _T("177777"));
    PrintOctalValue(buffer, 0176);
    QCOMPARE(buffer, _T("000176"));
}

void TestCommon::testPrintBinaryValue()
{
    TCHAR buffer[17];

    PrintBinaryValue(buffer, 0);
    QCOMPARE((const TCHAR*)buffer, _T("0000000000000000"));
    PrintBinaryValue(buffer, 0177777);
    QCOMPARE((const TCHAR*)buffer, _T("1111111111111111"));
    PrintBinaryValue(buffer, 0123456);
    QCOMPARE((const TCHAR*)buffer, _T("1010011100101110"));
}

#endif // if !defined(QT_NO_DEBUG)
