#pragma once
#include <QMainWindow>
#include "bytebuffer.h"
#include "editormodel.h"
#include "editorview.h"
#include "filemanager.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void openFile(const QString &path);

private:
    void setupTable();
    void setupMenuBar();

    /*
     * Prompts the user to save their work if there are unsaved changes.
     * Called before proceeding to a destructive operation
     * (e.g. opening a new file or exiting).
     * Returns true if it is safe to continue to the next operation,
     *   e.g. there are no changes to save, or the user decided to discard them.
     * Returns false if the next operation should be aborted, eg.
     *   the user chooses to continue editing, or the save failed.
     */
    bool promptSaveIfModified();
    /*
     * Writes the updates to the file to disk.
     */
    void saveFile();
    /*
     * Opens the file selection dialog (only if there are no current changes, or the user discards them)
     */
    void openFileDialog();
    /*
     * Sets the title of the current window based on the currently loaded file.
     */
    void updateTitle();

    // Dependencies
    ByteBuffer  m_buffer;
    EditorModel m_model;
    FileManager  m_fileManager;
    EditorView *m_tableView = nullptr;

    /*
     * Path to the currently loaded file. Empty if no file loaded.
     */
    QString      m_currentPath;
};