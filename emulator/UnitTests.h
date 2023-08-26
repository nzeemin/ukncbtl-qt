#ifndef UNITTESTS_H
#define UNITTESTS_H

#if !defined(QT_NO_DEBUG)

#include <QtTest/QtTest>


class TestCommon : public QObject
{
    Q_OBJECT
private slots:
    void testParseOctalValue();
    void testPrintOctalValue();
    void testPrintBinaryValue();
};


#endif // if !defined(QT_NO_DEBUG)

#endif // UNITTESTS_H
