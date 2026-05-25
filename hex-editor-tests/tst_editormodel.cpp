#include <QtTest>
#include "../bytebuffer.h"
#include "../editormodel.h"

class EditorModelTest : public QObject {
    Q_OBJECT

private slots:
    void rowAndColumnCountFollowsBytesPerRow() {
        ByteBuffer buf;
        buf.load(QByteArray(20, 0x00)); // 20 bytes → 3 rows (8+8+4)
        EditorModel model(buf);
        QCOMPARE(model.rowCount(), 3);
        QCOMPARE(model.columnCount(), 24);
    }

    void emptyBufferHasNoRows() {
        ByteBuffer buf;
        EditorModel model(buf);
        QCOMPARE(model.rowCount(), 0);
    }

    void panelsRenderWithProperFormatting() {
        ByteBuffer buf;
        buf.load(QByteArray("\x41\x7B", 2)); // "A{"
        EditorModel model(buf);
        // Column 0 = HEX panel, byte 0
        QCOMPARE(model.data(model.index(0, 0)).toString(), QString("41"));
        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("7B"));
        // Column 8 = CHAR panel, byte 0
        QCOMPARE(model.data(model.index(0, 8)).toString(), QString("A"));
        QCOMPARE(model.data(model.index(0, 9)).toString(), QString("{"));
        // Column 16 = BIN panel, byte 0
        QCOMPARE(model.data(model.index(0, 16)).toString(), QString("01000001"));
        QCOMPARE(model.data(model.index(0, 17)).toString(), QString("01111011"));
    }

    void nonPrintableCharactersRenderAsDotInCharPanel() {
        ByteBuffer buf;
        buf.load(QByteArray("\x01", 2)); // non-printable
        EditorModel model(buf);
        // Column 0 = HEX panel, byte 0
        QCOMPARE(model.data(model.index(0, 0)).toString(), QString("01"));
        // Column 8 = CHAR panel, byte 0
        QCOMPARE(model.data(model.index(0, 8)).toString(), QString("·"));
        // Column 16 = BIN panel, byte 0
        QCOMPARE(model.data(model.index(0, 16)).toString(), QString("00000001"));
    }

    void setDataPerPanel() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x00\x00", 3));
        EditorModel model(buf);
        // Column 0 = HEX panel, byte 0
        model.setData(model.index(0, 0), "7B", Qt::EditRole);
        QCOMPARE(buf.byteAt(0), quint8(0x7B));
        // Column 9 = CHAR panel, byte 1
        model.setData(model.index(0, 9), "|", Qt::EditRole);
        QCOMPARE(buf.byteAt(1), quint8(0x7C));
        // Column 18 = BIN panel, byte 2
        model.setData(model.index(0, 18), "01111101", Qt::EditRole);
        QCOMPARE(buf.byteAt(2), quint8(0x7D));
    }

    void setDataSyncsPanels() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        EditorModel model(buf);
        // Edit via HEX panel
        model.setData(model.index(0, 0), "41", Qt::EditRole);
        // CHAR panel should now show 'A'
        QCOMPARE(model.data(model.index(0, 8)).toString(), QString("A"));
        // BIN panel should reflect it too
        QCOMPARE(model.data(model.index(0, 16)).toString(), QString("01000001"));
    }

    void setDataInvalidInputRejected() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        EditorModel model(buf);
        bool result = model.setData(model.index(0, 0), "ZZ", Qt::EditRole);
        QVERIFY(!result);
        QCOMPARE(buf.byteAt(0), quint8(0x00)); // unchanged
    }

    void byteIndexMapping() {
        ByteBuffer buf;
        buf.load(QByteArray(16, 0x00));
        EditorModel model(buf);
        // All three panels in row 0 map to bytes 0–7
        QCOMPARE(model.byteIndex(0, 0),  0); // HEX col 0
        QCOMPARE(model.byteIndex(0, 8),  0); // CHAR col 0
        QCOMPARE(model.byteIndex(0, 16), 0); // BIN col 0
        QCOMPARE(model.byteIndex(0, 7),  7); // HEX col 7
        QCOMPARE(model.byteIndex(1, 0),  8); // second row
    }
};

QTEST_MAIN(EditorModelTest)
#include "tst_editormodel.moc"
