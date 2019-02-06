/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextList>
#include <QTextTable>
#include <QBuffer>
#include <QDebug>

#include <private/qtextmarkdownwriter_p.h>

// #define DEBUG_WRITE_OUTPUT

class tst_QTextMarkdownWriter : public QObject
{
    Q_OBJECT
public slots:
    void init();
    void cleanup();

private slots:
    void testWriteParagraph_data();
    void testWriteParagraph();
    void testWriteList();
    void testWriteNestedBulletLists();
    void testWriteNestedNumericLists();
    void testWriteTable();
    void rewriteDocument();
    void fromHtml_data();
    void fromHtml();

private:
    QString documentToUnixMarkdown();

private:
    QTextDocument *document;
};

void tst_QTextMarkdownWriter::init()
{
    document = new QTextDocument();
}

void tst_QTextMarkdownWriter::cleanup()
{
    delete document;
}

void tst_QTextMarkdownWriter::testWriteParagraph_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    QTest::newRow("empty") << "" <<
        "";
    QTest::newRow("spaces") << "foobar   word" <<
        "foobar   word\n\n";
    QTest::newRow("starting spaces") << "  starting spaces" <<
        "  starting spaces\n\n";
    QTest::newRow("trailing spaces") << "trailing spaces  " <<
        "trailing spaces  \n\n";
    QTest::newRow("tab") << "word\ttab x" <<
        "word\ttab x\n\n";
    QTest::newRow("tab2") << "word\t\ttab\tx" <<
        "word\t\ttab\tx\n\n";
    QTest::newRow("misc") << "foobar   word\ttab x" <<
        "foobar   word\ttab x\n\n";
    QTest::newRow("misc2") << "\t     \tFoo" <<
        "\t     \tFoo\n\n";
}

void tst_QTextMarkdownWriter::testWriteParagraph()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    QTextCursor cursor(document);
    cursor.insertText(input);

    QCOMPARE(documentToUnixMarkdown(), output);
}

void tst_QTextMarkdownWriter::testWriteList()
{
    QTextCursor cursor(document);
    QTextList *list = cursor.createList(QTextListFormat::ListDisc);
    cursor.insertText("ListItem 1");
    list->add(cursor.block());
    cursor.insertBlock();
    cursor.insertText("ListItem 2");
    list->add(cursor.block());

    QCOMPARE(documentToUnixMarkdown(), QString::fromLatin1(
        "- ListItem 1\n- ListItem 2\n"));
}

void tst_QTextMarkdownWriter::testWriteNestedBulletLists()
{
    QTextCursor cursor(document);

    QTextList *list1 = cursor.createList(QTextListFormat::ListDisc);
    cursor.insertText("ListItem 1");
    list1->add(cursor.block());

    QTextListFormat fmt2;
    fmt2.setStyle(QTextListFormat::ListCircle);
    fmt2.setIndent(2);
    QTextList *list2 = cursor.insertList(fmt2);
    cursor.insertText("ListItem 2");

    QTextListFormat fmt3;
    fmt3.setStyle(QTextListFormat::ListSquare);
    fmt3.setIndent(3);
    cursor.insertList(fmt3);
    cursor.insertText("ListItem 3");

    cursor.insertBlock();
    cursor.insertText("ListItem 4");
    list1->add(cursor.block());

    cursor.insertBlock();
    cursor.insertText("ListItem 5");
    list2->add(cursor.block());

    QCOMPARE(documentToUnixMarkdown(), QString::fromLatin1(
                 "- ListItem 1\n  * ListItem 2\n    + ListItem 3\n- ListItem 4\n  * ListItem 5\n"));
}

void tst_QTextMarkdownWriter::testWriteNestedNumericLists()
{
    QTextCursor cursor(document);

    QTextList *list1 = cursor.createList(QTextListFormat::ListDecimal);
    cursor.insertText("ListItem 1");
    list1->add(cursor.block());

    QTextListFormat fmt2;
    fmt2.setStyle(QTextListFormat::ListLowerAlpha);
    fmt2.setNumberSuffix(QLatin1String(")"));
    fmt2.setIndent(2);
    QTextList *list2 = cursor.insertList(fmt2);
    cursor.insertText("ListItem 2");

    QTextListFormat fmt3;
    fmt3.setStyle(QTextListFormat::ListDecimal);
    fmt3.setIndent(3);
    cursor.insertList(fmt3);
    cursor.insertText("ListItem 3");

    cursor.insertBlock();
    cursor.insertText("ListItem 4");
    list1->add(cursor.block());

    cursor.insertBlock();
    cursor.insertText("ListItem 5");
    list2->add(cursor.block());

    // There's no QTextList API to set the starting number so we hard-coded all lists to start at 1 (QTBUG-65384)
    QCOMPARE(documentToUnixMarkdown(), QString::fromLatin1(
                 "1 ListItem 1\n    1) ListItem 2\n        1 ListItem 3\n2 ListItem 4\n    2) ListItem 5\n"));
}

