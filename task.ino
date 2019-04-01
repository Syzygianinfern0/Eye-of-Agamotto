#include <avr/io.h>
#include <avr/interrupt.h>
#define X 0
#define Y 0
#define Z 0
volatile int k = 0;
volatile int last_x, last_y, last_z;
ISR(TIMER0_OVF_vect)
{
  ++k;
}

void dlay(float t)
{
  TCCR0B = (1 << CS02) | (1 << CS00);
  k = 0;
  while (k <= 10)       // Some delay thing
    ;
  TCCR0B = 0;
}
void init()             // Init fir timers and ADC
{
  TIMSK0 = 1 << TOIE0;
  ADMUX = (1 << REFS0); /* Vref: Avcc, ADC channel: 0 */
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

/*
  ADC Read function
*/
uint16_t ADC_Read(unsigned char channel)   // Reads from the channel passed. Anhy analog pin
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
  Serial.begin(9600);
  while (1)
  {
    x = y = z = 0;

    x = ADC_Read(0);
    y = ADC_Read(1);
    z = ADC_Read(2);

    Serial.print("x= ");
    Serial.print(x);
    Serial.print("  y= ");
    Serial.print(y);
    Serial.print("  z= ");
    Serial.println(z);
    dlay(0.01);
    if (x >= 265 && x <= 295 && y <= 360 && y >= 330 && z <= 365 && z >= 330)
    {
      Serial.println("IDLE");
    }
    else if (x >= 295 && x <= 330 && y <= 420 && y >= 379 && z <= 366 && z >= 330)
    {
      Serial.println("FWD");
    }
    else if (x >= 295 && x <= 330 && y <= 295 && y >= 260 && z <= 366 && z >= 335)
    {
      Serial.print("BACK");
    }
    else if (x >= 295 && x <= 330 && y >= 335 && y <= 367 && z >= 280 && z <= 315)
    {
      Serial.print("LEFT");
    }
    else if (x <= 315 && x >= 280 && y >= 335 && y <= 367 && z <= 425 && z >= 380)
    {
      Serial.print("RIGHT");
    }
  }
}