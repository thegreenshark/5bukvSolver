//#include <QtPlugin>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //инициализация цветов
    greyPalette = QPalette();
    whitePalette = QPalette();
    yellowPalette = QPalette();
    greyPalette.setColor(QPalette::Window, QColor(125, 125, 125));
    greyPalette.setColor(QPalette::WindowText, QColor(255, 255, 255));
    whitePalette.setColor(QPalette::Window, QColor(255, 255, 255));
    whitePalette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    yellowPalette.setColor(QPalette::Window, QColor(250, 210, 33));

    paletteSequence[0] = greyPalette;
    paletteSequence[1] = whitePalette;
    paletteSequence[2] = yellowPalette;

    for (int i = 0; i < WORD_LENGTH; i++) {
        currentWordCharsInfo[i].palette = paletteSequence[currentWordCharsInfo[i].colorIndex];
        currentWordCharsInfo[i].paletteSequence = paletteSequence;
    }

    //пробуем открыть файл
    std::ifstream wordsFile(WORDS_LIST_FILE);
    if (!wordsFile.is_open()) {
        initFailed = 1;
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл списка слов");
        return;
    }

    //считываем слова
    std::string line;
    while (std::getline(wordsFile, line)) {
        QString qStringLine = QString::fromStdString(line);
        if (qStringLine.length() == WORD_LENGTH)
            fittingWords.append(qStringLine);
    }
    wordsFile.close();

    if (fittingWords.size() == 0) {
        initFailed = 1;
        QMessageBox::critical(this, "Ошибка", "Список слов пуст");
        return;
    }


    setUpdatesEnabled(0);
    ui->setupUi(this);

    //записываем куда надо указатели на виджеты интерфейса
    currentWordCharsInfo[0].label = ui->labelChar0;
    currentWordCharsInfo[1].label = ui->labelChar1;
    currentWordCharsInfo[2].label = ui->labelChar2;
    currentWordCharsInfo[3].label = ui->labelChar3;
    currentWordCharsInfo[4].label = ui->labelChar4;

    colorsInputWidgets.push_back(ui->labelChar0);
    colorsInputWidgets.push_back(ui->labelChar1);
    colorsInputWidgets.push_back(ui->labelChar2);
    colorsInputWidgets.push_back(ui->labelChar3);
    colorsInputWidgets.push_back(ui->labelChar4);

    colorsInputWidgets.push_back(ui->pushButtonChar0Up);
    colorsInputWidgets.push_back(ui->pushButtonChar1Up);
    colorsInputWidgets.push_back(ui->pushButtonChar2Up);
    colorsInputWidgets.push_back(ui->pushButtonChar3Up);
    colorsInputWidgets.push_back(ui->pushButtonChar4Up);

    colorsInputWidgets.push_back(ui->pushButtonChar0Down);
    colorsInputWidgets.push_back(ui->pushButtonChar1Down);
    colorsInputWidgets.push_back(ui->pushButtonChar2Down);
    colorsInputWidgets.push_back(ui->pushButtonChar3Down);
    colorsInputWidgets.push_back(ui->pushButtonChar4Down);

    colorsInputWidgets.push_back(ui->pushButtonGoToWordList);
    colorsInputWidgets.push_back(ui->labelColorInputHint);

    wordListWidgets.push_back(ui->wordListWidget);
    wordListWidgets.push_back(ui->pushButtonWordListCancel);
    wordListWidgets.push_back(ui->pushButtonConfirmWord);
    wordListWidgets.push_back(ui->labelConfirmHint);


    writeWordToCharsInfo(defaultWord);
    updateLabels();
    ui->labelColorInputHint->setText(firstWordHint);
    onOffWidgets(wordListWidgets, 0);

    setUpdatesEnabled(1);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonChar0Up_clicked()
{
    currentWordCharsInfo[0].increaseColorIndex();
    currentWordCharsInfo[0].upateLabel();
}

void MainWindow::on_pushButtonChar1Up_clicked()
{
    currentWordCharsInfo[1].increaseColorIndex();
    currentWordCharsInfo[1].upateLabel();
}

void MainWindow::on_pushButtonChar2Up_clicked()
{
    currentWordCharsInfo[2].increaseColorIndex();
    currentWordCharsInfo[2].upateLabel();
}

void MainWindow::on_pushButtonChar3Up_clicked()
{
    currentWordCharsInfo[3].increaseColorIndex();
    currentWordCharsInfo[3].upateLabel();
}

void MainWindow::on_pushButtonChar4Up_clicked()
{
    currentWordCharsInfo[4].increaseColorIndex();
    currentWordCharsInfo[4].upateLabel();
}



void MainWindow::on_pushButtonChar0Down_clicked()
{
    currentWordCharsInfo[0].decreaseColorIndex();
    currentWordCharsInfo[0].upateLabel();
}

void MainWindow::on_pushButtonChar1Down_clicked()
{
    currentWordCharsInfo[1].decreaseColorIndex();
    currentWordCharsInfo[1].upateLabel();
}

void MainWindow::on_pushButtonChar2Down_clicked()
{
    currentWordCharsInfo[2].decreaseColorIndex();
    currentWordCharsInfo[2].upateLabel();
}

void MainWindow::on_pushButtonChar3Down_clicked()
{
    currentWordCharsInfo[3].decreaseColorIndex();
    currentWordCharsInfo[3].upateLabel();
}

void MainWindow::on_pushButtonChar4Down_clicked()
{
    currentWordCharsInfo[4].decreaseColorIndex();
    currentWordCharsInfo[4].upateLabel();
}