void tst_QTextMarkdownWriter::testWriteTable()
{
    QTextCursor cursor(document);
    QTextTable * table = cursor.insertTable(4, 3);
    cursor = table->cellAt(0, 0).firstCursorPosition();
    // valid Markdown tables need headers, but QTextTable doesn't make that distinction
    // so QTextMarkdownWriter assumes the first row of any table is a header
    cursor.insertText("one");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("two");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("three");
    cursor.movePosition(QTextCursor::NextCell);

    cursor.insertText("alice");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("bob");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("carl");
    cursor.movePosition(QTextCursor::NextCell);

    cursor.insertText("dennis");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("eric");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("fiona");
    cursor.movePosition(QTextCursor::NextCell);

    cursor.insertText("gina");
    /*
        |one   |two |three|
        |------|----|-----|
        |alice |bob |carl |
        |dennis|eric|fiona|
        |gina  |    |     |
    */

    QString md = documentToUnixMarkdown();

#ifdef DEBUG_WRITE_OUTPUT
    {
        QFile out("/tmp/table.md");
        out.open(QFile::WriteOnly);
        out.write(md.toUtf8());
        out.close();
    }
#endif

    QString expected = QString::fromLatin1(
        "\n|one   |two |three|\n|------|----|-----|\n|alice |bob |carl |\n|dennis|eric|fiona|\n|gina  |    |     |\n\n");
    QCOMPARE(md, expected);

    // create table with merged cells
    document->clear();
    cursor = QTextCursor(document);
    table = cursor.insertTable(3, 3);
    table->mergeCells(0, 0, 1, 2);
    table->mergeCells(1, 1, 1, 2);
    cursor = table->cellAt(0, 0).firstCursorPosition();
    cursor.insertText("a");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("b");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("c");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("d");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("e");
    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText("f");
    /*
      +---+-+
      |a  |b|
      +---+-+
      |c|  d|
      +-+-+-+
      |e|f| |
      +-+-+-+

      generates

      |a ||b|
      |-|-|-|
      |c|d ||
      |e|f| |

    */

    md = documentToUnixMarkdown();

#ifdef DEBUG_WRITE_OUTPUT
    {
        QFile out("/tmp/table-merged-cells.md");
        out.open(QFile::WriteOnly);
        out.write(md.toUtf8());
        out.close();
    }
#endif

    QCOMPARE(md, QString::fromLatin1("\n|a ||b|\n|-|-|-|\n|c|d ||\n|e|f| |\n\n"));
}

void tst_QTextMarkdownWriter::rewriteDocument()
{
    QTextDocument doc;
    QFile f(QFINDTESTDATA("data/example.md"));
    QVERIFY(f.open(QFile::ReadOnly | QIODevice::Text));
    QString orig = QString::fromUtf8(f.readAll());
    f.close();
    doc.setMarkdown(orig);
    QString md = doc.toMarkdown();

#ifdef DEBUG_WRITE_OUTPUT
    QFile out("/tmp/rewrite.md");
    out.open(QFile::WriteOnly);
    out.write(md.toUtf8());
    out.close();
#endif

    QCOMPARE(md, orig);
}

void tst_QTextMarkdownWriter::fromHtml_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    QTest::newRow("long URL") <<
        "<span style=\"font-style:italic;\">https://www.example.com/dir/subdir/subsubdir/subsubsubdir/subsubsubsubdir/subsubsubsubsubdir/</span>" <<
        "*https://www.example.com/dir/subdir/subsubdir/subsubsubdir/subsubsubsubdir/subsubsubsubsubdir/*\n\n";
    QTest::newRow("non-emphasis inline asterisk") << "3 * 4" << "3 * 4\n\n";
    QTest::newRow("arithmetic") << "(2 * a * x + b)^2 = b^2 - 4 * a * c" << "(2 * a * x + b)^2 = b^2 - 4 * a * c\n\n";
    QTest::newRow("escaped asterisk after newline") <<
        "The first sentence of this paragraph holds 80 characters, then there's a star. * This is wrapped, but is <em>not</em> a bullet point." <<
        "The first sentence of this paragraph holds 80 characters, then there's a star.\n\\* This is wrapped, but is *not* a bullet point.\n\n";
    QTest::newRow("escaped plus after newline") <<
        "The first sentence of this paragraph holds 80 characters, then there's a plus. + This is wrapped, but is <em>not</em> a bullet point." <<
        "The first sentence of this paragraph holds 80 characters, then there's a plus.\n\\+ This is wrapped, but is *not* a bullet point.\n\n";
    QTest::newRow("escaped hyphen after newline") <<
        "The first sentence of this paragraph holds 80 characters, then there's a minus. - This is wrapped, but is <em>not</em> a bullet point." <<
        "The first sentence of this paragraph holds 80 characters, then there's a minus.\n\\- This is wrapped, but is *not* a bullet point.\n\n";
    // TODO
//    QTest::newRow("escaped number and paren after double newline") <<
//        "<p>(The first sentence of this paragraph is a line, the next paragraph has a number</p>13) but that's not part of an ordered list" <<
//        "(The first sentence of this paragraph is a line, the next paragraph has a number\n\n13\\) but that's not part of an ordered list\n\n";
//    QTest::newRow("preformats with embedded backticks") <<
//        "<pre>none `one` ``two``</pre><pre>```three``` ````four````</pre>plain" <<
//        "``` none `one` ``two`` ```\n\n````` ```three``` ````four```` `````\n\nplain\n\n";
}

void tst_QTextMarkdownWriter::fromHtml()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    document->setHtml(input);
    QCOMPARE(documentToUnixMarkdown(), output);
}

QString tst_QTextMarkdownWriter::documentToUnixMarkdown()
{
    QString ret;
    QTextStream ts(&ret, QIODevice::WriteOnly);
    QTextMarkdownWriter writer(ts, QTextDocument::MarkdownDialectGitHub);
    writer.writeAll(*document);
    return ret;
}

QTEST_MAIN(tst_QTextMarkdownWriter)
#include "tst_qtextmarkdownwriter.moc"