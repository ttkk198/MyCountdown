#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QTime>
#include <QDialog>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QLabel>
class DateTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeWidget(QWidget *parent = nullptr);

private slots:
    void showCalendar();
    void updateDateTimeLineEdit();
    void on_dateTimeLineEdit_textEdited(const QString &arg1);
private:
    QLineEdit *dateTimeLineEdit;
    QPushButton *calendarButton;
    QVBoxLayout *mainLayout;
    QDate selectedDate;
    QTime selectedTime;
    QLabel *status;
    QTextEdit *result1;
};

#endif // DATETIMEWIDGET_H
