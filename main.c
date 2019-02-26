/*
 * GccApplication2.c
 *
 * Created: 10/8/2018 7:06:50 PM
 * Author : Lashan
 */ 

#include <avr/io.h>
#define F_CPU 11059200
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

void init_lcd(void);
void moveto(unsigned char, unsigned char);
void stringout(char *);
void writecommand(unsigned char);
void writedata(unsigned char);
void writenibble(unsigned char);
void con_al();
void al_ind();
void alarm_display();
void init_buz();

void time_display();
void set_min();
void set_hr();
void set_sec();


ISR(INT1_vect);

ISR(TIMER1_COMPA_vect);

static volatile int SEC =50;
static volatile int MIN =59;
static volatile int HOU =23;

static volatile int ALSEC = 0;
static volatile int ALMIN = 0;
static volatile int ALHOU = 0;



int al_stat=1;
int buz_stat=0;

int setmode=1;
int conmode=0;



int main(void)
{
	DDRC &= 0x00;
	

	EICRA = 0x08;
	EIMSK = 0x02;
	EIFR = 0x02;
	
	PCICR = 0x02;
	PCMSK1 = 0x0f;
	sei();
	
   
    TCCR1B |=(1<<CS12)|(1<<CS10)|(1<<WGM12);
    OCR1A=20150;
    sei();
    TIMSK1 |=(1<<OCIE1A);
  
  	init_lcd();
 
    char SHOWALSEC [2];
    char SHOWALMIN [2];
    char SHOWALHOU [2];
    
    stringout("Time : ");
	
	
	
    while(1)
    {
	
			init_backlit();
				
			if (al_stat==1)
			{
				
				if((ALHOU==HOU)&&(ALMIN==MIN)){
					init_buz();
					
				}else{
					
				}
		
			}
			
			else if(al_stat==0)
			{
				
					
				
			}
			
			if(conmode==0){
				
				if (al_stat==1)
				{
					
					moveto(1,11);
					
					
					
					}else{
					moveto(1,11);
					
					
					
				}
				
												
			}
			
			else if(conmode==1){
				
				
				moveto(1,0);
				stringout("change Time");
					
				
			}
			
			else if(conmode==2){
				
				
				alarm_display();
			
			}
	
			time_display();
      
    }
}

ISR(PCINT1_vect){
	
	if ((PINC & 0x01) == 0x01)
	{
		
		con_al();
		
		
	}else if ((PINC & 0x02) == 0x02)
	{
		
		con_mode();
		
	
	}else if ((PINC & 0x04) == 0x04){
		
		
		set_mode();
	
	}else if ((PINC & 0x08) == 0x08){
		
		
	
		if(conmode==1){
			if(setmode==0){
		
				set_sec();
			}
			else if(setmode==1){
				set_min();
			}
	
			else if(setmode==2){
		
				set_hr();
			}
		}
		else if(conmode==2){
			
			if(setmode==0){
				
				set_alsec();
			}
			else if(setmode==1){
				set_almin();
			}
			
			else if(setmode==2){
				
				set_alhr();
			}
		}

	}
	
}

void init_backlit(){
		
		DDRB |= 0x10 ;
			
		PORTB |= 0x10;
	
}



void init_lcd()
{
    /* ??? */                   // Set the DDR register bits for ports B and D
	DDRD|=0xF0;
	DDRB|=0x03;
    _delay_ms(15);              // Delay at least 15ms

	
	writecommand(0x03);
    /* ??? */                   // Use writenibble to send 0011
    _delay_ms(5);               // Delay at least 4msec
	writecommand(0x03);
    /* ??? */                   // Use writenibble to send 0011
    _delay_us(120);             // Delay at least 100usec

    /* ??? */                   // Use writenibble to send 0011, no delay needed
	writecommand(0x03);

	writecommand(0x02);
    /* ??? */                   // Use writenibble to send 0010
    _delay_ms(2);               // Delay at least 2ms
    
    writecommand(0x28);         // Function Set: 4-bit interface, 2 lines
	_delay_ms(2);
		writecommand(0x0f);         // Display and cursor on
	_delay_ms(25); 

	writecommand(0x01); 
	_delay_ms(25); 
	//writecommand(1);
}


void moveto(unsigned char row, unsigned char col)
{
    /* Don't need this routine for Lab 5 */
	if(row==0){
		
		writecommand(0x80+col);
	}
	if(row==1){
		
		writecommand(0xc0+col);
	}
	
}



void stringout(char *str)
{
    /* Don't need this routine for Lab 5 */
	do
	{
		
		writedata(*str);
		str++;
		
	 }while(*str!= '\0');
	
	
}

															/*
															  writecommand - Send the 8-bit byte "cmd" to the LCD command register
															*/
void writecommand(unsigned char cmd)
{
	unsigned char temp;

	PORTB&=~(0x01);
	temp=cmd&0xF0;
	writenibble(temp);
	temp=cmd&0x0F;
	temp=temp<<4;
	writenibble(temp);
	_delay_ms(3);

}



