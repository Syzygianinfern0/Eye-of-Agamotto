#include <avr/io.h>
#include <avr/interrupt.h>
volatile int k = 0, kl = 0;
uint8_t pos[6][6] = {0, 0, 0, 0, 0, 0,
                     0, 0, 1, 1, 0, 0,
                     0, 1, 1, 1, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0};

ISR(TIMER0_OVF_vect)
{
  ++k;
}
ISR(TIMER2_OVF_vect)
{
  ++kl;
}

void dlay()
{
  k = 0;
  TCCR0B = 1 << CS00;
  while (k <= 1)
    ;
  TCCR0B = 0;
}
void dlay_long()
{
  kl = 0;
  TCCR2B = 1 << CS20 | 1 << CS22 | 1 << CS21;
  while (kl <= 30)
    ;
  TCCR2B = 0;
  Serial.println("dlay long over");
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
void init()
{
  Serial.begin(9600);
  TIMSK0 = 1 << TOIE0;
  TIMSK2 = 1 << TOIE2;
  DDRD = 0b11111100;
  DDRB = 0xFF;
  PORTB = 0xFF;
  PORTD = 0;
  sei();
}
int main(void)
{
  init();
  while (1)
  {
    for (int m = 0; m < 6; m++)
    {
      for (int l = 0; l < 6; l++)
      {
        pos[m][l] = 1;
      }
      disp();
      dlay_long();
      for (int l = 0; l < 6; l++)
      {
        pos[m][l] = 0;
      }
    }
  }
  return 0;
}
