/*******************************************************
This program was created by the CodeWizardAVR V3.24
Automatic Program Generator
� Copyright 1998-2015 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project :
Version :
Date    : 15.06.2016
Author  :
Company :
Comments:


Chip type               : ATmega48P
AVR Core Clock frequency: 8,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 128
*******************************************************/
//���������� ���������� �������������� ������� �����-������.
#include <io.h>
//���������� � ���������, ����������� ������ ATmega48.
#include <mega48p.h>
//���������� ���������� ��������� ��������
#include <delay.h>
//���������� ���������� ����������.
#include <interrupt.h>

//���������
//���������� ����� ����������� � �������
#define NUM_LINES 7
//���������� �������� ����������� � ������������ �������
#define NUM_COLUMNS 5

#define DURATION_IMP 3 /*������������ 1 �������� � ������ ��������*/
#define CRC_POLINOME 4 /*������� ��� ������� CRC*/
#define MAX_LENGHT_SEND_WORD 24 //������������ ����� ����� � �����
//��������� ����, ������� �������� ������ ������ ������.
#define START_BYTE 0xFF
//�������� ����, ������� �������� ����� ������ ������.
#define STOP_BYTE 0xAA

int numLines=7;
int numColumns=5;


char init=0; //���� ������������� ���������.
char countImpulse=0;//������� ���������� �������� ���������.
char logicStateImpulse=0;//���������� ��������� ��������� �������� ��������
char data[MAX_LENGHT_SEND_WORD+1]; //������ �������� ������
char speed=0;//����������, �������� � ���� �������� �������� ����������.
char n_speed=0;//����������, �������� � ���� ����� ���������� �������� ����������.
char brightness=1;//����������� ������� �������

//������
//�������, ������� �������� ����������, �������� ������� � ���������.
void _(unsigned []);
//������������� ������������� �������-��������.
void TIMER_8bit_Init(void);
//������������� ������������������ �������-��������.
void TIMER_16bit_Init(void);
//������������ �������� �������� ������.
void parceData(char, char);
//��������� ����������� �����.
void calculateCRC(void);


//���������, ������� �������� �� ������� ������ ������������ �������
#define LINE_1R 0 //� bit
#define LINE_2R 1
#define LINE_3R 2
#define LINE_4R 3
#define LINE_5R 4
#define LINE_6R 5
#define LINE_7R 6

//����� ������� ���������, ������� �������� �� ������ � �������� ������������ �������.
#define LINE_1G 0 //� bit
#define LINE_2G 1
#define LINE_3G 2
#define LINE_4G 3
#define LINE_5G 4
#define LINE_6G 5
#define LINE_7G 6

//���������, �������� ������� �������� ������������ �������.
#define COLUMN_1 0 //� bit
#define COLUMN_2 1
#define COLUMN_3 5
#define COLUMN_4 6
#define COLUMN_5 7

//���������� ����� 1 �� ������������ �������, ��� ��������� ������� � ������ ��� ��������� ����������� � ���� ������� ����������.
unsigned int lineRedMatrix[numLines-1]={LINE_1R, LINE_2R, LINE_3R, LINE_4R, LINE_5R, LINE_6R, LINE_7R};
//unsigned int lineGreenMatrix[numLines-1]={LINE_1G, LINE_2G, LINE_3G, LINE_4G, LINE_5G, LINE_6G, LINE_7G};
unsigned int columnsMatrix[numColumns-1]={COLUMN_1, COLUMN_2, COLUMN_3, COLUMN_4, COLUMN_5};

//���������� ����� 1 �� ������������ �������
unsigned int num1[numLines-1]={0b00010, 0b00110, 0b01010, 0b00010, 0b00010, 0b00010, 0b00010};
//���������� ����� 2 �� ������������ �������
unsigned int num2[numLines-1]={0b01100, 0b10010, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111};
//���������� ����� 3 �� ������������ �������
unsigned int num3[numLines-1]={0b01110, 0b00001, 0b00001, 0b00010, 0b00001, 0b00001, 0b01110};
//���������� ����� 4 �� ������������ �������
unsigned int num4[numLines-1]={0b00011, 0b00101, 0b01001, 0b11111, 0b00001, 0b00001, 0b00001};
//���������� ����� 5 �� ������������ �������
unsigned int num5[numLines-1]={0b11111, 0b00001, 0b00001, 0b11111, 0b10000, 0b10000, 0b11111};
//���������� ����� 6 �� ������������ �������
unsigned int numBack[numLines-1]={0b00010, 0b00110, 0b01010, 0b00010, 0b00010, 0b00010, 0b00010};

