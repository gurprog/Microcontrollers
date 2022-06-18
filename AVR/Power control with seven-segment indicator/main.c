#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned int razr1,razr2,razr3,razr4,qwer=0,cif,k=0,U=0,reg=720,zzz=1,chis,s=0,schet=0;
unsigned int cifra [10] = {0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};
unsigned int N [4] = {0b00001111,0b00001101,0b00001011,0b00000111};

void razryad (unsigned int chislo)
{
	if (qwer == 0)
	{
		cif=chislo/1000;
	}
	if (qwer == 1)
	{
		cif=chislo%1000/100;
	}
	if (qwer == 2)
	{
		cif=chislo%100/10;
	}
	if (qwer == 3)
	{
		cif=chislo%10;
	}
}

ISR (INT0_vect)
{
 	k++;
	_delay_ms(20);
}

ISR (TIMER0_OVF_vect)
{
	if (~PIND&(1<<2))
	{
		SPDR=0b00001110;
		while (!(SPSR&(1<<SPIF)));
		SPDR=0b00111110;
		while (!(SPSR&(1<<SPIF)));
		PORTB |=(1<<PORTB2);
		PORTB &=~(1<<PORTB2);
		SPDR=0b00001101;
		while (!(SPSR&(1<<SPIF)));
		SPDR=0b01110111;
		while (!(SPSR&(1<<SPIF)));
		PORTB |=(1<<PORTB2);
		PORTB &=~(1<<PORTB2);
		SPDR=0b00001011;
		while (!(SPSR&(1<<SPIF)));
		SPDR=0b01110011;
		while (!(SPSR&(1<<SPIF)));
		PORTB |=(1<<PORTB2);
		PORTB &=~(1<<PORTB2);
		SPDR=0b00000111;
		while (!(SPSR&(1<<SPIF)));
		SPDR=0b01111001;
		while (!(SPSR&(1<<SPIF)));
		PORTB |=(1<<PORTB2);
		PORTB &=~(1<<PORTB2);
	}
	else
	{
		if (s==1)
		{
			U=ADC/2.45;
			schet++;
			if (schet>600){schet=0;s=0;}
		}
		else
		{
			U=reg/2.45;	
		}
		razryad(U);
		SPDR = N[qwer];
		while (!(SPSR&(1<<SPIF)));
		if (qwer==1)
		{
			chis=cifra[cif]+0b10000000;	
		}
		else
		{
			chis=cifra[cif];
		}
		SPDR = chis;
		while (!(SPSR&(1<<SPIF)));
		PORTB |=(1<<PORTB2);
		PORTB &=~(1<<PORTB2);
		qwer++;
		if (qwer>3)
		{
			qwer=0;
		}
		}
}


int main(void)
{
	//Настройка прерывания кнопки ФАЙР
	DDRD &=~(1<<2);
	PORTD |=(1<<2);
// 	DDRD =(1<<3);
// 	PORTD |=(1<<3);
	GICR |=(1<<6);
	MCUCR &=~(1<<ISC00);
	MCUCR |=(1<<ISC01);
	
	//Настройка АЦП
	ADCSRA |=(1<<ADEN);//включение АЦП
	ADCSRA |=(1<<ADFR);//непрерывное измерение
	ADCSRA &=~(1<<ADPS2);//деление частоты
	ADCSRA &=~(1<<ADPS1);
	ADCSRA |=(1<<ADPS0);
	ADMUX |=((1<<REFS1)|(1<<REFS1));//опорное напряжение 2,56
	ADMUX &=~(1<<ADLAR);
	ADMUX &=~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));//выбор порта ацп
	ADCSRA |=(1<<ADSC); //запуск АЦП
	
	//Настройка ШИМ
	DDRB |=(1<<1);//включение порта B1 на выход
	PORTB &=~(1<<1);
	TCCR1A |=(1<<COM1A1);//вклчение портов на шим
	TCCR1A &=~(1<<COM1A0);
	TCCR1A |=(1<<WGM10);// выбор режима шим
	TCCR1A |=(1<<WGM11);
	TCCR1B |=(1<<WGM12);
	TCCR1B &=~(1<<WGM13);
	TCCR1B |=(1<<CS10);// предделитель таймера 1
	TCCR1B &=~(1<<CS11);
	TCCR1B &=~(1<<CS12);
	OCR1A = 0;// регистр сравнения
	
	//Настройка индикации
	TCCR0 &=~(1<<CS00);// предделитель таймера 0
	TCCR0 |=(1<<CS01);
	TCCR0 &=~(1<<CS02);
	TIMSK |=(1<<0); // прерывания таймера 0
	SREG |=(1<<7);//разрешение глобальных прерываний
	DDRD &=~((1<<0)|(1<<1));//настройка кнопок
	PORTD |=((1<<0)|(1<<1));
	DDRB |=((1<<DDB2)|(1<<DDB3)|(1<<DDB5));//Настройка SPI
	PORTB &=~((1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5));
	SPCR |= ((1<<SPE)|(1<<MSTR));
	
	while (1)
	{
		//Настройка однократного нажатия
		if (~PIND&(1<<2)&&(k==1))
		{
			if (ADC>720)
			{
				OCR1A=64*reg/ADC;
				OCR1A=OCR1A*16-1;
			}
			else
			{
				OCR1A=1023;
			}
			_delay_ms(800);
		}
		//Максимум мощности при двукратном нажатии
			if (~PIND&(1<<2)&&(k==2))
			{
				OCR1A=1023;
				_delay_ms(50);
			}
				
			if ((~PIND&(1<<0))&&(~PIND&(1<<1)))
			{
				s=1;
				_delay_ms(50);
			}

		// Отключение кнопки файр
		if (PIND&(1<<2))
		{
			OCR1A=0;
			k=0;
		}
		
		if (~ADCSRA&(1<<4))
		{
			zzz=ADC;
			ADCSRA|=(1<<4);
		}
				
		//Настройка изменения вых U
		if (~PIND&(1<<0))
		{
			if (reg>720)
			{
				reg-=1;
				_delay_ms(5);
			}
			
		}
		if ((reg>ADC)&&(ADC>724))
		{
			reg=ADC-3;
		}
		if (reg<720){reg=720;}
		if (~PIND&(1<<1))
		{
			if ((reg<1024)&&(reg<ADC))
			{
				reg+=1;
				_delay_ms(5);
			}
			
		}
	}
}