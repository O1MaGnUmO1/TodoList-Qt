#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QDate>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QStyledItemDelegate>


class CustomDelegate : public QStyledItemDelegate {
public:
    CustomDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void showInsertionDialog();
    void showUpdateDialog();
    void showFilterByDateDialog();
    void moveCenter();
    ~MainWindow();
private slots:
    void on_actionAdd_Task_triggered();
    void on_actionEdit_Task_triggered();
    void on_actionDelete_Task_triggered();

    void on_NamePushButton_clicked();
    void on_DescriptionPushButton_clicked();
    void on_DatePushButton_clicked();
    void on_DoneTasksPushButton_clicked();
    void on_InProgressPushButton_clicked();
    void on_ResetPushButton_clicked();
    void on_DonePushButton_clicked();
    void on_MarkInProgressPushButton_clicked();


    void insertTask();
    void updateTask();
    void filterByDate();
private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *model;
    // for insertion
    QDialog* insertionDialog;
    QLineEdit* nameLineEdit;
    QLineEdit* descriptionLineEdit;
    QDateEdit* dateEdit;
    QComboBox* stateComboBox;
    // for update
    QDialog* updateDialog;
    QLineEdit* updateName;
    QLineEdit* updateDescription;
    QDateEdit* updateDate;
    QComboBox* currentState;
    // for filtering by date
    QDialog* dateDialog;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;

    const QString dateFormat = "dd/MM/yyyy";
};
#endif // MAINWINDOW_H
