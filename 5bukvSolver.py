import os
import keyboard
import time
from easyansi.core import colors, cursor, screen, utils
import sys

DICTIONARY_FILE ='russian_nouns.txt' #список существительных русского языка
FIRST_WORD = 'океан' #океан, потому что в нем самые часто встречающиеся буквы
WORD_LENGTH = 5
MAX_ATTEMPTS = 6

CHAR_COLOR_GREY = colors.BRIGHT_BLACK
CHAR_COLOR_WHITE = colors.BRIGHT_WHITE
CHAR_COLOR_YELLOW = colors.YELLOW
CHAR_AVAILABLE_COLORS = [CHAR_COLOR_GREY, CHAR_COLOR_WHITE, CHAR_COLOR_YELLOW]

CHAR_NOT_CONTAINED = 0
CHAR_AT_DIFFERENT_POS = 1
CHAR_CORRECT = 2

KEY_OFF_NOCHANGE = 0
KEY_PRESSED = 1
KEY_RELEASED = 2
KEY_ON_NOCHANGE = 3

LOOP_SLEEP_TIME_SEC = 0.001

WORDS_IN_ONE_LINE = 8

class inputKey:
    def __init__(self, keyName:str):
        self.keyName = keyName
        self.prevState = 0

    def processInput(self):
        if keyboard.is_pressed(self.keyName):
            if self.prevState == 0:
                self.prevState = 1
                return KEY_PRESSED
            else:
                return KEY_ON_NOCHANGE

        else:
            if self.prevState == 1:
                self.prevState = 0
                return KEY_RELEASED
            else:
                return KEY_OFF_NOCHANGE

    def waitFor(self, status:int):
        while 1:
            if self.processInput() == status:
                return

            time.sleep(LOOP_SLEEP_TIME_SEC)



def clearWordMenu(returnCursor: bool = 0):
    utils.prnt(cursor.up(7))
    utils.prnt(screen.CLEAR_ROW)
    utils.prnt(screen.CLEAR_FWD)

    if returnCursor: utils.prnt(cursor.down(7))


def printWordMenu(word:str, wordColorsIndexes:list, controlPos:int, attempt:int):
    lenEqual = 1
    if len(word) != len(wordColorsIndexes):
        lenEqual = 0

    print(f'[Попытка {attempt} из {MAX_ATTEMPTS}]')
    print(f'Пометь буквы цветами, как в приложении:')
    print(' '*controlPos + '▲' + ' '*(len(word) - controlPos))

    for i in range(len(word)):
        if lenEqual: utils.prnt(CHAR_AVAILABLE_COLORS[wordColorsIndexes[i]])
        utils.prnt(word[i].upper())
    utils.prnt(colors.DEFAULT)

    print('\n' + ' '*controlPos + '▼' + ' '*(len(word) - controlPos))
    print('')
    print('Нажми Enter чтобы продолжить...')


def updateWordMenu(word:str, colors:list, controlPos:int, attempt:int):
    clearWordMenu()
    printWordMenu(word, colors, controlPos, attempt)


def flushInput():
    try:
        import msvcrt
        while msvcrt.kbhit():
            msvcrt.getch()
    except ImportError:
        import sys, termios
        termios.tcflush(sys.stdin, termios.TCIOFLUSH)


def exitProgram():
    enterKey.waitFor(KEY_OFF_NOCHANGE)
    enterKey.waitFor(KEY_PRESSED)
    enterKey.waitFor(KEY_RELEASED)
    flushInput()
    sys.exit(0)



leftKey = inputKey('left arrow')
rightKey = inputKey('right arrow')
upKey = inputKey('up arrow')
downKey = inputKey('down arrow')
enterKey = inputKey('enter')

currentWord = FIRST_WORD
currentWordCharColorsIndexes = [0] * WORD_LENGTH
cursorPos = 0


