#include <QtTest>
#include <QTemporaryFile>
#include <QStackedWidget>
#include <QDockwidget>
#include <QMessageBox>
#include "../mainwindow.h"

class MainWindowTest : public QObject {
    Q_OBJECT

private slots:
    void transitionsBetweenEmptyStateAndEditorView() {
        MainWindow window;

        // Verify initial state: Stack should be on the empty state (index 0)
        QCOMPARE(window.m_stackedWidget->currentIndex(), 0);
        QCOMPARE(window.m_stackedWidget->currentWidget(), window.m_emptyStateWidget);

        // Create a temporary file to load
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.write("TEST");
        tmp.close();

        // Load the file
        window.openFile(tmp.fileName());

        // Verify state after loading: Stack should be on the editor view (index 1)
        QCOMPARE(window.m_stackedWidget->currentIndex(), 1);
        QCOMPARE(window.m_stackedWidget->currentWidget(), window.m_tableView);
        QCOMPARE(window.windowTitle(), QString("%1 (4 bytes) — Hex Editor").arg(tmp.fileName()));
    }

    void menuItemsEnabledStatusOnFileLoad() {
        MainWindow window;

        // Initially no file is loaded, so save and find actions should be disabled
        QVERIFY(!window.m_saveAction->isEnabled());
        QVERIFY(!window.m_findAction->isEnabled());

        // Create a temporary file to load
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.write("TEST");
        tmp.close();

        // Load the file
        window.openFile(tmp.fileName());

        // After loading a file, these actions should be enabled
        QVERIFY(window.m_saveAction->isEnabled());
        QVERIFY(window.m_findAction->isEnabled());
    }

    void windowTitleUpdatesOnModification() {
        MainWindow window;
        
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.write("TEST");
        tmp.close();

        window.openFile(tmp.fileName());
        
        QString fileName = tmp.fileName();
        QString expectedTitleUnmodified = QString("%1 (4 bytes) — Hex Editor").arg(fileName);
        QCOMPARE(window.windowTitle(), expectedTitleUnmodified);

        // Modify the buffer via the model
        // Assuming column 1 is the first hex byte column, which is editable
        QModelIndex index = window.m_model.index(0, 1);
        QVERIFY(window.m_model.setData(index, "AA", Qt::EditRole));

        // Wait for signals
        QCoreApplication::processEvents();

        QString expectedTitleModified = QString("%1* (4 bytes) — Hex Editor").arg(fileName);
        QCOMPARE(window.windowTitle(), expectedTitleModified);
    }

    void findPanelVisibility() {
        MainWindow window;

        // Verify the find panel (dock widget) is initially hidden
        QDockWidget *dock = window.findChild<QDockWidget *>();
        QVERIFY(dock != nullptr);
        QVERIFY(!dock->isVisible());

        // Load a file so find action is enabled
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.write("TEST");
        tmp.close();
        window.openFile(tmp.fileName());

        // The window must be visible for children to be truly visible.
        window.show();

        // Trigger find action
        window.m_findAction->trigger();

        // Verify find panel becomes visible
        QVERIFY(dock->isVisible());
    }

    void openFileFailedState() {
        MainWindow window;

        // Schedule a timer to intercept and dismiss the blocking QMessageBox
        // that gets created by QMessageBox::critical
        QTimer::singleShot(100, []() {
            QWidget *activeModal = QApplication::activeModalWidget();
            if (auto *msgBox = qobject_cast<QMessageBox *>(activeModal)) {
                msgBox->accept(); // Clicks "OK" to dismiss it
            }
        });

        // This call will block until our QTimer fires and dismisses the message box
        window.openFile("this_path_does_not_exist_and_will_fail.bin");

        // Verify that the UI state didn't change (still on empty state)
        QCOMPARE(window.m_stackedWidget->currentIndex(), 0); 
        QCOMPARE(window.m_stackedWidget->currentWidget(), window.m_emptyStateWidget);
        QCOMPARE(window.windowTitle(), QString("Hex Editor"));
        
        // Ensure actions are still disabled
        QVERIFY(!window.m_saveAction->isEnabled());
        QVERIFY(!window.m_findAction->isEnabled());
    }
};

QTEST_MAIN(MainWindowTest)
#include "tst_mainwindow.moc"