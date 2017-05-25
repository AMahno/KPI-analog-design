#include <avr/io.h>
#include <avr/delay.h>

unsigned volatile char toggle = 0;   
void beep(unsigned int compare);
int findClosest(float input);
int skipPause = 0;

const unsigned int notes[] = {1915, 1706, 1519, 1432, 1275, 1136, 1012,
			 956, 851, 758, 716, 637, 568, 506,
			 478, 425, 379, 358, 318, 290, 253,
			 239};

int main(void){
    
    DDRA |= (1 << PA7);     //output on PA7
	DDRB |= (1 << PB0) | (1 << PB1);
    TCCR1A = 0;             //CTC mode
    //CTC mode, clk src = clk/8, start timer
    TCCR1B = (1 << WGM12) | (1 << CS10);
	TCCR0B = (1 << CS02); //Timer0, clk/256
	
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);
	// ADEN: Set to turn on ADC , by default it is turned off 
	//ADPS2: ADPS2 and ADPS0 set to make division factor 32 
	ADMUX=0; // ADC input channel set to ADC0
	unsigned int adcValue;
		
	while(1){
		ADCSRA |= (1<<ADSC); // Start conversion
        while (ADCSRA & (1<<ADSC)); // wait for conversion to complete
		adcValue = 1024-ADC; //Store ADC value
		
		int midNote = notes[findClosest(adcValue * 1.63)];
		
		beep(midNote);
		int i;
		if(PINA & (1 << 1)){
			skipPause = 1;
			PORTB |=  (1 << PB0);
			PORTB &= ~(1 << PB1);
		}
		else{
			if(PINA & (1 << 2)){
			skipPause = 1;
			PORTB |=  (1 << PB1);
			PORTB &= ~(1 << PB0);
			}else skipPause = 0;
		}
		if(!skipPause){ 
			for(i = 0; !(i > 500 || PINA & (1 << 1) || PINA & (1 << 2)); i++)
				_delay_ms(1);
				PORTB &= ~(1 << PB0);
				PORTB &= ~(1 << PB1);
		}
		else _delay_ms(100);
	}
}


void beep(unsigned int compare){
	OCR1A = compare;
	TCNT1 = 0;
	TCNT0 = 0;
	while(TCNT0 <= 250){
		if (TIFR1 & (1 << OCF1A)) {  //count reached?
			TIFR1 |= (1 << OCF1A);   //clear flag
			if (toggle) {           //toggle output
				toggle = 0;
				PORTA &= ~(1 << PA7);
			}
			else {
				toggle = 1;
				PORTA |=  (1 << PA7);
			}
		}
	}
	PORTA &= ~(1 << PA7); //set output to 0
}

int findClosest(float input){
	int intInput = (int)input; //in case it goes wrong
	int i;
	for(i = 0; i < 21; i++){
		if(intInput > notes[i]) break;
	}
	return i;
}

/*	
		
		beep(1915); //до
		beep(1706); //ре
		beep(1519); //ми
		beep(1432);	//фа
		beep(1275);	//соль
		beep(1136);	//ля
		beep(1012);	//си
		
		beep(956); 	//до (2)
		beep(851); //ре
		beep(758); //ми
		beep(716);	//фа
		beep(637);	//соль
		beep(568);	//ля
		beep(506);	//си
		
		beep(478); //до (3)
		beep(425); //ре
		beep(379); //ми
		beep(358);	//фа
		beep(318);	//соль
		beep(290);	//ля
		beep(253);	//си
		
		beep(239); 	//до (4)
		*/