if __name__ == "__main__":
    try:
        os.system('') #чтобы работали ANSI коды


        utils.prnt(cursor.HIDE)
        utils.prnt(colors.DEFAULT)

        print('Загрузка...', end = '\r', flush=True)

        wordsFile = open(DICTIONARY_FILE, 'r',  encoding='utf-8')
        fittingWordsList = wordsFile.readlines()
        wordsFile.close()

        #оставляем только слова из 5 букв
        wordIndex = 0
        while wordIndex < len(fittingWordsList):
            word_ = fittingWordsList[wordIndex]
            wordFromList = word_[:-1]
            if word_[-1] != '\n':
                wordFromList = word_

            deleteWord = False

            if len(wordFromList) != WORD_LENGTH:
                fittingWordsList.pop(wordIndex)
            else:
                wordIndex += 1

        utils.prnt(screen.CLEAR_ROW)





        print('Управление:')
        print('← →  Выбор буквы')
        print('↑ ↓  Выбор цвета буквы')
        print('')
        print('Нажми Enter чтобы продолжить...')

        enterKey.waitFor(KEY_PRESSED)
        utils.prnt(cursor.up(5))
        utils.prnt(screen.CLEAR_FWD)

        print(f'Введи в приложении {colors.BRIGHT_BLUE_FG}{FIRST_WORD.upper()}{colors.DEFAULT}')
        print('Нажми Enter чтобы продолжить...')
        enterKey.waitFor(KEY_PRESSED)
        utils.prnt(cursor.up(2))
        utils.prnt(screen.CLEAR_FWD)




        for attempt in range(MAX_ATTEMPTS):
            #изменение цвета букв в слове
            printWordMenu(currentWord, currentWordCharColorsIndexes, cursorPos, attempt + 1)
            while 1:
                updateRequired = False

                if leftKey.processInput() == KEY_PRESSED:
                    if cursorPos > 0:
                        cursorPos -= 1
                        updateRequired = True

                if rightKey.processInput() == KEY_PRESSED:
                    if cursorPos < WORD_LENGTH - 1:
                        cursorPos += 1
                        updateRequired = True

                if upKey.processInput() == KEY_PRESSED:
                    if currentWordCharColorsIndexes[cursorPos] >= len(CHAR_AVAILABLE_COLORS) - 1:
                        currentWordCharColorsIndexes[cursorPos] = 0
                    else:
                        currentWordCharColorsIndexes[cursorPos] += 1

                    updateRequired = True

                if downKey.processInput() == KEY_PRESSED:
                    if currentWordCharColorsIndexes[cursorPos] <= 0:
                        currentWordCharColorsIndexes[cursorPos] = len(CHAR_AVAILABLE_COLORS) - 1
                    else:
                        currentWordCharColorsIndexes[cursorPos] -= 1

                    updateRequired = True

                if enterKey.processInput() == KEY_PRESSED:
                    break

                if updateRequired:
                    updateWordMenu(currentWord, currentWordCharColorsIndexes, cursorPos, attempt + 1)

                time.sleep(LOOP_SLEEP_TIME_SEC)


            clearWordMenu()





            #если все буквы желтые (слово отгадано)
            solved = True
            for charIndex in range(WORD_LENGTH):
                if currentWordCharColorsIndexes[charIndex] != CHAR_CORRECT:
                    solved = False
                    break

            if solved:
                print(f'{colors.GREEN}Ура! Получилось!{colors.DEFAULT_FG}')
                print('\nНажми Enter чтобы продолжить...')
                exitProgram()

            #не отагано, и это было последняя  попытка
            elif attempt == MAX_ATTEMPTS - 1:
                print(f'{colors.RED}Кажется кончились попыткти...{colors.DEFAULT_FG}')
                print('\nНажми Enter чтобы продолжить...')
                exitProgram()


            #отсеиваем неподходящие слова
            print('Поиск подходящих слов...', end = '\r', flush=True)


            wordIndex = 0
            while wordIndex < len(fittingWordsList):
                word_ = fittingWordsList[wordIndex]
                wordFromList = word_[:-1]
                if word_[-1] != '\n':
                    wordFromList = word_

                deleteWord = False

                for charIndex in range(WORD_LENGTH):
                    if currentWordCharColorsIndexes[charIndex] == CHAR_NOT_CONTAINED:
                        #если в введенном слове две одинаковых буквы, и одна из них угадана(стала желтой), то вторая будет станет серой
                        #в таком случае эта серая буква игнорируется
                        ignore = False
                        for charIndex2 in range(WORD_LENGTH):
                            if currentWord[charIndex] == currentWord[charIndex2] and currentWordCharColorsIndexes[charIndex2] == CHAR_CORRECT:
                                ignore = True

                        if not ignore and currentWord[charIndex] in wordFromList:
                            deleteWord = True


                    elif currentWordCharColorsIndexes[charIndex] == CHAR_AT_DIFFERENT_POS:
                        if currentWord[charIndex] == wordFromList[charIndex] or currentWord[charIndex] not in wordFromList:
                            deleteWord = True

                    elif currentWordCharColorsIndexes[charIndex] == CHAR_CORRECT:
                        if wordFromList[charIndex] != currentWord[charIndex]:
                            deleteWord = True


                if deleteWord:
                    fittingWordsList.pop(wordIndex)
                else:
                    wordIndex += 1





            utils.prnt(screen.CLEAR_ROW)

            if len(fittingWordsList) == 0:
                print(f'{colors.RED_FG}Подходящх слов не найдено{colors.DEFAULT_FG}')
                print('Боюсь, больше я ничем не смогу помочь :(')
                print('\nНажми Enter чтобы продолжить...')
                exitProgram()


            #вывод списка слов
            print('Выбери любое слово из списка:')
            utils.prnt(colors.BRIGHT_BLUE_FG)

            newLines = 2
            wordInLineIndex = 0
            for fittingWord_ in fittingWordsList:
                fittingWord = fittingWord_[:-1]
                if fittingWord_[-1] != '\n':
                    fittingWord = fittingWord_

                print(fittingWord, end = '', flush=True)
                wordInLineIndex += 1

                if wordInLineIndex == WORDS_IN_ONE_LINE:
                    if fittingWord_ != fittingWordsList[-1]:#если это последнее слово, строку не переносим
                        print('')
                        newLines += 1
                    wordInLineIndex = 0
                else:
                    print('    ', end = '', flush=True)





            #ввод нового слова
            print('\n')
            utils.prnt(colors.DEFAULT_FG)
            enterKey.waitFor(KEY_OFF_NOCHANGE)

            while 1:
                flushInput()
                utils.prnt(cursor.SHOW)
                inputWord = input('Введи выбранное слово сначала в приложении, а потом здесь: ').lower()
                utils.prnt(cursor.HIDE)
                if inputWord + '\n' not in fittingWordsList and inputWord not in fittingWordsList:
                    utils.prnt(cursor.PREVIOUS_LINE)
                    utils.prnt(screen.CLEAR_ROW)
                    print(f'{colors.RED_FG}Такого слова нет в списке{colors.DEFAULT_FG}')
                    print('Нажми Enter чтобы продолжить...')
                    enterKey.processInput()
                    enterKey.waitFor(KEY_PRESSED)
                    enterKey.waitFor(KEY_RELEASED)
                    utils.prnt(cursor.up(2))
                    utils.prnt(cursor.HOME_ON_ROW)
                    utils.prnt(screen.CLEAR_FWD)

                else:
                    currentWord = inputWord
                    enterKey.waitFor(KEY_OFF_NOCHANGE)
                    newLines += 3
                    break

            utils.prnt(cursor.up(newLines - 1))
            utils.prnt(cursor.HOME_ON_ROW)
            utils.prnt(screen.CLEAR_FWD)

            #новое слово
            cursorPos = 0
            currentWordCharColorsIndexes = [0] * WORD_LENGTH



    except KeyboardInterrupt: pass

    utils.prnt(colors.DEFAULT)
    utils.prnt(cursor.SHOW)
