#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    prevInstrCell = nullptr;
    turingMachine = Machine(SampleData::GetSampleProgram(), SampleData::GetSampleTape());
    ui->setupUi(this);

    this->SetupMachine();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupMachine() {
    // Configure the functional matrix table's Headers
    ui->funcMatrixTable->setRowCount(3);
    ui->funcMatrixTable->setColumnCount(4);

    //Configure the tape
    ui->tapeTable->setRowCount(1);
    ui->tapeTable->setColumnCount(Machine::TAPE_LEN);

    this->RefreshFuncMatrix();
    this->RefreshTape();
}

void MainWindow::RefreshFuncMatrix() {
    prevInstrCell = nullptr;
    symbols.clear();
    ui->funcMatrixTable->blockSignals(true);
    QStringList stateHeaders;
    for (int i = 0; i < 4; ++i) {
        stateHeaders << QString("q%1").arg(i);
    }

    QStringList charHeaders;
    int curr_row = 0;
    for (const auto& [symbol, instrArray] : turingMachine.functionalMatrix) {
        symbols.push_back(symbol);
        charHeaders << QString(symbol);

        for(int curr_column = 0; curr_column < Machine::NUM_STATES; curr_column++) {
            const auto& opt_instr = instrArray[curr_column];
            std::string cellText = "";

            // Get the existing QTableWidgetItem* at the current coordinates.
            QTableWidgetItem* cellItem = ui->funcMatrixTable->item(curr_row, curr_column);

            // If it's NULL, we'll initialize a new one
            if(cellItem == NULL) {
                cellItem = new QTableWidgetItem();
                cellItem->setTextAlignment(Qt::AlignCenter);
                ui->funcMatrixTable->setItem(curr_row, curr_column, cellItem);
            }

            // Check whether an Instruction is actually set at the current coordinates in the
            // functional matrix, before writing it to the cell
            if(opt_instr.has_value()) {
                const Instruction& instr = opt_instr.value();
                cellText = instr.to_string();

            }
            cellItem->setText(QString::fromStdString(cellText));

        }
        curr_row++;
    }

    ui->funcMatrixTable->setHorizontalHeaderLabels(stateHeaders);
    ui->funcMatrixTable->setVerticalHeaderLabels(charHeaders);
    ui->funcMatrixTable->blockSignals(false);
}

void MainWindow::RefreshTape() {
    ui->funcMatrixTable->blockSignals(true);
    ui->tapeTable->blockSignals(true);

    const auto& tape = turingMachine.getTape();
    for(int tapeIndex = 0; tapeIndex < tape.size(); tapeIndex++) {
        QString cellText = QString(tape[tapeIndex]);

        QTableWidgetItem* tapeItem = ui->tapeTable->item(0, tapeIndex);
        bool do_insert = false;

        if(tapeItem == nullptr) {
            tapeItem = new QTableWidgetItem();
            tapeItem->setTextAlignment(Qt::AlignCenter);
            do_insert = true;
        }

        tapeItem->setText(cellText);

        if(turingMachine.currentCell == tapeIndex) {
            tapeItem->setBackground(Qt::yellow);
        } else {
            tapeItem->setBackground(Qt::white);
        }

        if(do_insert) {
            ui->tapeTable->setItem(0, tapeIndex, tapeItem);
            ui->tapeTable->setColumnWidth(tapeIndex, 20);
        }

    }

    std::string state_str = "Current state: ";
    if(turingMachine.currentState == -1) {
        state_str += "HALT";
    } else {
        state_str += "q" + std::to_string(turingMachine.currentState);
    }
    QString state_text = QString::fromStdString(state_str);
    ui->stateLabel->setText(state_text);

    if(prevInstrCell != nullptr) {
        prevInstrCell->setBackground(Qt::white);
    }

    int row = std::find(symbols.begin(), symbols.end(), turingMachine.GetCurrentChar()) - symbols.begin();
    int column = turingMachine.currentState;

    if(row >= ui->funcMatrixTable->rowCount() || column >= ui->funcMatrixTable->columnCount()) {
        // Invalid cell
        return;
    }

    QTableWidgetItem* currentCell = ui->funcMatrixTable->item(row, column);
    if(currentCell != nullptr) {
        currentCell->setBackground(Qt::yellow);
    }
    prevInstrCell = currentCell;

    // Can only edit the tape when the machine isn't running!
    ui->tapeTable->setEnabled(turingMachine.GetFlag() == Flag::NOT_RUNNING);

    ui->tapeTable->clearSelection();

    this->RefreshStatusBar();
    ui->funcMatrixTable->blockSignals(false);
    ui->tapeTable->blockSignals(false);
}

