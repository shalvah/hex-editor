#pragma once
#include <QWidget>
#include <QList>
#include "editormodel.h"

class ByteBuffer;
class QLineEdit;
class QLabel;
class QComboBox;

class FindPanelTest;

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
    void findNext();
    void findPrevious();
    void clearResults();

private:
    QList<int> search(const QString &text, EditorModel::Panel mode, int &outMatchLength) const;
    void updateStatus();

    ByteBuffer   &m_buffer;
    EditorModel  &m_model;

    QComboBox    *m_modeCombo  = nullptr;
    QLineEdit    *m_input      = nullptr;
    QLabel       *m_status     = nullptr;

    QList<int>    m_matches;   // byte indices of match starts
    int           m_current = -1; // Index of current match in m_matches array
};