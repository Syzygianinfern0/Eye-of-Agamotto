#include <avr/io.h>
#include <avr/interrupt.h>
volatile int k = 0, brightness = 0;
ISR(TIMER2_OVF_vect)
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

void dlay(void)
{
  TCCR2B = (1 << CS22) | (1 << CS20);
  k = 0;
  while (k <= 10);
  TCCR2B = 0;
}
void init()
{
  TIMSK2 = 1 << TOIE2;
  ADMUX = (1 << REFS0); /* Vref: Avcc, ADC channel: 0 */
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  UCSR0A = 0X00;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  UBRR0 = 103;
  DDRD = 0b01000000;
  TCCR0A = 1 << WGM00 | 1 << WGM01 | 1 << COM0A1;
  TCCR0B = 0x01;
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
    usart_write((String)brightness);
    OCR0A = brightness;
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
    dlay();
    if (y <= 380 && y >= 330)
    {
      usart_write("\nIDLE");
    }
    else if (y <= 420 && y >= 379)
    {
      usart_write("\nINCREASE");
      brightness += 10;
      if (brightness > 255)
        brightness = 255;
    }
    else if (y <= 329 && y >= 290)
    {
      usart_write("\nDECREASE");
      brightness -= 10;
      if (brightness < 0)
        brightness = 0;
    }
  }
}
