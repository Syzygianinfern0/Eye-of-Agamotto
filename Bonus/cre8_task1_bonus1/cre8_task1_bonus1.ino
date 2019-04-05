#include <avr/io.h>
#include <avr/interrupt.h>
volatile int k0 = 0, flag = 0, k2 = 0;
char direction = 'i', prev_direction = 'r', game_status = 'r';
int h[2] = {2, 2}, food[2] = {4, 4};
int counter = 0;
uint8_t pos[6][6] = {0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     1, 1, 0, 0, 0, 0};
// | - origin
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
  while (k0 <= 2)
    ;
  TCCR0B = 0;
}

void dlay_long()
{
  TCCR2B = (1 << CS22) | (1 << CS20) | (1<<CS20);
  k2 = 0;
  while (k2 <= 70)
    ;
  TCCR2B = 0;
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
  //usart_write("\n");
  for (int i = 5; i >= 0; --i)
  {
    for (int j = 0; j < 6; j++)
    {
      if (pos[i][j] == 1)
      {
        PORTD = 0;
        PORTB = 0xFF;
        PORTD = 1 << (j + 2);
        PORTB = 0xFF ^ (1 << i);
        dlay();
      }
      //usart_write((String)pos[i][j]);
    }
    //usart_write("\n");
  }
}
void disp_xy()
{

  // pos[0][h[0]] = 1;
  // pos[1][h[1]] = 1;
  k2 = 0;
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  k2 = 0;
  while (k2 <= 120)
  {
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 6; j++)
      {
        pos[i][j] = 0;
      }
    }

    pos[h[0]][h[1]] = 1;
    disp();

    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 6; j++)
      {
        pos[i][j] = 0;
      }
    }

    pos[food[0]][food[1]] = 1;
    disp();
  }
  TCCR2B = 0;
}
void ran()
{
  while (food[0] == h[0] && food[1] == h[1])
  {
    food[0] = random(0, 6);
    food[1] = random(0, 6);
  }
}
int main()
{
  counter++;
  init();
  sei();
  int x, y, z;
  while (1)
  {
    ran();
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 6; j++)
      {
        pos[i][j] = 0;
      }
    }

    if ((h[0] == 6) || (h[1] == 6) || (h[0] == -1) || (h[1] == -1))
    {
      usart_write("Game over!!");
      game_status = 'n';
      break;
    }

    disp_xy();

    usart_write((String)counter);
    // usart_write("Head : ");
    // usart_write((String)h[0]);
    // usart_write((String)h[1]);
    // usart_write("  direction : ");
    // usart_write((String)direction);
    // usart_write("\n");
    // _delay_ms(3000);

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

    if (x >= 260 && x <= 285 && y <= 385 && y >= 315 && z <= 385 && z >= 315)
    { // 270, 400, 350
      usart_write("IDLE");
      direction = 'i';
    }
    else if (x >= 285 && x <= 345 && y <= 420 && y >= 380 && z <= 385 && z >= 315)
    { // 330, 445, 400
      usart_write("UP");
      direction = 'u';
    }
    else if (x <= 315 && x >= 270 && y <= 330 && y >= 290 && z <= 385 && z >= 315)
    {
      usart_write("DOWN");
      direction = 'd';
    }
    else if (x <= 330 && x >= 295 && y <= 365 && y >= 335 && z >= 290 && z <= 315) // 360, 400, 340
    {
      usart_write("LEFT");
      direction = 'l';
    }
    else if (x <= 300 && x >= 270 && y <= 370 && y >= 340 && z <= 415 && z >= 375) // 330, 380, 450
    {
      usart_write("RIGHT");
      direction = 'r';
    }
    else
    {
      usart_write("IDLE");
      direction = 'i';
    }

    // if(counter == 2){
    //   direction = 'u';
    // }

    if (direction == 'i')
    {
      switch (prev_direction)
      {
      case 'r':
        ++h[1];
        break;
      case 'l':
        --h[1];
        break;
      case 'd':
        --h[0];
        break;
      case 'u':
        ++h[0];
        break;
      default:
        break;
      }
      // disp_xy();
    }

    if (direction != 'i')
    {
      prev_direction = direction;
    }
  }
  while (game_status != 'r')
  { 
    usart_write("Game over");
    // for (int i = 0; i < 6; ++i)
    //   for (int j = 0; j < 6; ++j)
    //     pos[i][j] = 1;
    // disp();
    PORTB = 0;
    PORTD = 0xFF;
    dlay_long();
    PORTD = 0;
    dlay_long();
  }
}
