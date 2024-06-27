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
#include <QDateTimeEdit>>
class DateTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeWidget(QWidget *parent = nullptr);

private slots:
    void showCalendar();
    void updateDateTimeLineEdit();
    void validateAndUpdateDateTime();

private:
    QLineEdit *dateTimeLineEdit;
    QPushButton *calendarButton;
    QVBoxLayout *mainLayout;
    QDate selectedDate;
    QTime selectedTime;
};

#endif // DATETIMEWIDGET_H
