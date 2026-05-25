#pragma once
#include <QAbstractTableModel>
#include <QSet>

class ByteBuffer;

/**
 * @brief The EditorModel class is the ViewModel, connecting Qt's views to the ByteBuffer data model.
 */
class EditorModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit EditorModel(ByteBuffer &buffer, QObject *parent = nullptr);

    // QAbstractTableModel interface methods
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    /*
     * For a given index in the table, return the data to be displayed in it.
     * Formats the byte according to the panel (Hex, Char, Bin), and highlights modified cells.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /*
     * Set data for the row and column headers (byte offsets).
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    /*
     * Set properties of each cell
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    /*
     * Given some input for a cell, parse and validate the input, persist it in the ByteBuffer,
     * and notify UI of changes.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /*
     * Call after external changes to ByteBuffer (e.g. file load).
     */
    void reload();

    enum class Panel { Hex, Char, Bin };
    /*
     * Which panel does this column belong to?
     * With a row size of 8, the first 8 columns will be Hex, the next 8 Char, and the last 8 Bin.
     */
    static Panel panelForColumn(int col);

    /*
     * Given a row and column, what byte index in the ByteBuffer is this?
     */
    int byteIndex(int row, int col) const;

    void setHighlightedByte(int byteIndex);

    void setSearchMatches(const QList<int> &matches, int current, int matchLength);

private:
    /*
     * Format a byte for display in the UI
     */
    QVariant formatByte(quint8 byte, Panel panel) const;
    /*
     * Validate and parse an incoming edit, based on the panel it was done in.
     */
    bool parseEdit(const QString &text, Panel panel, quint8 &outByte) const;

    ByteBuffer &m_buffer;
    int m_highlightedByte = -1;

    QList<int> m_searchMatches; // List of byte indices of matches
    int        m_currentMatch = -1; // Byte index of current match
    int        m_matchLength = 0; // Length of the search string
};