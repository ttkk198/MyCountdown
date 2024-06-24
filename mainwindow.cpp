#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QRandomGenerator>
#include <QtMultimedia/QSoundEffect>
#include <QPropertyAnimation>
#include <QRegularExpression>
#include <cmath>
#include <QMessageBox>
#include <QTextToSpeech>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentIndex(0)
    , currentWordIndex(0)
    , rhymeRunning(false)
    , highlightedLabel(nullptr)
{
    ui->setupUi(this);
    setFixedSize(800, 600);
    QRandomGenerator::securelySeeded();
    longestWordLabel = ui->longestWordLabel;
    currentRhymeWordLabel = new QLabel(this);
    currentRhymeWordLabel->setAlignment(Qt::AlignCenter);
    currentRhymeWordLabel->setFont(QFont("Arial", 15));
    currentRhymeWordLabel->move(width() / 2 - 50, height() / 2 + 30);
    currentRhymeWordLabel->setFixedWidth(100);
    currentRhymeWordLabel->hide();

    nextWordButton = ui->nextWordButton;
    connect(nextWordButton, &QPushButton::clicked, this, &MainWindow::onNextWordButtonClicked);

    QVector<QString> rhymes = loadRhymes(":/texts/rhymes.txt");
    persons = loadPhotos(QDir::currentPath() + "/photos");

    if (persons.isEmpty() || rhymes.isEmpty()) {
        QMessageBox::critical(this, "Critical Error", "No photos or rhymes found. The application will now exit.");
        QTimer::singleShot(0, this, &QCoreApplication::quit);
        return;
    }

    displayPhotosInCircle(persons);

    QString rhyme = selectRandomRhyme(rhymes);

    rhymeWords = rhyme.split(QRegularExpression("\\s+"));
    longestWordLabel->setText(findLongestWord(rhyme));
    longestWordLabel->show();
    textToSpeech = new QTextToSpeech(this);
    textToSpeech->setVolume(0.8);
    rhymeTimer = new QTimer(this);
    connect(rhymeTimer, &QTimer::timeout, this, &MainWindow::updateRhymeWord);
}

MainWindow::~MainWindow() {
    delete ui;
    delete rhymeTimer;
    delete textToSpeech;
}

QVector<QString> MainWindow::loadRhymes(const QString &filePath) {
    QVector<QString> rhymes;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            line.remove(QRegularExpression("[[:punct:]]"));
            rhymes.append(line);
        }
        file.close();
    }
    return rhymes;
}

QList<Person> MainWindow::loadPhotos(const QString &folderPath) {
    QList<Person> persons;
    QDir dir(folderPath);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg";
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo &fileInfo : fileList) {
        QPixmap pixmap(fileInfo.absoluteFilePath());
        persons.append(Person(fileInfo.baseName(), pixmap));
    }
    return persons;
}

void MainWindow::displayPhotosInCircle(const QList<Person> &persons) {
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = qMin(centerX, centerY) - 100;

    int count = persons.size();
    for (int i = 0; i < count; ++i) {
        double angle = (2 * M_PI / count) * i;
        int x = centerX + radius * cos(angle) - 75;
        int y = centerY + radius * sin(angle) - 75;

        QLabel *label = new QLabel(this);
        label->setPixmap(persons[i].photo.scaled(150, 150, Qt::KeepAspectRatio));
        label->setFixedSize(150, 150);
        label->move(x, y);
        label->setAlignment(Qt::AlignCenter);
        label->setObjectName(persons[i].name);
        label->show();

        QLabel *nameLabel = new QLabel(persons[i].name, this);
        nameLabel->setFont(QFont("Arial", 10, QFont::Bold));
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setGeometry(x, y + 150, 150, 20);
        nameLabel->setObjectName(persons[i].name + "_name");
        nameLabel->show();
    }
}


QString MainWindow::selectRandomRhyme(const QVector<QString> &rhymes) {
    if (rhymes.isEmpty()) {
        return QString();
    }
    int index = QRandomGenerator::global()->bounded(rhymes.size());
    return rhymes[index];
}

QString MainWindow::findLongestWord(const QString &rhyme) {
    QStringList words = rhyme.split(QRegularExpression("\\s+"));
    QString longestWord;
    for (const QString &word : words) {
        if (word.length() > longestWord.length()) {
            longestWord = word;
        }
    }
    return longestWord;
}

