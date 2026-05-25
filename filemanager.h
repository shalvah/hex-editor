#pragma once
#include <QString>

class ByteBuffer;

/**
 * @brief The FileManager class is the bridge between our data model and the filesystem.
 * Responsible for loading from and writing to files.
 */
class FileManager {
public:
    enum class Error {
        None,
        NotFound,
        TooLarge,
        ReadFailed,
        WriteFailed,
        CouldNotOpenFileForReading,
        CouldNotOpenFileForWriting,
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