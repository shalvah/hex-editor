#include <QtTest>
#include "../bytebuffer.h"

class ByteBufferTest : public QObject {
    Q_OBJECT

private slots:
    void loadSetsData() {
        ByteBuffer buf;
        buf.load(QByteArray("\x01\x02\x03", 3));
        QCOMPARE(buf.size(), 3);
        QCOMPARE(buf.byteAt(0), quint8(0x01));
        QCOMPARE(buf.byteAt(2), quint8(0x03));
    }

    void loadClearsModified() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        buf.setByte(0, 0xFF);
        QVERIFY(buf.isModified());

        buf.load(QByteArray("\x00", 1)); // reload
        QVERIFY(!buf.isModified());
    }

    void setByteTracksChange() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x00", 2));
        buf.setByte(1, 0xAB);
        QCOMPARE(buf.byteAt(1), quint8(0xAB));
        QVERIFY(buf.modifiedIndices().contains(1));
        QVERIFY(!buf.modifiedIndices().contains(0));
    }

    void setByteNoChangeNotTracked() {
        // Writing the same value shouldn't mark it as modified
        ByteBuffer buf;
        buf.load(QByteArray("\x42", 1));
        buf.setByte(0, 0x42);
        QVERIFY(!buf.isModified());
    }

    void setByteOutOfBoundsIgnored() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        buf.setByte(-1, 0xFF); // should not crash
        buf.setByte(99, 0xFF); // should not crash
        QVERIFY(!buf.isModified());
    }

    void getByteOutOfBoundsReturnsZero() {
        ByteBuffer buf;
        buf.load(QByteArray("\x01\x02", 2));
        QCOMPARE(buf.byteAt(-1), quint8(0));
        QCOMPARE(buf.byteAt(2), quint8(0));
        QCOMPARE(buf.byteAt(99), quint8(0));
    }

    void clearModified() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        buf.setByte(0, 0xFF);
        buf.clearModified();
        QVERIFY(!buf.isModified());
        QCOMPARE(buf.byteAt(0), quint8(0xFF)); // data still changed
    }

    void rawDataMatchesContent() {
        QByteArray original("\x11\x22\x33", 3);
        ByteBuffer buf;
        buf.load(original);
        QCOMPARE(buf.rawData(), original);
    }
};

QTEST_MAIN(ByteBufferTest)
#include "tst_bytebuffer.moc"
