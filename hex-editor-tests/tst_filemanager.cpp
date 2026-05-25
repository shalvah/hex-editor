#include <QtTest>
#include <QTemporaryFile>
#include "../filemanager.h"
#include "../bytebuffer.h"

class FileManagerTest : public QObject {
    Q_OBJECT

private slots:
    void loadNonExistentFile() {
        FileManager fm;
        ByteBuffer buf;
        auto err = fm.loadFile("/nonexistent/path/file.bin", buf);
        QCOMPARE(err, FileManager::Error::NotFound);
    }

    void loadAndSaveRoundtrip() {
        // Write known bytes to a temp file
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        QByteArray original("\x11\x22\x33\x44", 4);
        tmp.write(original);
        tmp.close();

        // Load into buffer
        FileManager fm;
        ByteBuffer buf;
        auto loadErr = fm.loadFile(tmp.fileName(), buf);
        QCOMPARE(loadErr, FileManager::Error::None);
        QCOMPARE(buf.rawData(), original);
        QVERIFY(!buf.isModified());

        // Modify and save
        buf.setByte(0, 0xFF);
        QVERIFY(buf.isModified());
        auto saveErr = fm.saveFile(tmp.fileName(), buf);
        QCOMPARE(saveErr, FileManager::Error::None);
        QVERIFY(!buf.isModified()); // cleared after save

        // Reload and verify
        ByteBuffer buf2;
        fm.loadFile(tmp.fileName(), buf2);
        QCOMPARE(buf2.byteAt(0), quint8(0xFF));
        QCOMPARE(buf2.byteAt(1), quint8(0x22)); // unchanged
    }

    void loadEmptyFile() {
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.close();

        FileManager fm;
        ByteBuffer buf;
        auto err = fm.loadFile(tmp.fileName(), buf);
        QCOMPARE(err, FileManager::Error::None);
        QCOMPARE(buf.size(), 0);
    }
};

QTEST_MAIN(FileManagerTest)
#include "tst_filemanager.moc"
