#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <qtablewidget.h>
#include <QFileDialog>
#include <QMessageBox>
#include "Turing/machine.h"
#include "Turing/sampledata.h"
#include "turingfile.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_executeOneStepBtn_clicked();

    void on_executeAllBtn_clicked();

    void on_resetButton_clicked();

    void onSaveCalled();

    void onLoadCalled();

    void on_funcMatrixTable_cellChanged(int row, int column);

    void on_defaultButton_clicked();

    void Exit();

    void on_actionAbout_triggered();

    void on_tapeTable_cellChanged(int row, int column);

private:
    Ui::MainWindow *ui;
    void SetupMachine();
    void RefreshFuncMatrix();
    void RefreshTape();
    void RefreshStatusBar();
    Machine turingMachine;
    QTableWidgetItem* prevInstrCell;
    std::vector<char> symbols;
};
#endif // MAINWINDOW_H
