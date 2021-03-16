/*
  TM74HC595Display.cpp - Library
*/
 
#include "TM74HC595Display.h"
#include <Arduino.h>

unsigned char _LED_0F[29];
   
TM74HC595Display::TM74HC595Display(int SCLK, int RCLK, int DIO)
{
  pinMode(SCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(DIO, OUTPUT);
    
  _SCLK = SCLK;
  _RCLK = RCLK;
  _DIO = DIO;
  
  _LED_0F[0] = 0xC0; //0
  _LED_0F[1] = 0xF9; //1
  _LED_0F[2] = 0xA4; //2
  _LED_0F[3] = 0xB0; //3
  _LED_0F[4] = 0x99; //4
  _LED_0F[5] = 0x92; //5
  _LED_0F[6] = 0x82; //6
  _LED_0F[7] = 0xF8; //7
  _LED_0F[8] = 0x80; //8
  _LED_0F[9] = 0x90; //9
  _LED_0F[10] = 0x88; //A
  _LED_0F[11] = 0x83; //b
  _LED_0F[12] = 0xC6; //C
  _LED_0F[13] = 0xA1; //d
  _LED_0F[14] = 0x86; //E
  _LED_0F[15] = 0x8E; //F
  _LED_0F[16] = 0xC2; //G
  _LED_0F[17] = 0x89; //H
  _LED_0F[18] = 0xF9; //I
  _LED_0F[19] = 0xF1; //J
  _LED_0F[20] = 0xC3; //L
  _LED_0F[21] = 0xA9; //n
  _LED_0F[22] = 0xC0; //O
  _LED_0F[23] = 0x8C; //P
  _LED_0F[24] = 0x98; //q
  _LED_0F[25] = 0x92; //S
  _LED_0F[26] = 0xC1; //U
  _LED_0F[27] = 0x91; //Y
  _LED_0F[28] = 0xFE; //hight -
  
	clear();
}

void TM74HC595Display::timerIsr()
{
  send(_DATA[0]);
  send(0b0001);
  digitalWrite(_RCLK, LOW);
  digitalWrite(_RCLK, HIGH);
  send(_DATA[1]);
  send(0b0010);
  digitalWrite(_RCLK, LOW);
  digitalWrite(_RCLK, HIGH);
  send(_DATA[2]);
  send(0b0100);
  digitalWrite(_RCLK, LOW);
  digitalWrite(_RCLK, HIGH);
  send(_DATA[3]);
  send(0b1000);
  digitalWrite(_RCLK, LOW);
  digitalWrite(_RCLK, HIGH);
}


void TM74HC595Display::send(unsigned char X)
{

  for (int i = 8; i >= 1; i--)
  {
    if (X & 0x80)
    {
      digitalWrite(_DIO, HIGH);
    }
    else
    {
      digitalWrite(_DIO, LOW);
    }
    X <<= 1;
    digitalWrite(_SCLK, LOW);
    digitalWrite(_SCLK, HIGH);  
  }
}

void TM74HC595Display::send(unsigned char X, unsigned char port)
{
  send(X);
  send(port);
  digitalWrite(_RCLK, LOW);
  digitalWrite(_RCLK, HIGH);
}

void TM74HC595Display::digit4(int n, bool showZero)
{
  int n1, n2, n3, n4;
  n1 = (int)  n % 10;
  n2 = (int) ((n % 100)-n1)/10;
  n3 = (int) ((n % 1000) - n2 - n1) / 100;
  n4 = (int) ((n % 10000) - n3 - n2 - n1) / 1000;

	set(_LED_0F[n1], 0);
    if(showZero | n>9)set(_LED_0F[n2], 1);
    if(showZero | n>99)set(_LED_0F[n3], 2);
    if(showZero | n>999)set(_LED_0F[n4], 3);
}

void TM74HC595Display::digit4(int n)
{
  digit4(n,false);
}

void TM74HC595Display::digit4showZero(int n)
{
	digit4(n, true);
}


void TM74HC595Display::digit2(int n, int port)
{
	 int n1, n2;
  n1 = (int)  n % 10;
  n2 = (int) ((n % 100)-n1)/10;
  
   set(_LED_0F[n1], port);
   set(_LED_0F[n2], port+1); 

}

void TM74HC595Display::set(unsigned char X, int port ){
	_DATA[port] = X;
}

void TM74HC595Display::clear(){
  _DATA[0]= 0xFF;
  _DATA[1]= 0xFF;
  _DATA[2]= 0xFF;
  _DATA[3]= 0xFF;
}