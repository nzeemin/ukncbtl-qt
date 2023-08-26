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
    quint16 value;

    QVERIFY(ParseOctalValue("0", &value));
    QVERIFY(value == 0);
    QVERIFY(ParseOctalValue("000000", &value));
    QVERIFY(value == 0);
    QVERIFY(ParseOctalValue("177777", &value));
    QVERIFY(value == 0177777);
    QVERIFY(ParseOctalValue("123456", &value));
    QVERIFY(value == 0123456);
    QVERIFY(ParseOctalValue("5", &value));
    QVERIFY(value == 05);
    QVERIFY(ParseOctalValue("17", &value));
    QVERIFY(value == 017);
    QVERIFY(ParseOctalValue("176", &value));
    QVERIFY(value == 0176);

    QVERIFY(!ParseOctalValue("8", &value));
    QVERIFY(!ParseOctalValue("-1", &value));
    QVERIFY(!ParseOctalValue("x", &value));
}

void TestCommon::testPrintOctalValue()
{
    char buffer[10];

    PrintOctalValue(buffer, 0);
    QCOMPARE(buffer, "000000");
    PrintOctalValue(buffer, 0177777);
    QCOMPARE(buffer, "177777");
    PrintOctalValue(buffer, 0176);
    QCOMPARE(buffer, "000176");
}

void TestCommon::testPrintBinaryValue()
{
    char buffer[17];

    PrintBinaryValue(buffer, 0);
    QCOMPARE((const char*)buffer, "0000000000000000");
    PrintBinaryValue(buffer, 0177777);
    QCOMPARE((const char*)buffer, "1111111111111111");
    PrintBinaryValue(buffer, 0123456);
    QCOMPARE((const char*)buffer, "1010011100101110");
}

#endif // if !defined(QT_NO_DEBUG)
