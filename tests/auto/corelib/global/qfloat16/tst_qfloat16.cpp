/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Copyright (C) 2016 by Southwest Research Institute (R)
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QFloat16>

#include <math.h>

class tst_qfloat16: public QObject
{
    Q_OBJECT

private slots:
    void fuzzyCompare_data();
    void fuzzyCompare();
    void ltgt_data();
    void ltgt();
    void qNan();
    void float_cast();
    void float_cast_data();
    void promotionTests();
    void arithOps_data();
    void arithOps();
    void floatToFloat16();
    void floatFromFloat16();
    void limits();
};

void tst_qfloat16::fuzzyCompare_data()
{
    QTest::addColumn<qfloat16>("val1");
    QTest::addColumn<qfloat16>("val2");
    QTest::addColumn<bool>("fuzEqual");
    QTest::addColumn<bool>("isEqual");

    QTest::newRow("zero")  << qfloat16(0.0f)   << qfloat16(0.0f)   << true << true;
    QTest::newRow("ten")   << qfloat16(1e1f)   << qfloat16(1e1f)   << true << true;
    QTest::newRow("large") << qfloat16(1e4f)   << qfloat16(1e4f)   << true << true;
    QTest::newRow("small") << qfloat16(1e-5f)  << qfloat16(1e-5f)  << true << true;
    QTest::newRow("eps")   << qfloat16(10.01f) << qfloat16(10.02f) << true << false;
    QTest::newRow("eps2")  << qfloat16(1024.f) << qfloat16(1033.f) << true << false;

    QTest::newRow("mis1") << qfloat16(0.0f)   << qfloat16(1.0f)   << false << false;
    QTest::newRow("mis2") << qfloat16(0.0f)   << qfloat16(1e7f)   << false << false;
    QTest::newRow("mis3") << qfloat16(0.0f)   << qfloat16(1e-4f)  << false << false;
    QTest::newRow("mis4") << qfloat16(1e8f)   << qfloat16(1e-8f)  << false << false;
    QTest::newRow("mis5") << qfloat16(1e-4f)  << qfloat16(1e-5)   << false << false;
    QTest::newRow("mis6") << qfloat16(1024.f) << qfloat16(1034.f) << false << false;
}

void tst_qfloat16::fuzzyCompare()
{
    QFETCH(qfloat16, val1);
    QFETCH(qfloat16, val2);
    QFETCH(bool, fuzEqual);
    QFETCH(bool, isEqual);

    if (!isEqual && (val1==val2))
        qWarning() << "Identical arguments provided unintentionally!";

    if (fuzEqual) {
        QVERIFY(::qFuzzyCompare(val1, val2));
        QVERIFY(::qFuzzyCompare(val2, val1));
        QVERIFY(::qFuzzyCompare(-val1, -val2));
        QVERIFY(::qFuzzyCompare(-val2, -val1));
    } else {
        QVERIFY(!::qFuzzyCompare(val1, val2));
        QVERIFY(!::qFuzzyCompare(val2, val1));
        QVERIFY(!::qFuzzyCompare(-val1, -val2));
        QVERIFY(!::qFuzzyCompare(-val2, -val1));
    }
}

void tst_qfloat16::ltgt_data()
{
    QTest::addColumn<float>("val1");
    QTest::addColumn<float>("val2");

    QTest::newRow("zero")  << 0.0f << 0.0f;
    QTest::newRow("ten")   << 10.0f << 10.0f;
    QTest::newRow("large") << 100000.0f << 100000.0f;
    QTest::newRow("small") << 0.0000001f << 0.0000001f;
    QTest::newRow("eps")   << 10.000000000000001f << 10.00000000000002f;
    QTest::newRow("eps2")  << 10.000000000000001f << 10.000000000000009f;

    QTest::newRow("mis1") << 0.0f << 1.0f;
    QTest::newRow("mis2") << 0.0f << 10000000.0f;
    QTest::newRow("mis3") << 0.0f << 0.0001f;
    QTest::newRow("mis4") << 100000000.0f << 0.000000001f;
    QTest::newRow("mis5") << 0.0001f << 0.00001f;

    QTest::newRow("45,23") << 45.f << 23.f;
    QTest::newRow("1000,76") << 1000.f << 76.f;
}

