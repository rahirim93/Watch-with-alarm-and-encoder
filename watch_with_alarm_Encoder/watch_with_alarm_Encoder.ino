#define CLK 7                               // Пин энкодера
#define DT 8                                // Пин энкодера
#define SW 9                                // Пин энкодера
#include "GyverEncoder.h"                   // Библиотека для работы с энкодером
#define FAST_ALGORITHM                      // Быстрый алгоритм для энкодера
#include <iarduino_RTC.h>                   // Библиотека для работы с модулем времени
#include <Wire.h>                           // Библиотека для работы с дисплеем
#include <LiquidCrystal_I2C.h>              // Библиотека для работы с дисплеем
LiquidCrystal_I2C lcd(0x27, 16, 2);         // Инициализация дисплея
iarduino_RTC time(RTC_DS1302, 10, 12, 11);  // Инициализация модуля времени RST, CLK, DAT.
Encoder enc1(CLK, DT, SW);                  // Инициализация энкодера
byte mode;                                  // Основной режим
byte mode2;                                 // Подрежим настройки времени
byte mode3;                                 // Подрежим настройки времени будильника
byte clearCounter;                          // Счетчик для отчистки экрана
byte setTimeCounter;                        // Счетчик для настройки времени
String timeCurrent = "";                    // Переменная для хранения текущего времени
long timerOutputTime;                       // Переменная для таймера вывода времени каждую секунду
byte getHours = 0;
byte getMinutes = 0;
byte setHours = 0;                          // Переменная для настройки часов
byte setMinutes = 0;                        // Переменная для настройки минут
byte hoursWake;                             // Переменная часов будильника
byte minutesWake;                           // Переменная минут будильника
boolean factAlarm;                          // Факт наступления времени пробуждения
int piezoPin = 2;                           // Вывод для зуммера
long piezoTimerOn;                          // Таймер для пищалки
long piezoTimerOff;
void setup() {
  enc1.setType(TYPE2);                      // Выбор типа энкодера
  Serial.begin(9600);                       // Открытие порта для передачи в порт
  time.begin();                             // Инициируем работу с модулем времени
  lcd.init();                               // Инициализация дисплея
  lcd.backlight();                          // Включаем подсветку дисплея
  lcd.clear();                              // Очистка экрана
  //delay(500);                             // Задержка для стабильности
  noTone(piezoPin);
  digitalWrite(piezoPin, HIGH);
}

