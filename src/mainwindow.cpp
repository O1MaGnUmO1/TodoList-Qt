#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QColor>
#include <QStandardItemModel>
#include <QItemDelegate>

CustomDelegate::CustomDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

// For painting only 4-rth column of tableView
void CustomDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.isValid()) {
        int lastColumn = index.model()->columnCount() - 1;

        if (index.column() == lastColumn) {
            QString text = index.data(Qt::DisplayRole).toString();

            if (text == "Done") {
                painter->fillRect(option.rect, qRgb(122,241,120));
            } else {
                painter->fillRect(option.rect, qRgb(230,80,98));
            }
        } else {
            painter->fillRect(option.rect, option.backgroundBrush);
        }
        QStyledItemDelegate::paint(painter, option, index);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->setStyleSheet("background-color:darkgrey;");
    CustomDelegate * customDelegate = new CustomDelegate(ui->tableView);
    ui->tableView->setItemDelegate(customDelegate);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("todolistDB");
    if (db.open()){
        qDebug("Open");
    }
    else {
        qDebug("not open");
    }
    model = new QSqlTableModel(this, db);
    query = new QSqlQuery(db);
    query->exec("CREATE TABLE todolist(Name TEXT, Description TEXT, Date DATE, Status TEXT)");
    model->setTable("todolist");
    model->select();
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setVisible(false);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setVisible(true);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete query;
    delete model;
    delete insertionDialog;
    delete nameLineEdit;
    delete descriptionLineEdit;
    delete dateEdit;
    delete stateComboBox;
    delete updateDialog;
    delete updateName;
    delete updateDescription;
    delete updateDate;
    delete currentState;
    delete dateDialog;
    delete startDateEdit;
    delete endDateEdit;
}


// For add task tool button
void MainWindow::on_actionAdd_Task_triggered()
{
    showInsertionDialog();
}

// For edit task tool button
void MainWindow::on_actionEdit_Task_triggered()
{
    showUpdateDialog();
}

// For delete task tool button
void MainWindow::on_actionDelete_Task_triggered()
{
    int selectedRow = ui->tableView->currentIndex().row();
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Confirm Deletion", "Are you sure you want to delete this row?", QMessageBox::Yes | QMessageBox::No);
    if( confirmation == QMessageBox::Yes){
        model->removeRow(selectedRow);
        model->select();
    }
}

// Functionality of showing insertion dialog when we want to add task
void MainWindow::showInsertionDialog() {
    insertionDialog = new QDialog(this);
    insertionDialog->setWindowTitle("Insert New Task");
    insertionDialog->setMinimumWidth(450);
    nameLineEdit = new QLineEdit(insertionDialog);
    descriptionLineEdit = new QLineEdit(insertionDialog);

    dateEdit = new QDateEdit(QDate::currentDate(), insertionDialog);
    dateEdit->setDisplayFormat(dateFormat);
    stateComboBox = new QComboBox(insertionDialog);
    stateComboBox->addItem("In Progress");
    stateComboBox->addItem("Done");
    insertionDialog->show();

    QPushButton* submitButton = new QPushButton("Submit", insertionDialog);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::insertTask);

    QVBoxLayout* dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(new QLabel("Name", insertionDialog));
    dialogLayout->addWidget(nameLineEdit);
    dialogLayout->addWidget(new QLabel("Description", insertionDialog));
    dialogLayout->addWidget(descriptionLineEdit);
    dialogLayout->addWidget(new QLabel("Date", insertionDialog));
    dialogLayout->addWidget(dateEdit);
    dialogLayout->addWidget(new QLabel("Status", insertionDialog));
    dialogLayout->addWidget(stateComboBox);
    dialogLayout->addWidget(submitButton);

    insertionDialog->setLayout(dialogLayout);
    insertionDialog->exec();
    delete submitButton;
    delete dialogLayout;
    submitButton = nullptr;
    dialogLayout = nullptr;
}

// Functionality of showing update dialog when we want to update task
void MainWindow::showUpdateDialog()
{
    auto current = ui->tableView->currentIndex().model();
    int row = ui->tableView->currentIndex().row();
    updateDialog = new QDialog(this);
    updateDialog->setWindowTitle("Update Selected Task");
    updateDialog->setMinimumWidth(450);

    QPushButton* submitButton = new QPushButton("Submit", updateDialog);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::updateTask);

    updateName = new QLineEdit(updateDialog);
    updateName->setText(current->index(row,0).data().toString());
    updateDescription = new QLineEdit(updateDialog);
    updateDescription->setText(current->index(row,1).data().toString());
    updateDate = new QDateEdit(QDate::currentDate(), updateDialog);
    updateDate->setDate(current->index(row, 2).data(Qt::UserRole).toDate());
    updateDate->setDisplayFormat(dateFormat);

    QVBoxLayout* dialogLayout = new QVBoxLayout;
    dialogLayout->addWidget(new QLabel("Name", updateDialog));
    dialogLayout->addWidget(updateName);
    dialogLayout->addWidget(new QLabel("Description", updateDialog));
    dialogLayout->addWidget(updateDescription);
    dialogLayout->addWidget(new QLabel("Date", updateDialog));
    dialogLayout->addWidget(updateDate);
    dialogLayout->addWidget(submitButton);
    updateDialog->setLayout(dialogLayout);
    updateDialog->exec();

    delete submitButton;
    delete dialogLayout;
    submitButton = nullptr;
    dialogLayout = nullptr;
}

