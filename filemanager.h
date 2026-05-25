#pragma once
#include <QString>

class ByteBuffer;

class FileManager {
public:
    enum class Error {
        None,
        NotFound,
        NoPermission,
        TooLarge,
        ReadFailed,
        CouldNotOpenFileForWriting,
        WriteFailed,
    };

    static QString errorMessage(Error error);

    /*
     * Opens a file and loads it into a ByteBuffer
     */
    Error loadFile(const QString &path, ByteBuffer &buffer);

    /*
     * Writes the content in the ByteBuffer back to disk
     */
    Error saveFile(const QString &path, ByteBuffer &buffer);
};