//переключение на список слов
void MainWindow::on_pushButtonGoToWordList_clicked()
{
    //если все буквы отгаданы
    bool allYellow = 1;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (currentWordCharsInfo[i].colorIndex != colorIndexMeanings::CORRECT){
            allYellow = 0;
            break;
        }
    }
    if (allYellow){
         QMessageBox::information(this, " ", "Слово отгадано!");
        qApp->exit();
    }



    fittingWordsNew = fittingWords;
    uint32_t wordIndex = 0;

    //оставляем в fittingWordsNew только подходящие слова
    while (wordIndex < fittingWordsNew.size()) {
        QString wordFromList = fittingWordsNew[wordIndex];
        bool deleteWord = 0;

        for (int charIndex = 0; charIndex < WORD_LENGTH; charIndex++) {


            if (currentWordCharsInfo[charIndex].colorIndex == colorIndexMeanings::NOT_IN_WORD) {
                //если в введенном слове две одинаковых буквы, и одна из них угадана(стала желтой), то вторая станет серой
                //в таком случае эта серая буква игнорируется
                bool ignore = 0;
                for (int charIndex2 = 0; charIndex2 < WORD_LENGTH; charIndex2++) {
                    if (currentWordCharsInfo[charIndex].character == currentWordCharsInfo[charIndex2].character && currentWordCharsInfo[charIndex2].colorIndex == colorIndexMeanings::CORRECT) {
                        ignore = 1;
                        break;
                    }
                }

                if (!ignore && wordFromList.contains(currentWordCharsInfo[charIndex].character, Qt::CaseSensitivity::CaseInsensitive)) {
                    deleteWord = 1;
                }
            }


            else if (currentWordCharsInfo[charIndex].colorIndex == colorIndexMeanings::AT_WRONG_POSITION) {
                if (wordFromList.at(charIndex) == currentWordCharsInfo[charIndex].character || !wordFromList.contains(currentWordCharsInfo[charIndex].character, Qt::CaseSensitivity::CaseInsensitive)) {
                    deleteWord = 1;
                }
            }


            else if (currentWordCharsInfo[charIndex].colorIndex == colorIndexMeanings::CORRECT) {
                if (wordFromList.at(charIndex) != currentWordCharsInfo[charIndex].character) {
                    deleteWord = 1;
                }
            }
        }


        if (deleteWord)
            fittingWordsNew.erase(fittingWordsNew.begin() + wordIndex);

        else
            wordIndex++;
    }

    if (fittingWordsNew.size() == 0) {
        QMessageBox::warning(this, "Ошибка поиска", "Подходящие слова не найдены");
    }
    else{
        setUpdatesEnabled(0);
        onOffWidgets(colorsInputWidgets, 0);
        onOffWidgets(wordListWidgets, 1);
        ui->pushButtonConfirmWord->setEnabled(0);

        ui->wordListWidget->clear();
        ui->wordListWidget->addItems(fittingWordsNew);
        ui->wordListWidget->update();

        setUpdatesEnabled(1);

        inWordListMode = 1;
    }
}


//переключение на выбор цветов
void MainWindow::on_pushButtonWordListCancel_clicked()
{
    setUpdatesEnabled(0);
    ui->wordListWidget->clearSelection();
    onOffWidgets(wordListWidgets, 0);
    onOffWidgets(colorsInputWidgets, 1);
    setUpdatesEnabled(1);

    fittingWordsNew.clear();
    inWordListMode = 0;
}


//выбрано новое слово
void MainWindow::on_pushButtonConfirmWord_clicked()
{
    fittingWords = fittingWordsNew;

    QString newWord = ui->wordListWidget->selectedItems().at(0)->text();

    for (int i = 0; i < WORD_LENGTH; i++) {
        currentWordCharsInfo[i].character = newWord.at(i);
        currentWordCharsInfo[i].setColorIndex(0);
        currentWordCharsInfo[i].upateLabel();
    }

    tryNumber++;

    if (tryNumber <= MAX_TRY_NUBMER) {
        QString tryNumberString = "[ Попытка ";
        tryNumberString += QString::number(tryNumber);
        tryNumberString += " из ";
        tryNumberString += QString::number(MAX_TRY_NUBMER);
        tryNumberString += " ]";
        ui->labelTryNumber->setText(tryNumberString);
    }
    else{
        ui->labelTryNumber->setText("[ Попытки кончились ]");
    }


    ui->labelColorInputHint->setText(regularHint);
    on_pushButtonWordListCancel_clicked();


    //израсходована 6-я попытка
    if (tryNumber > MAX_TRY_NUBMER) {
        QMessageBox questionMessageBox = QMessageBox(this);
        questionMessageBox.setText("Получилось угадать слово?");
        questionMessageBox.setWindowTitle(" ");

        QAbstractButton *yesButton = questionMessageBox.addButton("Да", QMessageBox::YesRole);
        QAbstractButton *noButton = questionMessageBox.addButton("Нет", QMessageBox::NoRole);

        questionMessageBox.exec();

        if (questionMessageBox.clickedButton() == yesButton)
            QMessageBox::information(this, " ", "Круто!");
        else if (questionMessageBox.clickedButton() == noButton)
            QMessageBox::information(this, " ", "Мне очень жаль...");

        qApp->exit();
    }

}


void MainWindow::on_wordListWidget_itemSelectionChanged()
{
    ui->pushButtonConfirmWord->setEnabled(1);
}
