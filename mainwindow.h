#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include "qlistwidget.h"
#include <QMainWindow>
#include <QMessageBox>
#include <fstream>
#include <QDialogButtonBox>

#define WORD_LENGTH 5
#define AVAILABLE_COLORS 3
#define MAX_TRY_NUBMER 6
#define WORDS_LIST_FILE "russian_nouns.txt"

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

bool initFailed = 0;

private slots:
void on_pushButtonChar0Up_clicked();
void on_pushButtonChar1Up_clicked();
void on_pushButtonChar2Up_clicked();
void on_pushButtonChar3Up_clicked();
void on_pushButtonChar4Up_clicked();

void on_pushButtonChar0Down_clicked();
void on_pushButtonChar1Down_clicked();
void on_pushButtonChar2Down_clicked();
void on_pushButtonChar3Down_clicked();
void on_pushButtonChar4Down_clicked();

void on_pushButtonGoToWordList_clicked();
void on_pushButtonWordListCancel_clicked();

void on_wordListWidget_itemSelectionChanged();

void on_pushButtonConfirmWord_clicked();

private:
Ui::MainWindow *ui;

enum colorIndexMeanings {
    NOT_IN_WORD = 0,
    AT_WRONG_POSITION = 1,
    CORRECT = 2
};

QPalette greyPalette;
QPalette whitePalette;
QPalette yellowPalette;

QStringList fittingWords;
QStringList fittingWordsNew;

const QString defaultWord = "океан";
const QString firstWordHint = "Введи в игре слово \"ОКЕАН\"\n\nВыбери для каждой буквы такой же цвет, как в игре";
const QString regularHint = "Выбери для каждой буквы такой же цвет, как в игре";

bool inWordListMode = 0;

struct charInfo {
    QChar character;
    QPalette palette;
    uint8_t colorIndex;
    QPalette *paletteSequence;

    QLabel *label;

    void upateLabel(){
        label->setText(character.toUpper());
        label->setPalette(palette);
    }

    void increaseColorIndex(){
        setColorIndex(colorIndex + 1);
    }

    void decreaseColorIndex(){
        setColorIndex(colorIndex - 1);
    }

    void setColorIndex(int8_t index){
        if (index > AVAILABLE_COLORS - 1) {
            colorIndex = 0;
        }
        else if (index < 0) {
            colorIndex = AVAILABLE_COLORS - 1;
        }
        else {
            colorIndex = index;
        }

        palette = paletteSequence[colorIndex];
    }

    charInfo(QPalette palette = QPalette()) :
        palette(palette),
        character('-'),
        colorIndex(0),
        paletteSequence(nullptr),
        label(nullptr)
    {
    }
};




QPalette paletteSequence[AVAILABLE_COLORS];
charInfo currentWordCharsInfo[WORD_LENGTH];


std::vector<QWidget *> colorsInputWidgets;
std::vector<QWidget *> wordListWidgets;

uint8_t tryNumber = 1;


void writeWordToCharsInfo(QString word){
    uint16_t length = WORD_LENGTH;
    if (word.length() < length)
        length = word.length();

    for (int i = 0; i < length; i++) {
        currentWordCharsInfo[i].character = word[i];
    }

}

void updateLabels(){
    for (int i = 0; i < WORD_LENGTH; i++) {
        currentWordCharsInfo[i].upateLabel();
    }
}


void onOffWidgets(std::vector<QWidget *> widgets, bool state){
    for (int i = 0; i < widgets.size(); i++) {
        widgets[i]->setEnabled(state);
        widgets[i]->setVisible(state);
    }
}




};
#endif // MAINWINDOW_H
