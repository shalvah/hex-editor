#include <QtTest>
#include <QTemporaryFile>
#include <QDir>
#include <QUuid>
#include "../filemanager.h"
#include "../bytebuffer.h"
#include "../constants.h"

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
        // Create a temporary file path manually, as QTemporaryFile creates files with
        // an exclusive lock on Windows, which causes QSaveFile to fail during commit.
        QString tempPath = QDir::tempPath() + "/hex_test_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".bin";
        QFile tmp(tempPath);
        QVERIFY(tmp.open(QIODevice::WriteOnly));
        QByteArray original("\x11\x22\x33\x44", 4);
        tmp.write(original);
        tmp.close();

        // Load into buffer
        FileManager fm;
        ByteBuffer buf;
        auto loadErr = fm.loadFile(tempPath, buf);
        QCOMPARE(loadErr, FileManager::Error::None);
        QCOMPARE(buf.rawData(), original);
        QVERIFY(!buf.isModified());

        // Modify and save
        buf.setByte(0, 0xFF);
        QVERIFY(buf.isModified());

        auto saveErr = fm.saveFile(tempPath, buf);
        QCOMPARE(saveErr, FileManager::Error::None);
        QVERIFY(!buf.isModified()); // cleared after save

        // Reload and verify
        ByteBuffer buf2;
        fm.loadFile(tempPath, buf2);
        QCOMPARE(buf2.byteAt(0), quint8(0xFF));
        QCOMPARE(buf2.byteAt(1), quint8(0x22)); // unchanged

        QFile::remove(tempPath);
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

    void loadFileTooLarge() {
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        // Sparse file: skip writing actual data, just set size
        tmp.resize(Constants::MAX_FILE_SIZE + 1);
        tmp.close();

        FileManager fm;
        ByteBuffer buf;
        auto err = fm.loadFile(tmp.fileName(), buf);
        QCOMPARE(err, FileManager::Error::TooLarge);
    }

    void saveToInvalidPath() {
        FileManager fm;
        ByteBuffer buf;
        buf.load(QByteArray("data"));
        
        auto err = fm.saveFile("/invalid_directory_that_doesnt_exist/file.bin", buf);
        QCOMPARE(err, FileManager::Error::CouldNotOpenFileForWriting);
    }

    void errorMessagesAreValid() {
        QVERIFY(!FileManager::errorMessage(FileManager::Error::None).isEmpty());
        QVERIFY(!FileManager::errorMessage(FileManager::Error::NotFound).isEmpty());
        QVERIFY(!FileManager::errorMessage(FileManager::Error::TooLarge).isEmpty());
        QVERIFY(!FileManager::errorMessage(FileManager::Error::ReadFailed).isEmpty());
        QVERIFY(!FileManager::errorMessage(FileManager::Error::WriteFailed).isEmpty());
        QVERIFY(!FileManager::errorMessage(FileManager::Error::CouldNotOpenFileForReading).isEmpty());
        QVERIFY(!FileManager::errorMessage(FileManager::Error::CouldNotOpenFileForWriting).isEmpty());
    }
};

QTEST_MAIN(FileManagerTest)
#include "tst_filemanager.moc"