void tst_qfloat16::ltgt()
{
    QFETCH(float, val1);
    QFETCH(float, val2);

    QCOMPARE(qfloat16(val1)  == qfloat16(val2),   val1 ==  val2);
    QCOMPARE(qfloat16(val1)  <  qfloat16(val2),   val1 <   val2);
    QCOMPARE(qfloat16(val1)  <= qfloat16(val2),   val1 <=  val2);
    QCOMPARE(qfloat16(val1)  >  qfloat16(val2),   val1 >   val2);
    QCOMPARE(qfloat16(val1)  >= qfloat16(val2),   val1 >=  val2);

    QCOMPARE(qfloat16(val1)  == qfloat16(-val2),  val1 == -val2);
    QCOMPARE(qfloat16(val1)  <  qfloat16(-val2),  val1 <  -val2);
    QCOMPARE(qfloat16(val1)  <= qfloat16(-val2),  val1 <= -val2);
    QCOMPARE(qfloat16(val1)  >  qfloat16(-val2),  val1 >  -val2);
    QCOMPARE(qfloat16(val1)  >= qfloat16(-val2),  val1 >= -val2);

    QCOMPARE(qfloat16(-val1) == qfloat16(val2),  -val1 ==  val2);
    QCOMPARE(qfloat16(-val1) <  qfloat16(val2),  -val1 <   val2);
    QCOMPARE(qfloat16(-val1) <= qfloat16(val2),  -val1 <=  val2);
    QCOMPARE(qfloat16(-val1) >  qfloat16(val2),  -val1 >   val2);
    QCOMPARE(qfloat16(-val1) >= qfloat16(val2),  -val1 >=  val2);

    QCOMPARE(qfloat16(-val1) == qfloat16(-val2), -val1 == -val2);
    QCOMPARE(qfloat16(-val1) <  qfloat16(-val2), -val1 <  -val2);
    QCOMPARE(qfloat16(-val1) <= qfloat16(-val2), -val1 <= -val2);
    QCOMPARE(qfloat16(-val1) >  qfloat16(-val2), -val1 >  -val2);
    QCOMPARE(qfloat16(-val1) >= qfloat16(-val2), -val1 >= -val2);
}

#if defined __FAST_MATH__ && (__GNUC__ * 100 + __GNUC_MINOR__ >= 404)
   // turn -ffast-math off
#  pragma GCC optimize "no-fast-math"
#endif

void tst_qfloat16::qNan()
{
#if defined __FAST_MATH__ && (__GNUC__ * 100 + __GNUC_MINOR__ < 404)
    QSKIP("Non-conformant fast math mode is enabled, cannot run test");
#endif
    qfloat16 nan = qQNaN();
    QVERIFY(!(0. > nan));
    QVERIFY(!(0. < nan));
    QVERIFY(!qIsInf(nan));
    QVERIFY(qIsNaN(nan));
    QVERIFY(qIsNaN(nan + 1.f));
    QVERIFY(qIsNaN(-nan));
    qfloat16 inf = qInf();
    QVERIFY(inf > qfloat16(0));
    QVERIFY(-inf < qfloat16(0));
    QVERIFY(!qIsNaN(inf));
    QVERIFY(qIsInf(inf));
    QVERIFY(qIsInf(-inf));
    QVERIFY(qIsInf(2.f*inf));
    QVERIFY(qIsInf(inf*2.f));
    QCOMPARE(qfloat16(1.f/inf), qfloat16(0.f));
#ifdef Q_CC_INTEL
    QEXPECT_FAIL("", "ICC optimizes zero * anything to zero", Continue);
#endif
    QVERIFY(qIsNaN(nan*0.f));
#ifdef Q_CC_INTEL
    QEXPECT_FAIL("", "ICC optimizes zero * anything to zero", Continue);
#endif
    QVERIFY(qIsNaN(inf*0.f));
    QVERIFY(qFuzzyCompare(qfloat16(1.f/inf), qfloat16(0.0)));
}