void MainWindow::onNextWordButtonClicked() {
    if (persons.isEmpty() || rhymeWords.isEmpty()) {
        return;
    }

    if (!rhymeRunning) {
        rhymeRunning = true;
        currentWordIndex = 0;
        currentRhymeWordLabel->setText(rhymeWords[0]);
        currentRhymeWordLabel->show();
        highlightCurrentPerson();
        QString currentWord = rhymeWords[currentWordIndex];
        textToSpeech->say(currentWord);
        rhymeTimer->start(1500);

    }
}

void MainWindow::updateRhymeWord() {
    if (persons.isEmpty()) {
        return;
    }

    if (highlightedLabel) {
        highlightedLabel->setStyleSheet("");
    }

    currentWordIndex = (currentWordIndex + 1) % rhymeWords.size();
    currentRhymeWordLabel->setText(rhymeWords[currentWordIndex]);
    currentIndex = (currentIndex + 1) % persons.size();
    highlightCurrentPerson();
    QString currentWord = rhymeWords[currentWordIndex];
    textToSpeech->say(currentWord);
    if (currentWordIndex == rhymeWords.size() - 1) {
        rhymeTimer->stop();
        QTimer::singleShot(1000, this, [this]() {
            removeCurrentPerson();
            currentRhymeWordLabel->hide();
            rhymeRunning = false;
        });
    }
}


void MainWindow::highlightCurrentPerson() {
    highlightedLabel = findChild<QLabel *>(persons[currentIndex].name);
    if (highlightedLabel) {
        highlightedLabel->setStyleSheet("border: 2px solid red");
    }
}

void MainWindow::removeCurrentPerson() {
    QLabel *label = findChild<QLabel *>(persons[currentIndex].name);
    QLabel *nameLabel = findChild<QLabel *>(persons[currentIndex].name + "_name");
    if (label && nameLabel) {
        animateRemoval(label);
        animateRemoval(nameLabel);
        persons.removeAt(currentIndex);

        if (persons.size() == 1) {
            QLabel *winnerLabel = new QLabel(this);
            winnerLabel->setPixmap(persons[0].photo.scaled(150, 150, Qt::KeepAspectRatio));
            winnerLabel->setFixedSize(150, 150);
            winnerLabel->move(width() / 2 - 75, height() / 2 - 75);
            displayWinner(winnerLabel);
        } else if (persons.size() > 0) {
            currentIndex = currentIndex % persons.size();
        }

    }
}


void MainWindow::playSoundEffect(const QString &soundPath) {
    QSoundEffect *sound = new QSoundEffect(this);
    sound->setSource(QUrl::fromLocalFile(soundPath));
    sound->play();
}

void MainWindow::animateRemoval(QLabel *label) {
    QPropertyAnimation *animation = new QPropertyAnimation(label, "geometry");
    animation->setDuration(1000);
    animation->setStartValue(label->geometry());
    animation->setEndValue(QRect(label->x(), -label->height(), label->width(), label->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    if (persons.count() > 2) {
        playSoundEffect(":/sounds/out.wav");
    }
    else {
        for (const Person &person : persons) {
            QLabel *personLabel = findChild<QLabel *>(person.name);
            QLabel *nameLabel = findChild<QLabel *>(person.name + "_name");
            if (nameLabel) {
                nameLabel->hide();
            }
            if (personLabel) {
                personLabel->hide();
            }
        }
    }
}

void MainWindow::displayWinner(QLabel *winnerLabel) {
    QPixmap crown(":/images/crown.png");
    QLabel *crownLabel = new QLabel(this);
    crownLabel->setPixmap(crown.scaled(50, 50, Qt::KeepAspectRatio));
    crownLabel->move(winnerLabel->x() + 50, winnerLabel->y() - 25);
    crownLabel->show();
    winnerLabel->show();

    QLabel *winnerTextLabel = new QLabel(this);
    winnerTextLabel->setText("won!");
    winnerTextLabel->setFont(QFont("Arial", 25, QFont::Bold));
    winnerTextLabel->setAlignment(Qt::AlignCenter);
    winnerTextLabel->setGeometry(winnerLabel->x(), winnerLabel->y() + winnerLabel->height(), winnerLabel->width(), 50);
    winnerTextLabel->show();

    playSoundEffect(":/sounds/fireworks.wav");

}
