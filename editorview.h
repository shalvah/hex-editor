#pragma once
#include <QTableView>
#include <QStyledItemDelegate>
#include "editormodel.h"


/**
 * @brief The EditorDelegate class is the Delegate.
 * It controls the editor widget shown when a cell enters edit mode,
 * and validates input before committing it back to the model.
 */
class EditorDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit EditorDelegate(QObject *parent = nullptr);

    /*
     * Create the editing field shown to the user when they double-click a cell to edit.
     */
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    /*
     * Initialize the editing field with data.
     */
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

    /*
     * When user finishes editing, send their edits to the EditorModel.
     */
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;
};

/**
 * @brief The EditorView class is the View.
 */
class EditorView : public QTableView {
    Q_OBJECT

public:
    explicit EditorView(QWidget *parent = nullptr);

    /*
     * Prepare and set the EditorModel to be used.
     */
    void setEditorModel(EditorModel *model);

protected:
    /*
     * We override paintEvent to add some custom visual guides, such as panel separators.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    /*
     * Set widths for each column based on panel type.
     */
    void applyColumnWidths();

    EditorModel    *m_editorModel = nullptr;
    EditorDelegate  m_delegate;
};