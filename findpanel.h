#pragma once
#include <QWidget>
#include <QList>
#include "editormodel.h"

class ByteBuffer;
class QLineEdit;
class QLabel;
class QComboBox;

class FindPanelTest;

/**
 * @brief The FindPanel class is the UI widget responsible for the Find functionality.
 * Users can search for some text, and FindPanel finds all matches for that series of bytes, matching the selected mode.
 * For instance, in CHAR mode, searching for "Hey" will highlight all cases of three contiguous bytes "H", "E", "Y".
 * In HEX mode, searching for "732f" will find all cases of 0x73 followed by 0x2F, and so on.
 */
class FindPanel : public QWidget {
    Q_OBJECT
    friend class FindPanelTest;

public:
    explicit FindPanel(ByteBuffer &buffer, EditorModel &model, QWidget *parent = nullptr);

    /*
     * Show and focus the input field
     */
    void activate();

signals:
    void requestScrollToRow(int row);

private slots:
    /*
     * Rerun the search, advancing the current match to the next occurrence.
     */
    void findNext();
    /*
     * Rerun the search, advancing the current match to the previous occurrence.
     */
    void findPrevious();
    /*
     * Clear search results.
     */
    void clearResults();

private:
    /*
     * Given some search text and the search mode (Hex/Char/Bin), find all matches,
     * reporting the length of the match in outMatchLength.
     */
    QList<int> search(const QString &text, EditorModel::Panel mode, int &outMatchLength) const;

    /*
     * Update the status label ("x/N", "No matches"), based on the current match details.
     */
    void updateStatusLabel();

    ByteBuffer   &m_buffer;
    EditorModel  &m_model;

    QComboBox    *m_modeCombo  = nullptr;
    QLineEdit    *m_input      = nullptr;
    QLabel       *m_status     = nullptr;

    /*
     * Byte indices of starting locations for each match
     */
    QList<int> m_matches;
    /*
     * Index of the currently selected match (in m_matches array, not in the ByteBuffer)
     */
    int m_current = -1;
};