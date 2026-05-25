#include "editormodel.h"
#include "bytebuffer.h"

EditorModel::EditorModel(ByteBuffer &buffer, QObject *parent)
    : QAbstractTableModel(parent), m_buffer(buffer) {}

int EditorModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    if (m_buffer.size() == 0) return 0;
    return (m_buffer.size() + BYTES_PER_ROW - 1) / BYTES_PER_ROW;
}

int EditorModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return TOTAL_COLUMNS;
}

EditorModel::Panel EditorModel::panelForColumn(int col) {
    if (col < BYTES_PER_ROW)                return Panel::Hex;
    if (col < BYTES_PER_ROW * 2)            return Panel::Char;
    return Panel::Bin;
}

int EditorModel::byteIndex(int row, int col) const {
    // All three panels map to the same 8 bytes per row
    int byteCol = col % BYTES_PER_ROW;
    return row * BYTES_PER_ROW + byteCol;
}

QVariant EditorModel::formatByte(quint8 byte, Panel panel) const {
    switch (panel) {
    case Panel::Hex:
        return QString("%1").arg(byte, 2, 16, QChar('0')).toUpper();
    case Panel::Char:
        // Printable ASCII: 0x20–0x7E; everything else shows as a dot
        return (byte >= 0x20 && byte <= 0x7E)
                   ? QString(QChar(byte))
                   : QString(".");
    case Panel::Bin:
        return QString("%1").arg(byte, 8, 2, QChar('0'));
    }
    return {};
}

QVariant EditorModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    int byteIdx = byteIndex(index.row(), index.column());
    if (byteIdx >= m_buffer.size()) return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return formatByte(m_buffer.byteAt(byteIdx), panelForColumn(index.column()));

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if (role == Qt::BackgroundRole && m_buffer.modifiedIndices().contains(byteIdx))
        return QColor(0xFF, 0xF0, 0xA0); // soft yellow for modified bytes

    return {};
}

QVariant EditorModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return {};

    if (orientation == Qt::Horizontal) {
        // Label each column group
        int byteCol = section % BYTES_PER_ROW;
        return QString::number(byteCol);
    }

    if (orientation == Qt::Vertical) {
        // Row header shows the byte offset of the first byte in that row
        return QString("0x%1").arg(section * BYTES_PER_ROW, 4, 16, QChar('0')).toUpper();
    }

    return {};
}

Qt::ItemFlags EditorModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    int byteIdx = byteIndex(index.row(), index.column());
    if (byteIdx >= m_buffer.size()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool EditorModel::parseEdit(const QString &text, Panel panel, quint8 &outByte) const {
    bool ok = false;
    QString trimmed = text.trimmed();

    switch (panel) {
    case Panel::Hex: {
        uint val = trimmed.toUInt(&ok, 16);
        if (ok && val <= 0xFF) { outByte = static_cast<quint8>(val); return true; }
        return false;
    }
    case Panel::Char: {
        if (trimmed.size() == 1) {
            outByte = static_cast<quint8>(trimmed[0].toLatin1());
            return true;
        }
        return false;
    }
    case Panel::Bin: {
        uint val = trimmed.toUInt(&ok, 2);
        if (ok && val <= 0xFF) { outByte = static_cast<quint8>(val); return true; }
        return false;
    }
    }
    return false;
}

bool EditorModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole || !index.isValid()) return false;

    int byteIdx = byteIndex(index.row(), index.column());
    if (byteIdx >= m_buffer.size()) return false;

    quint8 newByte;
    if (!parseEdit(value.toString(), panelForColumn(index.column()), newByte))
        return false;

    m_buffer.setByte(byteIdx, newByte);

    // The same byte appears in 3 columns — invalidate all three panels for this row
    emit dataChanged(
        this->index(index.row(), 0),
        this->index(index.row(), TOTAL_COLUMNS - 1)
        );

    return true;
}

void EditorModel::reload() {
    beginResetModel();
    endResetModel();
}