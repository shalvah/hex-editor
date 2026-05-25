#pragma once
#include <QByteArray>
#include <QSet>

/**
 * @brief The ByteBuffer class is the data model for the editor.
 * It holds the file's bytes and tracks which byte indices have been modified.
 * All reads and writes to the in-memory file content (e.g. from FileManager, EditorModel) pass through this module.
 * All bytes are represented as quint8.
 */
class ByteBuffer {
public:
    ByteBuffer() = default;

    /*
     * Loads a new file into the buffer (also clears modified state).
     */
    void load(const QByteArray &data);

    /*
     * The number of bytes currently in the buffer
     */
    int size() const;

    /*
     * Returns the byte at a given index.
     */
    quint8 byteAt(int index) const;

    /*
     * Used to write to a given index. Currently, all writes are overwrites (no Undo/Redo).
     */
    void setByte(int index, quint8 value);

    /*
     * Returns a const reference to the underlying QByteArray. Used by callers to write the buffer back to disk.
     */
    const QByteArray &rawData() const;

    /*
     * Returns a list of modified indices. Useful for highlighting in display.
     */
    const QSet<int> &modifiedIndices() const;

    /*
     * Clears the modified state (eg when file is saved).
     */
    void clearModified();

    /*
     * Returns true if any changes have been made to file.
     */
    bool isModified() const;

private:
    QByteArray m_data;
    QSet<int>  m_modified;
};