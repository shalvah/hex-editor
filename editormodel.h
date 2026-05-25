#pragma once
#include <QAbstractTableModel>
#include <QSet>

class ByteBuffer;

class EditorModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit EditorModel(ByteBuffer &buffer, QObject *parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Call after external changes to ByteBuffer (e.g. file load)
    void reload();

    static constexpr int BYTES_PER_ROW = 8;
    static constexpr int PANEL_COUNT   = 3;
    static constexpr int TOTAL_COLUMNS = BYTES_PER_ROW * PANEL_COUNT; // 24

    // Which panel does this column belong to?
    enum class Panel { Hex, Char, Bin };
    static Panel panelForColumn(int col);

    // The byte index in ByteBuffer for a given (row, col)
    int byteIndex(int row, int col) const;

private:
    QVariant formatByte(quint8 byte, Panel panel) const;
    bool parseEdit(const QString &text, Panel panel, quint8 &outByte) const;

    ByteBuffer &m_buffer;
};