#include<avr/io.h>
#include<util/delay.h>
int main()
{
  DDRB=0b00100000;
  PORTB=0b00000000;
  while(1)
  {
    PORTB ^=1<<PORTB5; //^-EXOR
    _delay_ms(1000);   
  }
  return 0;  
}
