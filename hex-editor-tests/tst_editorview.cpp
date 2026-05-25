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
};

QTEST_MAIN(EditorViewTest)
#include "tst_editorview.moc"