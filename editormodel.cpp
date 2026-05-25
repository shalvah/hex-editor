#include "editormodel.h"
#include "bytebuffer.h"
#include "constants.h"
#include <QColor>
#include <QGuiApplication>
#include <QPalette>

EditorModel::EditorModel(ByteBuffer &buffer, QObject *parent)
    : QAbstractTableModel(parent), m_buffer(buffer) {}

int EditorModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    if (m_buffer.size() == 0) return 0;
    // The number of rows should be the number of bytes in the file,
    // factored into our bytes-per-row limit.
    return (m_buffer.size() + Constants::BYTES_PER_ROW - 1) / Constants::BYTES_PER_ROW;
}

int EditorModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    // Number of columns is always fixed as the bytes per row * no of panels.
    return Constants::TOTAL_COLUMNS;
}

EditorModel::Panel EditorModel::panelForColumn(int col) {
    if (col < Constants::BYTES_PER_ROW)                return Panel::Hex;
    if (col < Constants::BYTES_PER_ROW * 2)            return Panel::Char;
    return Panel::Bin;
}

int EditorModel::byteIndex(int row, int col) const {
    // First take the modulus, since all three panels map to the same N bytes per row
    int byteCol = col % Constants::BYTES_PER_ROW;
    return (row * Constants::BYTES_PER_ROW) + byteCol;
}

QVariant EditorModel::formatByte(quint8 byte, Panel panel) const {
    switch (panel) {
    case Panel::Hex:
        return QString::asprintf("%02X", byte);
    case Panel::Char:
        // Printable ASCII: 0x20–0x7E; everything else is invalid
        return (byte >= 0x20 && byte <= 0x7E)
                   ? QString(QChar(byte))
                   : QVariant();
    case Panel::Bin:
        return QString::number(byte, 2).rightJustified(8, '0');
    }
    return {};
}

QVariant EditorModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    int byteIdx = byteIndex(index.row(), index.column());
    if (byteIdx >= m_buffer.size()) return {};

    if (role == Qt::DisplayRole) {
        auto formatted = formatByte(m_buffer.byteAt(byteIdx), panelForColumn(index.column()));
        // For non-printable characters, we show a centre dot (·, U+00B7)
        return formatted.isValid() ? formatted : QString(u"·");
    }

    if (role == Qt::EditRole) {
        auto formatted = formatByte(m_buffer.byteAt(byteIdx), panelForColumn(index.column()));
        return formatted.isValid() ? formatted : QString("");
    }

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if (role == Qt::BackgroundRole) {
        if (m_buffer.modifiedIndices().contains(byteIdx))
            return QColor(0x66, 0x5D, 0x29); // yellow — modified
        if (byteIdx == m_currentMatch)
            return QColor(0xFF, 0x80, 0x00); // orange — current match
        if (m_searchMatches.contains(byteIdx))
            return QColor(0xFF, 0xD0, 0x80); // light orange — other matches
        if (byteIdx == m_highlightedByte) {
             // Set accent colour on currently highlighted cell, so we can sync to other panels
            QPalette palette = QGuiApplication::palette();
            return palette.color(QPalette::Highlight);
        }
    }

    return {};
}

QVariant EditorModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return {};

    if (orientation == Qt::Horizontal) {
        // Label each column group
        int byteCol = section % Constants::BYTES_PER_ROW;
        return QString::number(byteCol);
    }

    if (orientation == Qt::Vertical) {
        // Row header shows the byte offset of the first byte in that row
        return QString("0x%1").arg(section * Constants::BYTES_PER_ROW, 4, 16, QChar('0')).toUpper();
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
        // Do not trim here, to allow entering space character
        if (text.size() == 1) {
            outByte = static_cast<quint8>(text[0].toLatin1());
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
        return false; // TODO Add visual feedback for invalid edits

    m_buffer.setByte(byteIdx, newByte);

    // The same byte appears in 3 columns — invalidate all three panels for this row
    emit dataChanged(
        this->index(index.row(), 0),
        this->index(index.row(), Constants::TOTAL_COLUMNS - 1)
        );

    return true;
}

void EditorModel::setHighlightedByte(int byteIndex) {
    int old = m_highlightedByte;
    m_highlightedByte = byteIndex;

    // Invalidate old and new highlight rows so Qt redraws them
    auto invalidateRow = [&](int idx) {
        if (idx < 0) return;
        int row = idx / Constants::BYTES_PER_ROW;
        emit dataChanged(index(row, 0), index(row, Constants::TOTAL_COLUMNS - 1));
    };
    invalidateRow(old);
    invalidateRow(byteIndex);
}

void EditorModel::setSearchMatches(const QList<int> &matches, int current) {
    m_searchMatches = matches;
    m_currentMatch = (current >= 0 && current < matches.size()) ? matches[current] : -1;
    // Emit dataChanged, forcing rerender of the cells
    emit dataChanged(index(0, 0), index(rowCount() - 1, Constants::TOTAL_COLUMNS - 1));
}

void EditorModel::reload() {
    // Underlying data has been replaced; inform the view to redraw.
    beginResetModel();
    m_highlightedByte = -1;
    m_searchMatches.clear();
    m_currentMatch = -1;
    endResetModel();
}