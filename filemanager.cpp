#include "filemanager.h"
#include "bytebuffer.h"
#include "constants.h"
#include <QFile>
#include <QSaveFile>
#include <QFileInfo>
#include <QDebug>

QString FileManager::errorMessage(Error error) {
    switch (error) {
    case Error::None:                       return "No error";
    case Error::NotFound:                   return "File not found";
    case Error::TooLarge:                   return "File exceeds 100 MB limit";
    case Error::ReadFailed:                 return "Failed to read file";
    case Error::WriteFailed:                return "Failed to write file";
    case Error::CouldNotOpenFileForWriting: return "Could not open file for writing";
    case Error::CouldNotOpenFileForReading: return "Could not open file for reading";
    }
    return "Unknown error";
}

FileManager::Error FileManager::loadFile(const QString &path, ByteBuffer &buffer) {
    QFile file(path);
    // Avoid race conditions between time-of-open and time-of-use: open file first, then handle possible errors.
    if (!file.open(QIODevice::ReadOnly)) {
        if (!QFileInfo::exists(path)) {
            qWarning() << "FileManager::loadFile: File not found:" << path;
            return Error::NotFound;
        }
        qWarning() << "FileManager::loadFile: Could not open file for reading:" << path;
        return Error::CouldNotOpenFileForReading;
    }

    if (file.size() > Constants::MAX_FILE_SIZE) {
        qWarning() << "FileManager::loadFile: File exceeds limit:" << path << file.size() << "bytes";
        return Error::TooLarge;
    }

    QByteArray data = file.readAll();
    if (data.isEmpty() && file.error() != QFileDevice::NoError) {
        qWarning() << "FileManager::loadFile: Read failed:" << path << file.errorString();
        return Error::ReadFailed;
    }

    buffer.load(data);
    return Error::None;
}

FileManager::Error FileManager::saveFile(const QString &path, ByteBuffer &buffer) {
    QSaveFile file(path);
    // On Windows, QSaveFile might fail to overwrite files if they are in use by other
    // processes (like antivirus) or if they are QTemporaryFiles.
    // Fallback to direct write, since user changes should take precedence.
    file.setDirectWriteFallback(true);
    
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "FileManager::saveFile: Could not open file for writing:" << path;
        return Error::CouldNotOpenFileForWriting;
    }

    qint64 written = file.write(buffer.rawData());
    if (written != buffer.rawData().size()) {
        file.cancelWriting();
        qWarning() << "FileManager::saveFile: Write failed (size mismatch):" << path;
        return Error::WriteFailed;
    }

    if (!file.commit()) {
        qWarning() << "FileManager::saveFile: Commit failed:" << path << file.errorString();
        return Error::WriteFailed;
    }

    buffer.clearModified();
    return Error::None;
}