void main(void)
{
DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

DDRC=(0<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (1<<DDD1) | (1<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

//����������� ����������
sei(); //���������� ���������� ����������.
//���������� ����� ISC01 � ISC00 � ������� EICRA ��������,
// ��� ���������� ����� ����������� �� ������ ��������� ����� INT0.
EICRA =(0<<ISC01)|(1<<ISC00);

TIMER_8bit_Init();// �������������� 8-�� ��������� ������-�������
TIMER_16bit_Init();// �������������� 16-�� ��������� ������-�������

while (1)
    {
     switch(n_speed){
      case 1:
        _(num1);
      break;
      case 2:
        _(num2);
      break;
      case 3:
        _(num3);
      break;
      case 4:
        _(num4);
      break;
      case 5:
        _(num5);
      break;
      case 6:
        //_(num6);
      break;
      case 7:
        _(numBack);
      break;
      }
    }
}
//���������� ����� ���������� ������ ����������
void _(unsigned num[]){
   int i1=0;
   for(i1=0; i1<=(numColumns-1); i1++){  //����������� �� ��������
    int i2=0;
        for(i2=0; i2<=(numLines-1); i2++){
        if((num[i2] & (1<<i1))==(1<<i1)){
            PORTB|=(1<<lineRedMatrix[i2]);
            PORTD|=(1<<columnsMatrix[(numColumns-1)-i1]);
        };
    };
    delay_ms(50);
    PORTB=0;
    PORTD=0;
};
}


//������� ���������� �� �����. ����������.
//����� ����� ������
ISR(EXT_INT0)
{
    if(!init){
        init=1;
        TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00);//��������� ������-�������
    }
    //��������� ������ �� �����, ��������� ������ �������� ������� �� ����� ��������.
    if(logicStateImpulse & TCNT0>=DURATION_IMP){
      logicStateImpulse=0;
      parceData(TCNT0,logicStateImpulse);
    }else if (!logicStateImpulse & TCNT0>=DURATION_IMP){
      logicStateImpulse=1;
      parceData(TCNT0,logicStateImpulse);
    }

};

//������� ���������� �������� ������ �������� ������.
void parceData(char time, char logicState)
{
   int i=0;
    for(i=0; i<time/DURATION_IMP; i++){
     countImpulse++;
     data[countImpulse/8]=logicState<<(countImpulse%8);
    }
    if(countImpulse>=8 & !data[0]==0xFF){
        countImpulse=0;
    } else if(countImpulse>=MAX_LENGHT_SEND_WORD){
        countImpulse=0;
        calculateCRC();
    }
    TCNT0=0;
  }
//������� ���������� ������ ����������� �����
void calculateCRC(void)
{
  if(((data[0]+data[1]+data[2])%CRC_POLINOME)==data[(MAX_LENGHT_SEND_WORD/8)-1]){
    n_speed=data[1];
    speed=data[2];
  }
}
 /* ������������� 8-���������� �������-��������
    -��� ���� WGM02�WGM00 ����� ���� (����� Normal)
    -������������ 1024 (���� CS02�CS00)
 */
void TIMER_8bit_Init(void)
{
    TCCR0A = 0;
    TCCR0A = (0<<WGM02)|(0<<WGM01)|(0<<WGM00);
    TCCR0B = 0;
    TCCR0B = (0<<CS02)|(0<<CS01)|(0<<CS00); // stop/ ��������
    TCNT0=0;
}

 /* ������������� 16-���������� �������-��������
    -��� ���� WGM13 WGM12 ����� ���� (����� Normal)
    -������������ 1 (���� CS12�CS10)
 */
void TIMER_16bit_Init(void)
{
        TCCR1A=(0<<WGM13)|(0<<WGM12)|(0<<WGM11)|(0<<WGM10);
        TCCR1B=(0<<CS12)|(0<<CS11)|(1<<CS10);
        TCNT1L = 0;// ������� ���� 16 ����. �������
        TCNT1H = 0;// ������� ���� 16 ����. �������
}
void control_display(void){
OCR1AH=0x01;

}

ISR(TIM1_COMPA)
{

}