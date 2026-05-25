#include <QtTest>
#include "../bytebuffer.h"
#include "../editormodel.h"
#include "../findpanel.h"
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

class FindPanelTest : public QObject {
    Q_OBJECT

private slots:
    void searchHex() {
        ByteBuffer buf;
        buf.load(QByteArray("\x12\x34\xAB\xCD\xEF\x12\x34", 7));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        int matchLength = 0;
        QList<int> matches = panel.search("12 34", EditorModel::Panel::Hex, matchLength);
        QCOMPARE(matches, QList<int>({0, 5}));
        QCOMPARE(matchLength, 2);

        matches = panel.search("ABCD", EditorModel::Panel::Hex, matchLength); // Continuous
        QCOMPARE(matches, QList<int>({2}));
        QCOMPARE(matchLength, 2);

        matches = panel.search("FF", EditorModel::Panel::Hex, matchLength);
        QCOMPARE(matches, QList<int>());
    }

    void searchChar() {
        ByteBuffer buf;
        buf.load(QByteArray("Hello World Hello", 17));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        int matchLength = 0;
        QList<int> matches = panel.search("Hello", EditorModel::Panel::Char, matchLength);
        QCOMPARE(matches, QList<int>({0, 12}));
        QCOMPARE(matchLength, 5);

        matches = panel.search("World", EditorModel::Panel::Char, matchLength);
        QCOMPARE(matches, QList<int>({6}));
        QCOMPARE(matchLength, 5);

        matches = panel.search("xyz", EditorModel::Panel::Char, matchLength);
        QCOMPARE(matches, QList<int>());
    }

    void searchBin() {
        ByteBuffer buf;
        // 0xAA = 10101010, 0x55 = 01010101
        buf.load(QByteArray("\xAA\x55\xAA", 3));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        int matchLength = 0;
        QList<int> matches = panel.search("10101010", EditorModel::Panel::Bin, matchLength);
        QCOMPARE(matches, QList<int>({0, 2}));
        QCOMPARE(matchLength, 1);

        matches = panel.search("01010101", EditorModel::Panel::Bin, matchLength);
        QCOMPARE(matches, QList<int>({1}));
        QCOMPARE(matchLength, 1);
    }

    void searchInvalidInputsReturnsEmpty() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x00", 2));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        int matchLength = 0;
        // Invalid hex
        QCOMPARE(panel.search("XX", EditorModel::Panel::Hex, matchLength), QList<int>());
        QCOMPARE(panel.search("123", EditorModel::Panel::Hex, matchLength), QList<int>());

        // Invalid bin
        QCOMPARE(panel.search("102", EditorModel::Panel::Bin, matchLength), QList<int>());
        QCOMPARE(panel.search("999", EditorModel::Panel::Bin, matchLength), QList<int>());
    }

    void nextAndPreviousCycling() {
        ByteBuffer buf;
        buf.load(QByteArray("A B A", 5));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        // Trigger search by simulating UI event to populate m_matches and m_current
        panel.m_modeCombo->setCurrentIndex(panel.m_modeCombo->findData(static_cast<int>(EditorModel::Panel::Char)));
        panel.m_input->setText("A");

        QCOMPARE(panel.m_matches, QList<int>({0, 4}));
        QCOMPARE(panel.m_current, 0);

        // findNext
        panel.findNext();
        QCOMPARE(panel.m_current, 1);
        panel.findNext();
        QCOMPARE(panel.m_current, 0); // Wraps around

        // findPrevious
        panel.findPrevious();
        QCOMPARE(panel.m_current, 1); // Wraps around backward
        panel.findPrevious();
        QCOMPARE(panel.m_current, 0);
    }
};

QTEST_MAIN(FindPanelTest)
#include "tst_findpanel.moc"