void tst_qfloat16::float_cast_data()
{
    QTest::addColumn<float>("val");

    QTest::newRow("zero")  << 0.f;
    QTest::newRow("one")   << 1e0f;
    QTest::newRow("ten")   << 1e1f;
    QTest::newRow("hund")  << 1e2f;
    QTest::newRow("thou")  << 1e3f;
    QTest::newRow("tthou") << 1e4f;
    //QTest::newRow("hthou") << 1e5f;
    //QTest::newRow("mil")   << 1e6f;
    //QTest::newRow("tmil")  << 1e7f;
    //QTest::newRow("hmil")  << 1e8f;
}

void tst_qfloat16::float_cast()
{
    QFETCH(float, val);

    QVERIFY(qFuzzyCompare((float)(qfloat16(val)),val));
    QVERIFY(qFuzzyCompare((float)(qfloat16(-val)),-val));
    QVERIFY(qFuzzyCompare((double)(qfloat16(val)),(double)(val)));
    QVERIFY(qFuzzyCompare((double)(qfloat16(-val)),(double)(-val)));
    //QVERIFY(qFuzzyCompare((long double)(qfloat16(val)),(long double)(val)));
    //QVERIFY(qFuzzyCompare((long double)(qfloat16(-val)),(long double)(-val)));
}

void tst_qfloat16::promotionTests()
{
    QCOMPARE(sizeof(qfloat16),sizeof(qfloat16(1.f)+qfloat16(1.f)));
    QCOMPARE(sizeof(qfloat16),sizeof(qfloat16(1.f)-qfloat16(1.f)));
    QCOMPARE(sizeof(qfloat16),sizeof(qfloat16(1.f)*qfloat16(1.f)));
    QCOMPARE(sizeof(qfloat16),sizeof(qfloat16(1.f)/qfloat16(1.f)));

    QCOMPARE(sizeof(float),sizeof(1.f+qfloat16(1.f)));
    QCOMPARE(sizeof(float),sizeof(1.f-qfloat16(1.f)));
    QCOMPARE(sizeof(float),sizeof(1.f*qfloat16(1.f)));
    QCOMPARE(sizeof(float),sizeof(1.f/qfloat16(1.f)));

    QCOMPARE(sizeof(float),sizeof(qfloat16(1.f)+1.f));
    QCOMPARE(sizeof(float),sizeof(qfloat16(1.f)-1.f));
    QCOMPARE(sizeof(float),sizeof(qfloat16(1.f)*1.f));
    QCOMPARE(sizeof(float),sizeof(qfloat16(1.f)/1.f));

    QCOMPARE(sizeof(double),sizeof(1.+qfloat16(1.f)));
    QCOMPARE(sizeof(double),sizeof(1.-qfloat16(1.f)));
    QCOMPARE(sizeof(double),sizeof(1.*qfloat16(1.f)));
    QCOMPARE(sizeof(double),sizeof(1./qfloat16(1.f)));

    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)+1.));
    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)-1.));
    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)*1.));
    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)/1.));

    QCOMPARE(sizeof(long double),sizeof((long double)(1.)+qfloat16(1.f)));
    QCOMPARE(sizeof(long double),sizeof((long double)(1.)-qfloat16(1.f)));
    QCOMPARE(sizeof(long double),sizeof((long double)(1.)*qfloat16(1.f)));
    QCOMPARE(sizeof(long double),sizeof((long double)(1.)/qfloat16(1.f)));

    QCOMPARE(sizeof(long double),sizeof(qfloat16(1.f)+(long double)(1.)));
    QCOMPARE(sizeof(long double),sizeof(qfloat16(1.f)-(long double)(1.)));
    QCOMPARE(sizeof(long double),sizeof(qfloat16(1.f)*(long double)(1.)));
    QCOMPARE(sizeof(long double),sizeof(qfloat16(1.f)/(long double)(1.)));

    QCOMPARE(sizeof(double),sizeof(1+qfloat16(1.f)));
    QCOMPARE(sizeof(double),sizeof(1-qfloat16(1.f)));
    QCOMPARE(sizeof(double),sizeof(1*qfloat16(1.f)));
    QCOMPARE(sizeof(double),sizeof(1/qfloat16(1.f)));

    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)+1));
    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)-1));
    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)*1));
    QCOMPARE(sizeof(double),sizeof(qfloat16(1.f)/1));

    QCOMPARE(QString::number(1.f),QString::number(qfloat16(1.f)));
}

