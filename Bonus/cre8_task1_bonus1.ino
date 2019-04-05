#include <avr/io.h>
#include <avr/interrupt.h>
volatile int k0 = 0, flag = 0, k2 = 0;
char direction = 'r', prev_direction = 'r', game_status = 'r';
int h[2] = {1, 1};
uint8_t pos[6][6] = {0, 0, 0, 0, 0, 0,
                     0, 0, 1, 1, 0, 0,
                     0, 1, 1, 1, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0};
ISR(TIMER2_OVF_vect)
{
  ++k2;
}
ISR(TIMER0_OVF_vect)
{
  ++k0;
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

void dlay()
{
  TCCR0B = (1 << CS02) | (1 << CS00);
  k0 = 0;
  while (k0 <= 10)
    ;
  TCCR0B = 0;
}
void init()
{
  DDRD = 0b11111100;
  DDRB = 0xFF;
  PORTB = 0xFF;
  PORTD = 0;
  TIMSK0 = 1 << TOIE0;
  TIMSK2 = 1 << TOIE2;
  TCCR2B = 1 << CS21;

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
void disp()
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      if (pos[i][j] == 1)
      {
        PORTD = 1 << (2 + j);
        PORTB = 0xFF ^ (1 << i);
        dlay();
        PORTD = 0;
        PORTB = 0xFF;
        dlay();
      }
    }
  }
}
void disp_xy()
{

  pos[0][h[0]] = 1;
  pos[1][h[1]] = 1;
  disp();
}
int main()
{
  init();
  sei();
  int x, y, z;
  while (1)
  {
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 6; j++)
      {
        pos[i][j] = 0;
      }
    }
    x = y = z = 0;
    while (game_status != 'r')
    {
    }
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
    if (x >= 260 && x <= 285 && y <= 385 && y >= 315 && z <= 385 && z >= 315)
    { // 270, 400, 350
      direction = 'i';
    }
    else if (x >= 285 && x <= 345 && y <= 420 && y >= 380 && z <= 385 && z >= 315)
    { // 330, 445, 400
      direction = 'u';
    }
    else if (x <= 315 && x >= 280 && y <= 330 && y >= 290 && z <= 385 && z >= 315)
    { // 340, 340, 400
      direction = 'd';
    }
    else if (x <= 330 && x >= 300 && y <= 365 && y >= 335 && z >= 290 && z <= 315) // 360, 400, 340
    {
      direction = 'l';
    }
    else if (x <= 310 && x >= 280 && y <= 370 && y >= 345 && z <= 425 && z >= 395) // 330, 380, 450
    {
      direction = 'r';
    }
    else
    {
      direction = 'i'
    }

    if (direction == 'i')
    {
      if ((h[0] == 6) || (h[1] == 6) || (h[0] == -1) || (h[1] == -1))
      {
        usart_write("Game over!!");
        game_status = 'n';
        break;
      }
      else
      {
        switch (prev_direction)
        {
        case 'r':
          ++h[0];
          break;
        case 'l':
          --h[0];
          break;
        case 'd':
          ++h[1];
          break;
        case 'u':
          --h[1];
          break;
        default:
          break;
        }
        disp_xy();
      }
    }
  }
}
