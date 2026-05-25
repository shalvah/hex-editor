#include "filemanager.h"
#include "bytebuffer.h"
#include "constants.h"
#include <QFile>
#include <QSaveFile>
#include <QFileInfo>

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
        if (!QFileInfo::exists(path)) return Error::NotFound;
        return Error::CouldNotOpenFileForReading;
    }

    if (file.size() > Constants::MAX_FILE_SIZE)
        return Error::TooLarge;

    QByteArray data = file.readAll();
    if (data.isEmpty() && file.error() != QFileDevice::NoError)
        return Error::ReadFailed;

    buffer.load(data);
    return Error::None;
}

FileManager::Error FileManager::saveFile(const QString &path, ByteBuffer &buffer) {
    QSaveFile file(path);
    // On Windows, QSaveFile might fail to overwrite files if they are in use by other
    // processes (like antivirus) or if they are QTemporaryFiles.
    // Fallback to direct write, since user changes should take precedence.
    file.setDirectWriteFallback(true);
    
    if (!file.open(QIODevice::WriteOnly))
        return Error::CouldNotOpenFileForWriting;

    qint64 written = file.write(buffer.rawData());
    if (written != buffer.rawData().size()) {
        file.cancelWriting();
        return Error::WriteFailed;
    }

    if (!file.commit()) {
        return Error::WriteFailed;
    }

    buffer.clearModified();
    return Error::None;
}