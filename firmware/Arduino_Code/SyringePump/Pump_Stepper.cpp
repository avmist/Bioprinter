
#include "Pump_Stepper.h"

/* Constructors for TempDS18B20 class */

Pump_Stepper::Pump_Stepper(int tmpDirPin, int tmpStepPin, volatile int* ptmpStepPin, volatile uint16_t* ptmpStepCounter){

  dirPin   = tmpDirPin;                                               /* Digital Pin for direction signal */
  stepPin  = tmpStepPin;                                              /* Digital Pin for direction signal */
  pStepPin = ptmpStepPin;                      /* Pointer to step pin used in main program loop timer ISR */
  pStepCounter = ptmpStepCounter;
  MaxDelay = static_cast<uint16_t>(float(CLKFREQ)*1000*1000/(float(MIN_FREQ)*float(PRESCALE)));  /*(156.3) determines max cruise speed */
  MinDelay = static_cast<uint16_t>(float(CLKFREQ)*1000*1000/(float(MAX_FREQ)*float(PRESCALE))); /*(19.5) determines min startup speed */
  ExtrudeDelay = 165; // OCR1A value calculated in Syringe_Gear_Pump.m (matlab)
  
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
}

void Pump_Stepper::enable(void)
{
  /* Set the stepPin in main loop for ISR */
  *pStepPin = stepPin;

  /* Run initialization to set stepper speed etc... */
  initTimer();
}


/* Initialize the timer used to generate the step signal */
void Pump_Stepper::initTimer(void)
{
  cli();                                                   /* stop interrupts */

  TCCR1A = 0;                                 /* Clear entire TCCR1A register */
  TCCR1B = 0;                                 /* Clear entire TCCR1B register */
  TCNT1  = 0;                                /* Initialize counter value to 0 */
  TCCR1B |= (1 << WGM12);                                         /* CTC Mode */
  TCCR1B |= (1 << CS10)|(1 << CS12);                          /* Set CS bits for 
                                                   1024 prescaler CPU clock/1024
                                                        7.81Hz max, 30 Hz min */
  OCR1A = MaxDelay;                          /* Set default speed as slowest */

  sei();                                          /* enable global interrupts */
                      /* Notice we haven't set the timer2 interrupt flag yet. */
}

uint16_t Pump_Stepper::get_mindelay(void)
{
  return MinDelay;
}

uint16_t Pump_Stepper::get_maxdelay(void)
{
  return MinDelay;
}

/* Methods for movement of extruder */
void Pump_Stepper::goforward()
{
  digitalWrite(dirPin, LOW);
  //delay(100);
}

void Pump_Stepper::gobackward()
{
  digitalWrite(dirPin, HIGH);
  //delay(100);
}

void Pump_Stepper::set_extrudeDelay(uint16_t new_delay)
{
  ExtrudeDelay = new_delay; // OCR1A value calculated as avg 165 in Syringe_Gear_Pump.m (matlab)
}

void Pump_Stepper::set_speed(uint16_t new_speed)
{
  TCNT1  = 0;                                /* Initialize counter value to 0 */
  OCR1A = new_speed;                 /* Delay in the Counter Compare Register */
}

void Pump_Stepper::fastforward(float numRevs)
{
  goforward();
  take_steps(long(STEPS_PER_REV*numRevs), 2);
}

void Pump_Stepper::fastbackward(float numRevs)
{
  gobackward();
  take_steps(long(STEPS_PER_REV*numRevs), 2);
}

void Pump_Stepper::continuous_extrude(void)
{
  TIMSK1 &= ~(1 << OCIE1A);                                  /* Turn back off */
  goforward();
  set_speed(ExtrudeDelay);
  TIMSK1 |= (1 << OCIE1A);                    /* Turn on interrupts, stepping */
}

void Pump_Stepper::fastretract(void)
{
	TIMSK1 &= ~(1 << OCIE1A);                                  /* Turn back off */
	gobackward();
	set_speed( RETRACT_DELAY );
	TIMSK1 |= (1 << OCIE1A);                    /* Turn on interrupts, stepping */
}

void Pump_Stepper::resume_extrude(void)
{
  fastforward(0.25);
  continuous_extrude();
}
void Pump_Stepper::pause_extrude(void)
{
  stop_extrude();
  fastbackward(0.25);
}

void Pump_Stepper::stop_extrude(void)
{
  TIMSK1 &= ~(1 << OCIE1A);                                  /* Turn back off */
}

void Pump_Stepper::take_steps(uint32_t howManySteps, uint16_t delayTime)
{
  set_speed(delayTime);
  *pStepCounter = 0;                 /* Initialize to zero steps taken so far */
  TIMSK1 |= (1 << OCIE1A);                    /* Turn on interrupts, stepping */
  while(*pStepCounter <= howManySteps)
  { 
    /* Wait */
  }
  TIMSK1 &= ~(1 << OCIE1A);                                  /* Turn back off */
}


void Pump_Stepper::trapezoidMove(int howManySteps)
{
  float stepsTaken = 0;
  float delayTime  = MaxDelay;

  // set direction and ensure howManySteps is non-negative
  if(howManySteps > 0){
    goforward();
  } 
  else {
    gobackward();
    howManySteps = -howManySteps; 
  }

  // If there are enough steps: ramp up, cruise, ramp down
  if (howManySteps > 2*RAMP_STEPS)
  {
                                    /* Have enough steps for a full trapezoid */
                                                                /* Accelerate */
    Serial.println("Accelerate");
    while(stepsTaken < RAMP_STEPS)
    {
      take_steps(1,delayTime);
      delayTime -= ACCELERATION;
      stepsTaken++;
    }

                                                                    /* Cruise */
    //Serial.println("Cruise");
    delayTime = MinDelay;
    take_steps((howManySteps - 2*RAMP_STEPS), delayTime);
    stepsTaken += (howManySteps - 2*RAMP_STEPS);
    
                                                                /* Decelerate */
    
    Serial.println("Decelerate");
    while(stepsTaken < howManySteps)
    {
      take_steps(1, delayTime);
      delayTime += ACCELERATION;
      stepsTaken++;
    }
    
  }
  
  
  else 
  {
                                                      /* Partial ramp up/down */
    while(stepsTaken <= howManySteps / 2)
    {
                                                                   /* Ramp up */
      take_steps(1,delayTime);
      delayTime -= ACCELERATION;
      stepsTaken++;
    }
    delayTime += ACCELERATION;
    while(stepsTaken < howManySteps)
    {
                                                                 /* Ramp down */
      take_steps(1,delayTime);
      delayTime += ACCELERATION;
      stepsTaken++;
    }
  
  }
  

}



