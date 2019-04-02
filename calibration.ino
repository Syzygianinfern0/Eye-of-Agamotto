#include <avr/iom328p.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile int k = 0;
ISR(TIMER0_OVF_vect)
{
  ++k;
}
void usart_write(String data)
{
  for (int i = 0; data[i] != '\0'; i++)
  {
    while (!(UCSR0A & (1 << UDRE0)))
      ;
    UDR0 = data[i];
  }
}

void dlay(float t)
{
  TCCR0B = (1 << CS02) | (1 << CS00);
  k = 0;
  while (k <= 10)
    ;
  TCCR0B = 0;
}
void init()
{
  TIMSK0 = 1 << TOIE0;
  ADMUX = (1 << REFS0); /* Vref: Avcc, ADC channel: 0 */
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  UCSR0A = 0X00;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  UBRR0 = 103;
}

/*
  ADC Read function
*/
uint16_t ADC_Read(unsigned char channel)
{
  ADMUX = (1 << REFS0) | (channel & 0x07); /* set input channel to read */
  ADCSRA |= (1 << ADSC);                   /* Start ADC conversion */
  while (!(ADCSRA & (1 << ADIF)))
    ;                    /* Wait until end of conversion by polling ADC interrupt flag */
  ADCSRA |= (1 << ADIF); /* Clear interrupt flag */
  return ADC;            /* Return ADC word */
}
int main()
{
  init();
  sei();
  int x, y, z;
  while (1)
  {
    x = y = z = 0;

    x = ADC_Read(0);
    y = ADC_Read(1);
    z = ADC_Read(2);
    usart_write("\nx= ");
    usart_write((String)x);
    usart_write("\ty= ");
    usart_write((String)y);
    usart_write("\tz= ");
    usart_write((String)z);
    dlay(0.01);
    if (x >= 265 && x <= 295 && y <= 360 && y >= 330 && z <= 365 && z >= 330)
    {
      usart_write("\nIDLE");
    }
    else if (x >= 295 && x <= 330 && y <= 420 && y >= 379 && z <= 366 && z >= 330)
    {
      usart_write("\nFWD");
    }
    else if (x >= 295 && x <= 330 && y <= 295 && y >= 260 && z <= 366 && z >= 335)
    {
      usart_write("\nBACK");
    }
    else if (x >= 295 && x <= 330 && y >= 335 && y <= 367 && z >= 280 && z <= 315)
    {
      usart_write("\nLEFT");
    }
    else if (x <= 315 && x >= 280 && y >= 335 && y <= 367 && z <= 425 && z >= 380)
    {
      usart_write("\nRIGHT");
    }
  }
}