void tst_qfloat16::arithOps_data()
{
    QTest::addColumn<float>("val1");
    QTest::addColumn<float>("val2");

    QTest::newRow("zero")  << 0.0f << 2.0f;
    QTest::newRow("one")   << 1.0f << 4.0f;
    QTest::newRow("ten")   << 10.0f << 20.0f;
}

void tst_qfloat16::arithOps()
{
    QFETCH(float, val1);
    QFETCH(float, val2);

    QVERIFY(qFuzzyCompare(float(qfloat16(val1) + qfloat16(val2)), val1 + val2));
    QVERIFY(qFuzzyCompare(float(qfloat16(val1) - qfloat16(val2)), val1 - val2));
    QVERIFY(qFuzzyCompare(float(qfloat16(val1) * qfloat16(val2)), val1 * val2));
    QVERIFY(qFuzzyCompare(float(qfloat16(val1) / qfloat16(val2)), val1 / val2));

    QVERIFY(qFuzzyCompare(qfloat16(val1) + val2, val1 + val2));
    QVERIFY(qFuzzyCompare(qfloat16(val1) - val2, val1 - val2));
    QVERIFY(qFuzzyCompare(qfloat16(val1) * val2, val1 * val2));
    QVERIFY(qFuzzyCompare(qfloat16(val1) / val2, val1 / val2));

    QVERIFY(qFuzzyCompare(val1 + qfloat16(val2), val1 + val2));
    QVERIFY(qFuzzyCompare(val1 - qfloat16(val2), val1 - val2));
    QVERIFY(qFuzzyCompare(val1 * qfloat16(val2), val1 * val2));
    QVERIFY(qFuzzyCompare(val1 / qfloat16(val2), val1 / val2));

    float r1 = 0.f;
    r1 += qfloat16(val2);
    QVERIFY(qFuzzyCompare(r1,val2));

    float r2 = 0.f;
    r2 -= qfloat16(val2);
    QVERIFY(qFuzzyCompare(r2,-val2));

    float r3 = 1.f;
    r3 *= qfloat16(val2);
    QVERIFY(qFuzzyCompare(r3,val2));

    float r4 = 1.f;
    r4 /= qfloat16(val2);
    QVERIFY(qFuzzyCompare(r4,1.f/val2));
}

void tst_qfloat16::floatToFloat16()
{
    float in[63];
    qfloat16 out[63];
    qfloat16 expected[63];

    for (int i = 0; i < 63; ++i)
        in[i] = i * (1/13.f);

    for (int i = 0; i < 63; ++i)
        expected[i] = qfloat16(in[i]);

    qFloatToFloat16(out, in, 63);

    for (int i = 0; i < 63; ++i)
        QVERIFY(qFuzzyCompare(out[i], expected[i]));
}

void tst_qfloat16::floatFromFloat16()
{
    qfloat16 in[35];
    float out[35];
    float expected[35];

    for (int i = 0; i < 35; ++i)
        in[i] = qfloat16(i * (17.f / 3));

    for (int i = 0; i < 35; ++i)
        expected[i] = float(in[i]);

    qFloatFromFloat16(out, in, 35);

    for (int i = 0; i < 35; ++i)
        QCOMPARE(out[i], expected[i]);
}

static qfloat16 powf16(qfloat16 base, int raise)
{
    const qfloat16 one(1.f);
    if (raise < 0) {
        raise = -raise;
        base = one / base;
    }
    qfloat16 answer = (raise & 1) ? base : one;
    while (raise > 0) {
        raise >>= 1;
        base *= base;
        if (raise & 1)
            answer *= base;
    }
    return answer;
}

