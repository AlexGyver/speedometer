/*
Created 2016
by AlexGyver
AlexGyver Home Labs Inc.
*/

#include <EEPROM.h>   //библиотека для работы со внутренней памятью ардуино

//-----------дисплей-----------
#include <TimerOne.h>
#include <TM74HC595Display.h>
int SCLK = 7;
int RCLK = 6;
int DIO = 5;
TM74HC595Display disp(SCLK, RCLK, DIO);
unsigned char LED_0F[29];
//--------дисплей-------

unsigned long lastturn, time_press; //переменные хранения времени
float SPEED; //переменная хранения скорости в виде десятичной дроби
float DIST; //переменная хранения расстояния в виде десятичной дроби
float w_length=2.050; //длина окружности колеса в метрах
boolean flag; //флажок для хранения (что выводим на дисплее, скорость или расстояние)
boolean state, button; //флажки для обработчика нажатия кнопки

void setup() {
  Serial.begin(9600);  //открыть порт
  attachInterrupt(0,sens,RISING); //подключить прерывание на 2 пин при повышении сигнала
  pinMode(3, OUTPUT);   //3 пин как выход
  digitalWrite(3, HIGH);  //подать 5 вольт на 3 пин
  pinMode(8, INPUT);   //сюда подключена кнопка
  
  //для дисплея цифры----------
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  
  LED_0F[10] = 0b01000000; //.0
  LED_0F[11] = 0b01111001; //.1
  LED_0F[12] = 0b00100100; //.2
  LED_0F[13] = 0b00110000; //.3
  LED_0F[14] = 0b00011001; //.4
  LED_0F[15] = 0b00010010; //.5
  LED_0F[16] = 0b00000010; //.6
  LED_0F[17] = 0b01111000; //.7
  LED_0F[18] = 0b00000000; //.8
  LED_0F[19] = 0b00010000; //.9
  Timer1.initialize(1500); // set a timer of length 1500
  Timer1.attachInterrupt(timerIsr); // attach the service routine here
  //--------------
  DIST=(float)EEPROM.read(0)/10.0; //вспоминаем пройденное расстояние при запуске системы (деление на 10 нужно для сохранения десятых долей расстояния, см. запись)
}

void sens() {
  if (millis()-lastturn > 80) {  //защита от случайных измерений (основано на том, что велосипед не будет ехать быстрее 120 кмч)
    SPEED=w_length/((float)(millis()-lastturn)/1000)*3.6;  //расчет скорости, км/ч
    lastturn=millis();  //запомнить время последнего оборота
    DIST=DIST+w_length/1000;  //прибавляем длину колеса к дистанции при каждом обороте оного
  }
}

void loop() {
  int cel_sp=floor(SPEED);
  int sot_sp=(((float)cel_sp/1000)-floor((float)cel_sp/1000))*10;
  int des_sp=(((float)cel_sp/100)-floor((float)cel_sp/100))*10;
  int ed_sp=(((float)cel_sp/10)-floor((float)cel_sp/10))*10;
  int dr_sp=(float)(SPEED-floor(SPEED))*10;
  
  if (flag==0) {
    //disp.set(LED_0F[sot_sp],3);  //вывод сотен скорости (для велосипеда не нужно =)
    disp.set(LED_0F[des_sp],2);  //вывод десятков скорости
    disp.set(LED_0F[ed_sp+10],1);  //вывод единиц скорости, с точкой
    disp.set(LED_0F[dr_sp],0);  //вывод после точки   
  }

  int cel_di=floor(DIST);  //целые
  int sot_di=(((float)cel_di/1000)-floor((float)cel_di/1000))*10;  //сотни
  int des_di=(((float)cel_di/100)-floor((float)cel_di/100))*10;  //десятки
  int ed_di=floor(((float)((float)cel_di/10)-floor((float)cel_di/10))*10);  //единицы (с точкой)
  int dr_di=(float)(DIST-floor(DIST))*10;  //десятые части
  
  if (flag==1) {
    disp.set(LED_0F[sot_di],3);  //вывод сотен расстояния
    disp.set(LED_0F[des_di],2);  //вывод десятков расстояния
    disp.set(LED_0F[ed_di+10],1);  //вывод единиц расстояния, с точкой
    disp.set(LED_0F[dr_di],0);  //вывод после точки  
  }

  if ((millis()-lastturn)>2000){ //если сигнала нет больше 2 секунды
    SPEED=0;  //считаем что SPEED 0
    EEPROM.write(0,(float)DIST*10.0); //записываем DIST во внутреннюю память. Сделал так хитро, потому что внутренняя память не любит частой перезаписи. Также *10, чтобы сохранить десятую долю
  }

    if (digitalRead(8)==1 && state==0) {        //выбор режимов кнопкой. Если кнопка нажата
    state=1;
    button=1;
    time_press=millis();              //запомнить время нажатия
    while (millis()-time_press<500) {        //выполнять, пока кнопка нажата не менее 500 миллисекунд
      if (digitalRead(8)==0) {            //если кнопка отпущена, выйти из цикла и принять button=0
        button=0;
        break;
      }
    }        //если кнопка не отпущена 500мс, то button будет равен 1
    switch (button) {
    case 0:
      flag=!flag;state=0;disp.clear();  //просто переключение режима отображения скорости и расстояния
      break;
    case 1:
      if (flag==1) {DIST=0;state=0;disp.clear();}; //если мы в режиме отображения расстояния и кнопка удерживается, то обнулить расстояние
      break;
    }
  }
}

void timerIsr()  //нужно для дисплея
{
  disp.timerIsr();
}