// insert into table after
void MainWindow::insertTask()
{
    QString taskName = nameLineEdit->text();
    QString taskDescription = descriptionLineEdit->text();
    QDate taskDate = dateEdit->date();

    int row = model->rowCount();
    model->insertRow(model->rowCount());
    model->setData(model->index(row, 0), taskName);
    model->setData(model->index(row, 1), taskDescription);
    model->setData(model->index(row, 2), taskDate.toString(dateFormat));
    model->setData(model->index(row, 3), stateComboBox->currentText());
    if (model->submitAll()){
        model->select();
    }
    else{
        QMessageBox::critical(this, "Error", "Failed to insert row.");
    }
    insertionDialog->close();

}

// update table
void MainWindow::updateTask()
{
    QString taskName = updateName->text();
    QString description = updateDescription->text();
    QDate taskDate = updateDate->date();

    int currentRow = ui->tableView->currentIndex().row();
    model->setData(model->index(currentRow, 0), taskName);
    model->setData(model->index(currentRow, 1), description);
    model->setData(model->index(currentRow, 2), taskDate.toString(dateFormat));
    if (model->submitAll()){
        model->select();
    }
    else{
        QMessageBox::critical(this, "Error", "Failed to insert row.");
    }

    updateDialog->close();
}

// filter by name functionality
void MainWindow::on_NamePushButton_clicked()
{
    model->setSort(model->fieldIndex("Name"), Qt::AscendingOrder);
    model->select();
}

// filter by description funcionality
void MainWindow::on_DescriptionPushButton_clicked()
{
    model->setSort(model->fieldIndex("Description"), Qt::AscendingOrder);

    // Apply the filter
    model->select();
}

// filter by date
void MainWindow::on_DatePushButton_clicked()
{
    showFilterByDateDialog();
}


// show filtering by date dialog
void MainWindow::showFilterByDateDialog(){
    dateDialog = new QDialog(this);
    dateDialog->setWindowTitle("Filter by Date");
    dateDialog->setMinimumWidth(450);

    QPushButton* submitButton = new QPushButton("Submit", dateDialog);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::filterByDate);

    startDateEdit = new QDateEdit(QDate::currentDate(), dateDialog);
    endDateEdit = new QDateEdit(QDate::currentDate(), dateDialog);
    startDateEdit->setDate(QDate::currentDate());
    endDateEdit->setDate(QDate::currentDate());
    startDateEdit->setDisplayFormat(dateFormat);
    endDateEdit->setDisplayFormat(dateFormat);

    QVBoxLayout* dialogLayout = new QVBoxLayout;
    dialogLayout->addWidget(new QLabel("From", dateDialog));
    dialogLayout->addWidget(startDateEdit);
    dialogLayout->addWidget(new QLabel("To", dateDialog));
    dialogLayout->addWidget(endDateEdit);
    dialogLayout->addWidget(submitButton);
    dateDialog->setLayout(dialogLayout);

    dateDialog->exec();
    delete submitButton;
    delete dialogLayout;
    submitButton = nullptr;
    dialogLayout = nullptr;
}

// filter by date functionality
void MainWindow::filterByDate()
{
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    QString filter = QString("Date >= '%1' AND Date <= '%2'")
                         .arg(startDate.toString(dateFormat))
                         .arg(endDate.toString(dateFormat));
    model->setFilter(filter);
    model->select();
    dateDialog->close();
}

// filter by done tasks
void MainWindow::on_DoneTasksPushButton_clicked()
{
    model->setFilter("Status = 'Done'");
    model->select();
}

// filter by in progress tasks
void MainWindow::on_InProgressPushButton_clicked()
{
    model->setFilter("Status = 'In Progress'");
    model->select();
}

// reset filters
void MainWindow::on_ResetPushButton_clicked()
{
    model->setFilter(""); // Clear the filter
    model->select();
}

// mark as done functionality
void MainWindow::on_DonePushButton_clicked()
{
    int selectedRow = ui->tableView->currentIndex().row();
    QMessageBox::StandardButton confirmation;

    confirmation = QMessageBox::question(this, "Confirm Marking", "Are you sure you want to mark as Done?", QMessageBox::Yes | QMessageBox::No);
    if( confirmation == QMessageBox::Yes){
        model->setData(model->index(selectedRow, 3), "Done");
        model->submitAll();
        model->select();
    }
}

// mark as in progress functionality
void MainWindow::on_MarkInProgressPushButton_clicked()
{
    int selectedRow = ui->tableView->currentIndex().row();
    QMessageBox::StandardButton confirmation;

    confirmation = QMessageBox::question(this, "Confirm Marking", "Are you sure you want to mark as In Progress?", QMessageBox::Yes | QMessageBox::No);

    if( confirmation == QMessageBox::Yes){
        model->setData(model->index(selectedRow, 3), "In Progress");
        model->submitAll();
        model->select();
    }
}