void writedata(unsigned char dat)
{
	unsigned char temp;

	PORTB|=0x01;
	temp=dat&0xF0;
	writenibble(temp);
	temp=dat&0x0F;
	temp=temp<<4;
	writenibble(temp);
	_delay_ms(3);

}

															/*
															  writenibble - Send four bits of the byte "lcdbits" to the LCD
															*/
void writenibble(unsigned char lcdbits)
{
	PORTD = lcdbits;//&0xF0;
	//PORTB |= 0x02;
	PORTB &= ~(0x02);
	PORTB |= 0x02;
	PORTB &= ~(0x02);
}


ISR(TIMER1_COMPA_vect)
{
	
	
    if (SEC<60)
    {
        SEC++;
    }
    if (SEC==60)
    {
        if (MIN<60)
        {
            MIN++;
        }
        SEC=0;
    }
    if (MIN==60)
    {
        if (HOU<24)
        {
            HOU++;
        }
        MIN=0;
    }
    if (HOU==24)
    {
		
        HOU=0;
    }
	
	

}


void init_buz(){
	
	DDRB |= 0x04;
	
	PORTB |= 0x04;
	
	_delay_ms(1000);
	
	PORTB &= ~(0x04);
	
	_delay_ms(1000);
	
	
}

void al_ind(int val){
	
	DDRB |= 0x08;
	
	if(val==1){
		PORTB |= 0x08;
	}
	
	if(val==0){
		
		PORTB &= ~(0x08);
		
	}
	
	
}

void con_al(){
				
		al_stat =~ al_stat;
		_delay_ms(500);

}

void con_mode(){
	
		
		if(conmode<3){
			conmode++;
			setmode=0;
			_delay_ms(500);
			writecommand(1);
			stringout("Time : ");
		}
		if(conmode==3){
			conmode=0;
			setmode=0;
			_delay_ms(500);
			writecommand(1);
			stringout("Time : ");
		}
	
}

void set_mode(){
	
	
		if(setmode<3){
			setmode++;
			
			_delay_ms(500);
		}
		if(setmode==3){
			setmode=0;
			
			_delay_ms(500);
		}
		
}

void set_sec()
{
	
		if(SEC<60){
			SEC++;
			_delay_ms(500);
		}
		if(SEC==60){
			SEC=0;
			MIN++;
			_delay_ms(500);
		}
	
}

void set_min()
{
		if(MIN<60){
			MIN++;
			_delay_ms(500);
		}
		if(MIN==60){
			MIN=0;
			HOU++;
			_delay_ms(500);
		}
		
}

void set_hr()
{
		if(HOU<24){
			HOU++;
			_delay_ms(500);
		}
		if(HOU==24){
			HOU=0;
			_delay_ms(500);
		}

}

void set_alsec()
{
	
	if(ALSEC<60){
		ALSEC++;
		_delay_ms(500);
	}
	if(ALSEC==60){
		ALSEC=0;
		ALMIN++;
		_delay_ms(500);
	}
	
}

void set_almin()
{
	if(ALMIN<60){
		ALMIN++;
		_delay_ms(500);
	}
	if(ALMIN==60){
		ALMIN=0;
		ALHOU++;
		_delay_ms(500);
	}
	
}

void set_alhr()
{
	if(ALHOU<24){
		ALHOU++;
		_delay_ms(500);
	}
	if(ALHOU==24){
		ALHOU=0;
		_delay_ms(500);
	}

}


void time_display(){
	
	char SHOWSEC [2];
	char SHOWMIN [2];
	char SHOWHOU [2];

	writecommand(0x80 + 7);
	
	itoa(HOU/10,SHOWHOU,10);
	stringout(SHOWHOU);
	itoa(HOU%10,SHOWHOU,10);
	stringout(SHOWHOU);
	stringout (":");
	writecommand(0x80 + 10);

	itoa(MIN/10,SHOWMIN,10);
	stringout(SHOWMIN);
	itoa(MIN%10,SHOWMIN,10);
	stringout(SHOWMIN);
	//writecommand(0x80 + 9);
	stringout (":");
	writecommand(0x80 + 13);
	
	itoa(SEC/10,SHOWSEC,10);
	stringout(SHOWSEC);
	itoa(SEC%10,SHOWSEC,10);
	stringout(SHOWSEC);
	
	writecommand(0x80 + 14);
	
	
}


void alarm_display(){
	
	char SHOWSEC [2];
	char SHOWMIN [2];
	char SHOWHOU [2];

	moveto(1,0);
	stringout("Alm : ");
	moveto(1,7);
	itoa(ALHOU/10,SHOWHOU,10);
	stringout(SHOWHOU);
	itoa(ALHOU%10,SHOWHOU,10);
	stringout(SHOWHOU);
	stringout (":");
	moveto(1,10);

	itoa(ALMIN/10,SHOWMIN,10);
	stringout(SHOWMIN);
	itoa(ALMIN%10,SHOWMIN,10);
	stringout(SHOWMIN);
	stringout (":");
	moveto(1,13);
	
	itoa(ALSEC/10,SHOWSEC,10);
	stringout(SHOWSEC);
	itoa(ALSEC%10,SHOWSEC,10);
	stringout(SHOWSEC);
	
	
	
	
}