void tst_qfloat16::limits()
{
    // *NOT* using QCOMPARE() on finite qfloat16 values, since that uses fuzzy
    // comparison, and we need exact here.
    using Bounds = std::numeric_limits<qfloat16>;
    QVERIFY(Bounds::is_specialized);
    QVERIFY(Bounds::is_signed);
    QVERIFY(!Bounds::is_integer);
    QVERIFY(!Bounds::is_exact);
    QVERIFY(Bounds::is_iec559);
    QVERIFY(Bounds::is_bounded);
    QVERIFY(!Bounds::is_modulo);
    QVERIFY(!Bounds::traps);
    QVERIFY(Bounds::has_infinity);
    QVERIFY(Bounds::has_quiet_NaN);
    QVERIFY(Bounds::has_signaling_NaN);
    QCOMPARE(Bounds::has_denorm, std::denorm_present);
    QCOMPARE(Bounds::round_style, std::round_to_nearest);
    QCOMPARE(Bounds::radix, 2);
    // Untested: has_denorm_loss

    // A few common values:
    const qfloat16 zero(0), one(1), ten(10);
    QVERIFY(qIsFinite(zero));
    QVERIFY(!qIsInf(zero));
    QVERIFY(!qIsNaN(zero));
    QCOMPARE(qFpClassify(zero), FP_ZERO);
    QVERIFY(qIsFinite(one));
    QVERIFY(!qIsInf(one));
    QCOMPARE(qFpClassify(one), FP_NORMAL);
    QVERIFY(!qIsNaN(one));
    QVERIFY(qIsFinite(ten));
    QVERIFY(!qIsInf(ten));
    QVERIFY(!qIsNaN(ten));
    QCOMPARE(qFpClassify(ten), FP_NORMAL);

    // digits in the mantissa, including the implicit 1 before the binary dot at its left:
    QVERIFY(qfloat16(1 << (Bounds::digits - 1)) + one > qfloat16(1 << (Bounds::digits - 1)));
    QVERIFY(qfloat16(1 << Bounds::digits) + one == qfloat16(1 << Bounds::digits));

    // There is a wilful of-by-one in how m(ax|in)_exponent are defined; they're
    // the lowest and highest n for which radix^{n-1} are normal and finite.
    const qfloat16 two(Bounds::radix);
    qfloat16 bit = powf16(two, Bounds::max_exponent - 1);
    QVERIFY(qIsFinite(bit));
    QVERIFY(qIsInf(bit * two));
    bit = powf16(two, Bounds::min_exponent - 1);
    QVERIFY(bit.isNormal());
    QCOMPARE(qFpClassify(bit), FP_NORMAL);
    QVERIFY(!(bit / two).isNormal());
    QCOMPARE(qFpClassify(bit / two), FP_SUBNORMAL);
    QVERIFY(bit / two > zero);

    // Base ten (with no matching off-by-one idiocy):
    // the lowest negative number n such that 10^n is a valid normalized value
    qfloat16 low10(powf16(ten, Bounds::min_exponent10));
    QVERIFY(low10 > zero);
    QVERIFY(low10.isNormal());
    low10 /= ten;
    QVERIFY(low10 == zero || !low10.isNormal());
    // the largest positive number n such that 10^n is a representable finite value
    qfloat16 high10(powf16(ten, Bounds::max_exponent10));
    QVERIFY(high10 > zero);
    QVERIFY(qIsFinite(high10));
    QVERIFY(!qIsFinite(high10 * ten));
    QCOMPARE(qFpClassify(high10), FP_NORMAL);

    // How many digits are significant ?  (Casts avoid linker errors ...)
    QCOMPARE(int(Bounds::digits10), 3); // 9.79e-4 has enough sigificant digits:
    qfloat16 below(9.785e-4f), above(9.794e-4f);
#if 0 // Sadly, the QEMU x-compile for arm64 "optimises" comparisons:
    const bool overOptimised = false;
#else
    const bool overOptimised = (below != above);
    if (overOptimised)
        QEXPECT_FAIL("", "Over-optimised on QEMU", Continue);
#endif // (but it did, so should, pass everywhere else, confirming digits10 is indeed 3).
    QVERIFY(below == above);
    QCOMPARE(int(Bounds::max_digits10), 5); // we need 5 to distinguish these two:
    QVERIFY(qfloat16(1000.5f) != qfloat16(1001.4f));

    // Actual limiting values of the type:
    const qfloat16 rose(one + Bounds::epsilon());
    QVERIFY(rose > one);
    if (overOptimised)
        QEXPECT_FAIL("", "Over-optimised on QEMU", Continue);
    QVERIFY(one + Bounds::epsilon() / rose == one);
    QVERIFY(qIsInf(Bounds::infinity()));
    QVERIFY(!qIsNaN(Bounds::infinity()));
    QVERIFY(!qIsFinite(Bounds::infinity()));
    // QCOMPARE(Bounds::infinity(), Bounds::infinity());
    QCOMPARE(qFpClassify(Bounds::infinity()), FP_INFINITE);

    QVERIFY(Bounds::infinity() > -Bounds::infinity());
    QVERIFY(Bounds::infinity() > zero);
    QVERIFY(qIsInf(-Bounds::infinity()));
    QVERIFY(!qIsNaN(-Bounds::infinity()));
    QVERIFY(!qIsFinite(-Bounds::infinity()));
    // QCOMPARE(-Bounds::infinity(), -Bounds::infinity());
    QCOMPARE(qFpClassify(-Bounds::infinity()), FP_INFINITE);

    QVERIFY(-Bounds::infinity() < zero);
    QVERIFY(qIsNaN(Bounds::quiet_NaN()));
    QVERIFY(!qIsInf(Bounds::quiet_NaN()));
    QVERIFY(!qIsFinite(Bounds::quiet_NaN()));
    QVERIFY(!(Bounds::quiet_NaN() == Bounds::quiet_NaN()));
    // QCOMPARE(Bounds::quiet_NaN(), Bounds::quiet_NaN());
    QCOMPARE(qFpClassify(Bounds::quiet_NaN()), FP_NAN);

    QVERIFY(Bounds::max() > zero);
    QVERIFY(qIsFinite(Bounds::max()));
    QVERIFY(!qIsInf(Bounds::max()));
    QVERIFY(!qIsNaN(Bounds::max()));
    QVERIFY(qIsInf(Bounds::max() * rose));
    QCOMPARE(qFpClassify(Bounds::max()), FP_NORMAL);

    QVERIFY(Bounds::lowest() < zero);
    QVERIFY(qIsFinite(Bounds::lowest()));
    QVERIFY(!qIsInf(Bounds::lowest()));
    QVERIFY(!qIsNaN(Bounds::lowest()));
    QVERIFY(qIsInf(Bounds::lowest() * rose));
    QCOMPARE(qFpClassify(Bounds::lowest()), FP_NORMAL);

    QVERIFY(Bounds::min() > zero);
    QVERIFY(Bounds::min().isNormal());
    QVERIFY(!(Bounds::min() / rose).isNormal());
    QVERIFY(qIsFinite(Bounds::min()));
    QVERIFY(!qIsInf(Bounds::min()));
    QVERIFY(!qIsNaN(Bounds::min()));
    QCOMPARE(qFpClassify(Bounds::min()), FP_NORMAL);

    QVERIFY(Bounds::denorm_min() > zero);
    QVERIFY(!Bounds::denorm_min().isNormal());
    QVERIFY(qIsFinite(Bounds::denorm_min()));
    QVERIFY(!qIsInf(Bounds::denorm_min()));
    QVERIFY(!qIsNaN(Bounds::denorm_min()));
    if (overOptimised)
        QEXPECT_FAIL("", "Over-optimised on QEMU", Continue);
    QCOMPARE(Bounds::denorm_min() / rose, zero);
    QCOMPARE(qFpClassify(Bounds::denorm_min()), FP_SUBNORMAL);
}

QTEST_APPLESS_MAIN(tst_qfloat16)
#include "tst_qfloat16.moc"