void loop() {
  //    if (enc1.isRight()) Serial.println("Right");
  //    if (enc1.isLeft()) Serial.println("Left");
  //    if (enc1.isRightH()) Serial.println("Right holded");
  //    if (enc1.isLeftH()) Serial.println("Left holded");
  //    if (enc1.isClick()) Serial.println("Click");
  //    if (enc1.isSingle()) Serial.println("Single");
  //    if (enc1.isDouble()) Serial.println("Double");
  //    if (enc1.isHolded()) Serial.println("Holded");

  enc1.tick();                                    // Опрос энкодера

  if (enc1.isDouble()) {                          // Смена основного режима при двойном клике на энкодер
    clearCounter = 0;                             // Обнуление счетчика очистки экрана
    setTimeCounter = 0;
    mode++;                                       // Увеличение переменной режима
    if (mode > 2) mode = 0;                       // Ограничение количества режимов, при превышении на первый
  }
  switch (mode) {
    case 0: {
        clearDisplay();
        outputTime();
        //time.gettime();
        setTimeCounter = 0;
        alarmOn();
        alarmOff();

        if (factAlarm) {
          alarmSound();
        } else {
          noTone(piezoPin);
          digitalWrite(piezoPin, 1);
        }

        lcd.setCursor(0, 1);
        lcd.print(hoursWake); lcd.print(":"); lcd.print(minutesWake);
      }
      break;

    case 1: {
        clearDisplay();
        setTime();
      }
      break;

    case 2: {
        clearDisplay();
        setAlarmTime();
      }
      break;
  }
}
// Настройка времени будильника
void setAlarmTime() {
  if (enc1.isRightH()) { // Переключение режимов часы/минуты
    mode3++;
    if (mode3 > 1) mode3 = 0;
    setTimeCounter = 0;
  }
  switch (mode3) {
    case 0: { // Настройка часов будильника
        if (setTimeCounter == 0) {
          lcd.clear();
          setTimeCounter++;
          lcd.print(hoursWake); lcd.print(":"); lcd.print(minutesWake);
          lcd.setCursor(7, 0); lcd.print("Alarm");
          lcd.setCursor(0, 1); lcd.print("Set Hours");
        }
        if (enc1.isRight()) {
          hoursWake++;
          if (hoursWake > 23) {
            hoursWake = 0;
            lcd.clear();
          }
          lcd.setCursor(0, 0);
          lcd.print(hoursWake); lcd.print(":"); lcd.print(minutesWake);
        }
      }
      break;
    case 1: { // Настройка минут будильника
        if (setTimeCounter == 0) {
          lcd.clear();
          setTimeCounter++;
          lcd.print(hoursWake); lcd.print(":"); lcd.print(minutesWake);
          lcd.setCursor(7, 0); lcd.print("Alarm");
          lcd.setCursor(0, 1); lcd.print("Set minutes");
        }
        if (enc1.isRight()) {
          minutesWake++;
          if (minutesWake > 59) {
            minutesWake = 0;
            lcd.clear();
          }
          lcd.setCursor(0, 0);
          lcd.print(hoursWake); lcd.print(":"); lcd.print(minutesWake);

        }
      }
      break;
  }
}
// Функция настройки времени
void setTime() {
  if (enc1.isRightH()) { // Переключение режимов часы/минуты
    mode2++;
    if (mode2 > 1) mode2 = 0;
    setTimeCounter = 0;
  }
  switch (mode2) {
    case 0: { // Настройка часов
        if (setTimeCounter == 0) {
          setTimeCounter++;
          outputTime();
          setHours = time.Hours;
          lcd.setCursor(0, 1);
          lcd.print("h");
        }
        if (enc1.isRight()) {
          setHours++;
          if (setHours > 23) {
            setHours = 0;
            lcd.clear();
            outputTime();
          }
          time.settime(-1, -1, setHours);
          outputTime();
        }
      }
      break;
    case 1: {
        if (setTimeCounter == 0) {
          setTimeCounter++;
          outputTime();
          setMinutes = time.minutes;
          lcd.setCursor(0, 1);
          lcd.print("m");
        }
        if (enc1.isRight()) {
          setMinutes++;
          if (setMinutes > 59) {
            setMinutes = 0;
            lcd.clear();
            outputTime();
          }
          time.settime(-1, setMinutes, -1);
          outputTime();
        }
      }
      break;
  }
}
void outputTime() {
  if (millis() - timerOutputTime > 1000) {
    timerOutputTime;
    timeCurrent = time.gettime("H:i:s");
    lcd.setCursor(0, 0);
    lcd.print(timeCurrent);
  }
}
void clearDisplay() {
  if (clearCounter == 0) {
    lcd.clear();
    clearCounter++;
  }
}
void alarmOn() {
  time.gettime();
  getHours = time.Hours;
  getMinutes = time.minutes;
  if (getHours == hoursWake && getMinutes == minutesWake) factAlarm = 1; //Включить зуммер если наступило время пробуждения
}
void alarmSound () {
  if (millis() - piezoTimerOn > 1000) {
    tone(piezoPin, 1100);
    piezoTimerOn = millis();
  }
  if (millis() - piezoTimerOff > 2000) {
    noTone(piezoPin);
    piezoTimerOff = millis();
  }
}
void alarmOff() {
  if (enc1.isHolded()) factAlarm = 0;                //Если кнопка нажата - выключить зуммер
}
