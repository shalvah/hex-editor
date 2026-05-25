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
        
        // Invalid bin
        result = model.setData(model.index(0, 16), "1234", Qt::EditRole);
        QVERIFY(!result);
        QCOMPARE(buf.byteAt(0), quint8(0x00)); // unchanged
    }

    void setDataWhitespaceInChar() {
        ByteBuffer buf;
        buf.load(QByteArray("\x41", 1)); // 'A'
        EditorModel model(buf);
        bool result = model.setData(model.index(0, 8), " ", Qt::EditRole);
        
        QVERIFY(result);
        QCOMPARE(buf.byteAt(0), quint8(' '));
    }

    void outOfBoundsPrintableCharactersRenderAsDot() {
        ByteBuffer buf;
        // 0x7F (DEL) and 0x80 (extended ASCII) are typically non-printable
        buf.load(QByteArray("\x7F\x80", 2)); 
        EditorModel model(buf);
        // CHAR panel byte 0 (0x7F)
        QCOMPARE(model.data(model.index(0, 8)).toString(), QString("·"));
        // CHAR panel byte 1 (0x80)
        QCOMPARE(model.data(model.index(0, 9)).toString(), QString("·"));
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

    void headerDataFormatting() {
        ByteBuffer buf;
        buf.load(QByteArray(20, 0x00)); // 3 rows
        EditorModel model(buf);

        // Vertical headers (row offsets in hex)
        QCOMPARE(model.headerData(0, Qt::Vertical).toString(), QString("0x00000000"));
        QCOMPARE(model.headerData(1, Qt::Vertical).toString(), QString("0x00000008"));
        QCOMPARE(model.headerData(2, Qt::Vertical).toString(), QString("0x00000010"));

        // Horizontal headers (Hex panel)
        QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("0"));
        QCOMPARE(model.headerData(7, Qt::Horizontal).toString(), QString("7"));

        // Horizontal headers (Char panel)
        QCOMPARE(model.headerData(8, Qt::Horizontal).toString(), QString("0"));
        QCOMPARE(model.headerData(15, Qt::Horizontal).toString(), QString("7"));

        // Horizontal headers (Bin panel)
        QCOMPARE(model.headerData(16, Qt::Horizontal).toString(), QString("0"));
        QCOMPARE(model.headerData(23, Qt::Horizontal).toString(), QString("7"));
    }

    void flagsForCells() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1)); // Only 1 byte loaded
        EditorModel model(buf);

        // Valid cells should be selectable, enabled, and editable
        Qt::ItemFlags validFlags = model.flags(model.index(0, 0));
        QVERIFY(validFlags & Qt::ItemIsSelectable);
        QVERIFY(validFlags & Qt::ItemIsEnabled);
        QVERIFY(validFlags & Qt::ItemIsEditable);

        // Out of bounds cells in the row should just return NoItemFlags
        Qt::ItemFlags invalidFlags = model.flags(model.index(0, 1));
        QCOMPARE(invalidFlags, Qt::NoItemFlags);
    }

    void outOfBoundsCellsReturnEmptyData() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1)); // Only 1 byte loaded
        EditorModel model(buf);

        // Cell 0 is valid, should have string
        QCOMPARE(model.data(model.index(0, 0)).toString(), QString("00"));
        
        // Cell 1 is out of bounds, should return invalid QVariant
        QVERIFY(!model.data(model.index(0, 1)).isValid());
        QVERIFY(!model.data(model.index(0, 9)).isValid()); // Char panel
        QVERIFY(!model.data(model.index(0, 17)).isValid()); // Bin panel
    }

    void reloadClearsState() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x00", 2));
        EditorModel model(buf);

        // Set state
        model.setHighlightedByte(1);
        model.setSearchMatches({0}, 0, 1);
        
        // Ensure background color shows match is set
        QVariant bg = model.data(model.index(0, 0), Qt::BackgroundRole);
        QVERIFY(bg.isValid());

        // Reload
        model.reload();

        // Background should be default (matches and highlight cleared)
        bg = model.data(model.index(0, 0), Qt::BackgroundRole);
        QVERIFY(!bg.isValid());
        bg = model.data(model.index(0, 1), Qt::BackgroundRole);
        QVERIFY(!bg.isValid());
    }

    void visualRoles() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x01", 2));
        EditorModel model(buf);
        
        // Default background
        QCOMPARE(model.data(model.index(0, 0), Qt::BackgroundRole), QVariant());

        // Background should change on modification
        buf.setByte(0, 0xFF);
        QVariant bg = model.data(model.index(0, 0), Qt::BackgroundRole);
        QVERIFY(bg.isValid());
        QCOMPARE(bg.value<QColor>(), QColor(0x66, 0x5D, 0x29));

        // Background should change on selecting/highlighting a cell
        model.setHighlightedByte(0);
        bg = model.data(model.index(0, 0), Qt::BackgroundRole);
        // Since highlight takes precedence over modified in our logic:
        QVERIFY(bg.isValid());
        
        // Background should change for search matches
        model.setHighlightedByte(-1); // clear
        model.setSearchMatches({0, 1}, 0, 1);
        bg = model.data(model.index(0, 0), Qt::BackgroundRole);
        QVERIFY(bg.isValid());
        QCOMPARE(bg.value<QColor>(), QColor(0xFF, 0x80, 0x00)); // current match color

        bg = model.data(model.index(0, 1), Qt::BackgroundRole);
        QVERIFY(bg.isValid());
        QCOMPARE(bg.value<QColor>(), QColor(0xFF, 0xD0, 0x80)); // other match color
    }
};

QTEST_MAIN(EditorModelTest)
#include "tst_editormodel.moc"
