#include "dateTimeWidget.h"
#include <QDateTime>
#include <QLabel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QDialog>

DateTimeWidget::DateTimeWidget(QWidget *parent) : QWidget(parent)
{

    dateTimeLineEdit = new QLineEdit(this);
    calendarButton = new QPushButton("v", this);
    dateTimeLineEdit->setInputMask("00:00:0000 00:00:00");

    selectedDate = QDate::currentDate();
    selectedTime = QTime::currentTime();

    QString defaultDateTimeString = selectedDate.toString("dd:MM:yyyy") + " " + selectedTime.toString("HH:mm:ss");
    dateTimeLineEdit->setText(defaultDateTimeString);
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(dateTimeLineEdit);
    inputLayout->addWidget(calendarButton);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(inputLayout);

    setLayout(mainLayout);

    selectedDate = QDate::currentDate();
    selectedTime = QTime::currentTime();

    connect(calendarButton, &QPushButton::clicked, this, &DateTimeWidget::showCalendar);
    connect(dateTimeLineEdit, &QLineEdit::editingFinished, this, &DateTimeWidget::validateAndUpdateDateTime);
}

void DateTimeWidget::showCalendar()
{
    QDialog *calendarDialog = new QDialog(this, Qt::Popup);
    calendarDialog->setFixedSize(300, 300);

    QVBoxLayout *dialogLayout = new QVBoxLayout(calendarDialog);
    QCalendarWidget *calendarWidget = new QCalendarWidget(calendarDialog);
    dialogLayout->addWidget(calendarWidget);
    calendarDialog->setLayout(dialogLayout);

    QPoint buttonPos = calendarButton->mapToGlobal(QPoint(0, 0));
    calendarDialog->move(buttonPos.x() + calendarButton->width(), buttonPos.y());

    connect(calendarWidget, &QCalendarWidget::clicked, [this, calendarDialog, calendarWidget](const QDate &date) {
        selectedDate = date;
        updateDateTimeLineEdit();
        calendarDialog->accept();
    });

    calendarDialog->exec();
}

void DateTimeWidget::updateDateTimeLineEdit()
{
    QString dateTimeString = selectedDate.toString("dd:MM:yyyy") + " " + selectedTime.toString("HH:mm:ss");
    dateTimeLineEdit->setText(dateTimeString);
}

void DateTimeWidget::validateAndUpdateDateTime()
{
    QString dateTimeString = dateTimeLineEdit->text();
    QRegularExpression re("^(\\d{2}):(\\d{2}):(\\d{4}) (\\d{2}):(\\d{2}):(\\d{2})$");
    QRegularExpressionMatch match = re.match(dateTimeString);

    if (match.hasMatch())
    {
        int day = match.captured(1).toInt();
        int month = match.captured(2).toInt();
        int year = match.captured(3).toInt();
        int hour = match.captured(4).toInt();
        int minute = match.captured(5).toInt();
        int second = match.captured(6).toInt();

        if (QDate::isValid(year, month, day) && hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && second >= 0 && second <= 59)
        {
            selectedDate.setDate(year, month, day);
            selectedTime.setHMS(hour, minute, second);
        }
        else
        {
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid date and time.");
            updateDateTimeLineEdit();
        }
    }
    else
    {
        QMessageBox::warning(this, "Invalid Input", "Please enter the date and time in the format DD:MM:YYYY HH:MM:SS.");
        updateDateTimeLineEdit();
    }
}
