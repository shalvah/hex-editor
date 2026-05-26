#include "bytebuffer.h"
#include <QDebug>

void ByteBuffer::load(const QByteArray &data) {
    m_data = data;
    m_modified.clear();
}

int ByteBuffer::size() const {
    return m_data.size();
}

quint8 ByteBuffer::byteAt(int index) const {
    if (index < 0 || index >= m_data.size()) {
        qWarning() << "ByteBuffer::byteAt out of bounds:" << index << "size:" << m_data.size();
        return 0;
    }
    return static_cast<quint8>(m_data.at(index));
}

void ByteBuffer::setByte(int index, quint8 value) {
    if (index < 0 || index >= m_data.size()) {
        qWarning() << "ByteBuffer::setByte out of bounds:" << index << "size:" << m_data.size();
        return; // TODO Should we consider handling out-of-bounds cases differently?
    }
    // Important to cast to uint, since char is signed on most platforms.
    if (static_cast<quint8>(m_data[index]) != value) {
        // All writes are overwrites. In future, we can implement an UndoStack allowing us to Undo/Redo.
        m_data[index] = static_cast<char>(value);
        m_modified.insert(index);
    }
}

const QByteArray &ByteBuffer::rawData() const {
    return m_data;
}

const QSet<int> &ByteBuffer::modifiedIndices() const {
    return m_modified;
}

void ByteBuffer::clearModified() {
    m_modified.clear();
}

bool ByteBuffer::isModified() const {
    return !m_modified.isEmpty();
}