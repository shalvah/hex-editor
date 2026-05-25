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

    void statusLabelUpdates() {
        ByteBuffer buf;
        buf.load(QByteArray("X Y X", 5));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        panel.m_modeCombo->setCurrentIndex(panel.m_modeCombo->findData(static_cast<int>(EditorModel::Panel::Char)));
        
        // Empty text
        panel.m_input->setText("");
        QCOMPARE(panel.m_status->text(), QString(""));
        
        // Valid text, matches found
        panel.m_input->setText("X");
        QCOMPARE(panel.m_status->text(), QString("1 / 2"));
        panel.findNext();
        QCOMPARE(panel.m_status->text(), QString("2 / 2"));

        // Valid text, no matches found
        panel.m_input->setText("Z");
        QCOMPARE(panel.m_status->text(), QString("No matches"));
    }

    void clearResults() {
        ByteBuffer buf;
        buf.load(QByteArray("Hello", 5));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        panel.m_modeCombo->setCurrentIndex(panel.m_modeCombo->findData(static_cast<int>(EditorModel::Panel::Char)));
        panel.m_input->setText("Hello");

        QCOMPARE(panel.m_matches.size(), 1);
        QVERIFY(!panel.m_input->text().isEmpty());

        // Invoke the clearResults private slot via Qt's meta-object system
        QMetaObject::invokeMethod(&panel, "clearResults");

        QCOMPARE(panel.m_matches.size(), 0);
        QCOMPARE(panel.m_current, -1);
        QVERIFY(panel.m_input->text().isEmpty());
    }

    void requestScrollSignalFires() {
        ByteBuffer buf;
        buf.load(QByteArray("test_data", 9));
        EditorModel model(buf);
        FindPanel panel(buf, model);

        QSignalSpy spy(&panel, &FindPanel::requestScrollToRow);

        panel.m_modeCombo->setCurrentIndex(panel.m_modeCombo->findData(static_cast<int>(EditorModel::Panel::Char)));
        panel.m_input->setText("data");

        // The text change immediately triggers a search and emits requestScrollToRow for the first match
        QCOMPARE(spy.count(), 1);
        QList<QVariant> arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).toInt(), 0); // "data" starts at index 5, which is row 0

        // findNext should emit it again
        panel.findNext();
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(FindPanelTest)
#include "tst_findpanel.moc"
