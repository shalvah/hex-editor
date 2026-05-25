#pragma once
#include <QTableView>
#include <QStyledItemDelegate>
#include "editormodel.h"

// ── Delegate ────────────────────────────────────────────────────────────────
// Controls the editor widget shown when a cell enters edit mode,
// and validates input before committing it back to the model.

class EditorDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit EditorDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;
};

// ── View ────────────────────────────────────────────────────────────────────

class EditorView : public QTableView {
    Q_OBJECT

public:
    explicit EditorView(QWidget *parent = nullptr);

    void setEditorModel(EditorModel *model);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void applyColumnWidths();

    EditorModel    *m_editorModel = nullptr;
    EditorDelegate  m_delegate;
};