#include "filemanager.h"
#include "bytebuffer.h"
#include "constants.h"
#include <QFile>
#include <QFileInfo>

QString FileManager::errorMessage(Error error) {
    switch (error) {
    case Error::None:        return "No error";
    case Error::NotFound:    return "File not found";
    case Error::NoPermission:return "Permission denied";
    case Error::TooLarge:    return "File exceeds 100 MB limit";
    case Error::ReadFailed:  return "Failed to read file";
    case Error::WriteFailed: return "Failed to write file";
    }
    return "Unknown error";
}

FileManager::Error FileManager::loadFile(const QString &path, ByteBuffer &buffer) {
    QFile file(path);
    // Avoid race conditions between time-of-open and time-of-use: open file first, then handle possible errors.
    if (!file.open(QIODevice::ReadOnly)) {
        if (!QFileInfo::exists(path))
            return Error::NotFound;
        return Error::NoPermission;
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
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return Error::WriteFailed;

    qint64 written = file.write(buffer.rawData());
    if (written != buffer.rawData().size())
        return Error::WriteFailed;

    buffer.clearModified();
    return Error::None;
}