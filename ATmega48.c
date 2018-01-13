/*******************************************************
This program was created by the CodeWizardAVR V3.24
Automatic Program Generator
© Copyright 1998-2015 Pavel Haiduc, HP InfoTech s.r.l.
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
//Подключаем библиотеку низкоуровневых функций ввода-вывода.
#include <io.h>
//Библиотека с функциями, касающимися только ATmega48.
#include <mega48p.h>
//Библиотека реализация временных задержек
#include <delay.h>
//Подключаем библиотеку прерываний.
#include <interrupt.h>

//Константы
//Количество рядов светодиодов в матрице
#define NUM_LINES 7
//Количество столбцов светодиодов в светодиодной матрице
#define NUM_COLUMNS 5

#define DURATION_IMP 3 /*Длительность 1 импульса в тактах счетчика*/
#define CRC_POLINOME 4 /*Полином для расчета CRC*/
#define MAX_LENGHT_SEND_WORD 24 //максимальная длина слова в битах
//Стартовый байт, который означает начало пакета данных.
#define START_BYTE 0xFF
//Стоповый байт, который означает конец пакета данных.
#define STOP_BYTE 0xAA

int numLines=7;
int numColumns=5;


char init=0; //Флаг инициализации программы.
char countImpulse=0;//Счетчик количества принятых импульсов.
char logicStateImpulse=0;//Логическое состояние принятого текущего импульса
char data[MAX_LENGHT_SEND_WORD+1]; //Массив принятых данных
char speed=0;//Переменная, хранящая в себе значение скорости автомобиля.
char n_speed=0;//Переменная, хранящая в себе номер включенной скорости автомобиля.
char brightness=1;//регулировка яркости матрицы

//методы
//Функция, которая включает светодиоды, согласно матрице в параметре.
void _(unsigned []);
//Инициализация восьмибитного таймера-счетчика.
void TIMER_8bit_Init(void);
//Инициализация шестнадцатибитного таймера-счетчика.
void TIMER_16bit_Init(void);
//Распределяет принятые цифровые данные.
void parceData(char, char);
//Вычисляет контрольную сумму.
void calculateCRC(void);


//константы, которые отвечают за зеленые строки светодиодной матрицы
#define LINE_1R 0 //№ bit
#define LINE_2R 1
#define LINE_3R 2
#define LINE_4R 3
#define LINE_5R 4
#define LINE_6R 5
#define LINE_7R 6

//Далее следуют константы, которые отвечают за строки с красными светодиодной матрицы.
#define LINE_1G 0 //№ bit
#define LINE_2G 1
#define LINE_3G 2
#define LINE_4G 3
#define LINE_5G 4
#define LINE_6G 5
#define LINE_7G 6

//Константы, хранящие позиции столбцов светодиодной матрицы.
#define COLUMN_1 0 //№ bit
#define COLUMN_2 1
#define COLUMN_3 5
#define COLUMN_4 6
#define COLUMN_5 7

//Построение цифры 1 на светодиодной матрице, где установка единицы в каждый бит равняется включенному в этой позиции светодиоду.
unsigned int lineRedMatrix[numLines-1]={LINE_1R, LINE_2R, LINE_3R, LINE_4R, LINE_5R, LINE_6R, LINE_7R};
//unsigned int lineGreenMatrix[numLines-1]={LINE_1G, LINE_2G, LINE_3G, LINE_4G, LINE_5G, LINE_6G, LINE_7G};
unsigned int columnsMatrix[numColumns-1]={COLUMN_1, COLUMN_2, COLUMN_3, COLUMN_4, COLUMN_5};

//Построение цифры 1 на светодиодной матрице
unsigned int num1[numLines-1]={0b00010, 0b00110, 0b01010, 0b00010, 0b00010, 0b00010, 0b00010};
//Построение цифры 2 на светодиодной матрице
unsigned int num2[numLines-1]={0b01100, 0b10010, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111};
//Построение цифры 3 на светодиодной матрице
unsigned int num3[numLines-1]={0b01110, 0b00001, 0b00001, 0b00010, 0b00001, 0b00001, 0b01110};
//Построение цифры 4 на светодиодной матрице
unsigned int num4[numLines-1]={0b00011, 0b00101, 0b01001, 0b11111, 0b00001, 0b00001, 0b00001};
//Построение цифры 5 на светодиодной матрице
unsigned int num5[numLines-1]={0b11111, 0b00001, 0b00001, 0b11111, 0b10000, 0b10000, 0b11111};
//Построение цифры 6 на светодиодной матрице
unsigned int numBack[numLines-1]={0b00010, 0b00110, 0b01010, 0b00010, 0b00010, 0b00010, 0b00010};

void main(void)
{
DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

DDRC=(0<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (1<<DDD1) | (1<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

//Настраиваем прерывания
sei(); //Глобальное разрешение прерываний.
//Установкой битов ISC01 и ISC00 в регистр EICRA означает,
// что прерывание будет срабатывать по любому состоянию входа INT0.
EICRA =(0<<ISC01)|(1<<ISC00);

TIMER_8bit_Init();// инициализируем 8-ми разрядный таймер-счетчик
TIMER_16bit_Init();// инициализируем 16-ти разрядный таймер-счетчик

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
//определяет какие светодиоды должны включиться
void _(unsigned num[]){
   int i1=0;
   for(i1=0; i1<=(numColumns-1); i1++){  //Отображение по колонкам
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


//внешнее прерывание по входу. обработчик.
//Ловим пакет данных
ISR(EXT_INT0)
{
    if(!init){
        init=1;
        TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00);//запускаем таймер-счетчик
    }
    //Фильтруем данные от помех, пропуская только импульсы длинной не менее заданной.
    if(logicStateImpulse & TCNT0>=DURATION_IMP){
      logicStateImpulse=0;
      parceData(TCNT0,logicStateImpulse);
    }else if (!logicStateImpulse & TCNT0>=DURATION_IMP){
      logicStateImpulse=1;
      parceData(TCNT0,logicStateImpulse);
    }

};

//Функция производит побитный разбор принятых данных.
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
//Функция производит расчет контрольной суммы
void calculateCRC(void)
{
  if(((data[0]+data[1]+data[2])%CRC_POLINOME)==data[(MAX_LENGHT_SEND_WORD/8)-1]){
    n_speed=data[1];
    speed=data[2];
  }
}
 /* Инициализация 8-разрядного таймера-счетчика
    -все биты WGM02…WGM00 равны нулю (режим Normal)
    -предделитель 1024 (биты CS02…CS00)
 */
void TIMER_8bit_Init(void)
{
    TCCR0A = 0;
    TCCR0A = (0<<WGM02)|(0<<WGM01)|(0<<WGM00);
    TCCR0B = 0;
    TCCR0B = (0<<CS02)|(0<<CS01)|(0<<CS00); // stop/ делитель
    TCNT0=0;
}

 /* Инициализация 16-разрядного таймера-счетчика
    -все биты WGM13 WGM12 равны нулю (режим Normal)
    -предделитель 1 (биты CS12…CS10)
 */
void TIMER_16bit_Init(void)
{
        TCCR1A=(0<<WGM13)|(0<<WGM12)|(0<<WGM11)|(0<<WGM10);
        TCCR1B=(0<<CS12)|(0<<CS11)|(1<<CS10);
        TCNT1L = 0;// младший байт 16 разр. таймера
        TCNT1H = 0;// старший байт 16 разр. таймера
}
void control_display(void){
OCR1AH=0x01;

}

ISR(TIM1_COMPA)
{

}