void MainWindow::on_executeOneStepBtn_clicked()
{
    turingMachine.ExecuteSingle();
    RefreshTape();
}


void MainWindow::on_executeAllBtn_clicked()
{
    turingMachine.Execute();
    RefreshTape();
}

void MainWindow::on_resetButton_clicked()
{
    turingMachine.Reset();
    RefreshTape();
}

void MainWindow::RefreshStatusBar() {
    Flag machine_flag = turingMachine.GetFlag();
    switch(machine_flag) {
    case Flag::NOT_RUNNING:
        ui->statusBar->showMessage("Machine Not Running");
        break;
    case Flag::RUNNING:
        ui->statusBar->showMessage("Machine Running");
        break;
    case Flag::HALTED:
        ui->statusBar->showMessage("Machine Halted");
        break;
    case Flag::TOOMANYINSTR:
        ui->statusBar->showMessage("Machine killed due to infinite loop");
        break;
    case Flag::INVALID_INSTR:
        ui->statusBar->showMessage("Machine killed due to invalid instruction");
        break;
    }
}


void MainWindow::onSaveCalled()
{
    QString save_filename = QFileDialog::getSaveFileName(this, "Save Turing Machine", "", "TuringEmu files (*.tmu);;All Files (*)");

    if(save_filename.isEmpty()) {
        return;
    }

    std::string filePath = save_filename.toStdString();
    try {
        TuringFile::Save(turingMachine, filePath);
        QMessageBox::information(this, "Success", "Machine saved successfully!");
    } catch(const std::runtime_error& ex) {
        std::string error_msg = std::string("An error occurred while saving the machine: ") + ex.what();
        QMessageBox::critical(this, "Error", QString::fromStdString(error_msg));
    }

}


void MainWindow::onLoadCalled()
{
    QString load_filename = QFileDialog::getOpenFileName(this, "Load Turing Machine", "", "TuringEmu files (*.tmu);;All Files (*)");

    if(load_filename.isEmpty()) {
        return;
    }

    std::string filePath = load_filename.toStdString();
    try {
        turingMachine = TuringFile::Load(filePath);
        QMessageBox::information(this, "Success", "Machine loaded successfully!");
        this->RefreshFuncMatrix();
        this->RefreshTape();

    } catch(const std::runtime_error& ex) {
        std::string error_msg = std::string("An error occurred while loading the machine: ") + ex.what();
        QMessageBox::critical(this, "Error", QString::fromStdString(error_msg));
    }
}


void MainWindow::on_funcMatrixTable_cellChanged(int row, int column)
{
    auto edited_cell = ui->funcMatrixTable->item(row, column);
    char row_char = ui->funcMatrixTable->verticalHeaderItem(row)->text().toStdString()[0];

    std::string new_instr = edited_cell->text().toStdString();
    try {
        turingMachine.setInstr_fromString(new_instr, row_char, column);
    } catch(const std::runtime_error& ex) {
        QMessageBox::critical(this, "Error", ex.what());

        // Revert the value in the edited cell
        std::string instr_text = "";
        auto curr_instr = &turingMachine.functionalMatrix[row_char][column];

        if(curr_instr->has_value()) {
            instr_text = curr_instr->value().to_string();
        }

        ui->funcMatrixTable->blockSignals(true);
        edited_cell->setText(QString::fromStdString(instr_text));
        ui->funcMatrixTable->blockSignals(false);
    }
}

void MainWindow::on_tapeTable_cellChanged(int row, int column)
{
    std::string new_value = ui->tapeTable->item(row, column)->text().toStdString();

    // Always trim the newly inserted value so that it's always just one character
    new_value = new_value.substr(0, 1);

    ui->tapeTable->item(row, column)->setText(QString::fromStdString(new_value));
    turingMachine.changeTapeValue(new_value[0], column);
}


void MainWindow::on_defaultButton_clicked()
{
    QMessageBox::StandardButton question_box = QMessageBox::question(this, "Revert to default machine", "Are you sure you want to revert the machine to its default configuration? Any unsaved changes will be lost.");
    if(question_box == QMessageBox::Yes) {
        turingMachine = Machine(SampleData::GetSampleProgram(), SampleData::GetSampleTape());
        this->RefreshFuncMatrix();
        this->RefreshTape();
    }
}

void MainWindow::Exit() {
    qApp->exit(0);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::StandardButton info_box = QMessageBox::information(this, "About", "Made with <3 by obbyrobby");
}




