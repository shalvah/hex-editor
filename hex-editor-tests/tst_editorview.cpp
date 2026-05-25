#include <QtTest>
#include <QLineEdit>
#include "../editorview.h"
#include "../editormodel.h"
#include "../bytebuffer.h"

class EditorViewTest : public QObject {
    Q_OBJECT

private slots:
    void delegateInputMasks() {
        EditorDelegate delegate;
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x00", 2));
        EditorModel model(buf);
        QTableView view;
        view.setModel(&model);

        // Hex Panel
        QModelIndex hexIndex = model.index(0, 0);
        QWidget *hexEditor = delegate.createEditor(&view, QStyleOptionViewItem(), hexIndex);
        QLineEdit *hexLine = qobject_cast<QLineEdit*>(hexEditor);
        QVERIFY(hexLine);
        QCOMPARE(hexLine->inputMask(), QString("HH"));
        QCOMPARE(hexLine->maxLength(), 2);
        delete hexEditor;

        // Char Panel
        QModelIndex charIndex = model.index(0, 8); // 8 is the start of the char panel
        QWidget *charEditor = delegate.createEditor(&view, QStyleOptionViewItem(), charIndex);
        QLineEdit *charLine = qobject_cast<QLineEdit*>(charEditor);
        QVERIFY(charLine);
        QCOMPARE(charLine->inputMask(), QString("")); // No mask for char
        QCOMPARE(charLine->maxLength(), 1);
        delete charEditor;

        // Bin Panel
        QModelIndex binIndex = model.index(0, 16); // 16 is the start of the bin panel
        QWidget *binEditor = delegate.createEditor(&view, QStyleOptionViewItem(), binIndex);
        QLineEdit *binLine = qobject_cast<QLineEdit*>(binEditor);
        QVERIFY(binLine);
        QCOMPARE(binLine->inputMask(), QString("BBBBBBBB"));
        QCOMPARE(binLine->maxLength(), 8);
        delete binEditor;
    }

    void delegateModelDataRouting() {
        EditorDelegate delegate;
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        EditorModel model(buf);

        QTableView view;
        view.setModel(&model);
        QModelIndex index = model.index(0, 0);

        QWidget *editor = delegate.createEditor(&view, QStyleOptionViewItem(), index);
        QLineEdit *line = qobject_cast<QLineEdit*>(editor);
        
        // Simulate typing "FF"
        line->setText("FF");
        
        // Push data to model
        delegate.setModelData(editor, &model, index);
        
        QCOMPARE(buf.byteAt(0), quint8(0xFF));
        delete editor;
    }

    void delegateSetEditorData() {
        EditorDelegate delegate;
        ByteBuffer buf;
        buf.load(QByteArray("\xAB", 1)); // Loads 0xAB
        EditorModel model(buf);

        QTableView view;
        view.setModel(&model);
        QModelIndex index = model.index(0, 0); // Hex column

        QWidget *editor = delegate.createEditor(&view, QStyleOptionViewItem(), index);
        QLineEdit *line = qobject_cast<QLineEdit*>(editor);
        
        // Pull data from model to editor
        delegate.setEditorData(editor, index);
        
        QCOMPARE(line->text(), QString("AB"));
        QVERIFY(line->hasSelectedText()); // Should select all text automatically
        
        delete editor;
    }

    void viewAppliesColumnWidths() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00", 1));
        EditorModel model(buf);
        EditorView view;
        
        view.setEditorModel(&model);
        
        // Hex columns (0-7) should be 36px
        QCOMPARE(view.columnWidth(0), 36);
        QCOMPARE(view.columnWidth(7), 36);
        
        // Char columns (8-15) should be 24px
        QCOMPARE(view.columnWidth(8), 24);
        QCOMPARE(view.columnWidth(15), 24);
        
        // Bin columns (16-23) should be 72px
        QCOMPARE(view.columnWidth(16), 72);
        QCOMPARE(view.columnWidth(23), 72);
    }

    void viewSelectionUpdatesHighlight() {
        ByteBuffer buf;
        buf.load(QByteArray("\x00\x01\x02", 3));
        EditorModel model(buf);
        EditorView view;
        
        view.setEditorModel(&model);
        
        // Select the second byte in the Char panel (row 0, col 9)
        QModelIndex charIndex = model.index(0, 9);
        view.selectionModel()->setCurrentIndex(charIndex, QItemSelectionModel::Select);
        
        // Let Qt's event loop process the selection signal
        QCoreApplication::processEvents();
        
        // We can verify this via the EditorModel's internal highlighted state
        // By checking if the background role is active on the corresponding hex cell
        QVariant bg = model.data(model.index(0, 1), Qt::BackgroundRole);
        QVERIFY(bg.isValid());
    }
};

QTEST_MAIN(EditorViewTest)
#include "tst_editorview.moc"