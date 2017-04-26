/* Pump_Stepper
 * 2015 by Steven Lammers
 *
 * This file is part of the Arduino SyringPump Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SyringePump Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef Pump_Stepper_H
#define Pump_Stepper_H
                                                               /* DEFINITIONS */
#define PUMP_STEPPER_VERSION "1.0"
// ------- Preamble -------- //
/* These parameters will depend on your motor, what it's driving */
#define PRESCALE      		1024 // Prescale for the CPU Clock
#define CLKFREQ        	 	16 // Mhz
#define MAX_FREQ       		800 //260 /* determines min startup speed */
#define MIN_FREQ       		100 //117 /* determines max cruise speed */
#define ACCELERATION     	3 //16  /* lower = smoother but slower accel */
#define STEPS_PER_REV 		3200 // based on 200*microsteps/step
#define RETRACT_DELAY 		0 // about 5x default extrude speed

#define RAMP_STEPS    (MaxDelay - MinDelay) / ACCELERATION
                                                                  /* INCLUDES */
#include <PID_v1.h>
#include "Arduino.h"
                                                          /* CLASS DEFINITION */
class Pump_Stepper
{
  int dirPin;                              /* pin for stepper drive direction */
  int stepPin;                                  /* pin for stepper drive step */
  volatile int * pStepPin; /* pointer to step pin in ISR in main program loop */
  volatile uint16_t* pStepCounter;
  uint16_t MaxDelay;                              /* determines max cruise speed */
  uint16_t MinDelay;                             /* determines min startup speed */
  uint16_t ExtrudeDelay;                           /* determines extrusion speed */

  public:
  /* Constructors */
  Pump_Stepper(int, int, volatile int *, volatile uint16_t *);

  void enable(void);       /* Enable extruder for printing */
  
  /* Initialize the timer used to generate the step signal */
  void initTimer(void);

  /* Get Set methods */
  uint16_t get_mindelay(void);
  uint16_t get_maxdelay(void);

  /* Methods for movement of extruder */
  void goforward(void);               /* Set direction to extrude */
  void gobackward(void);              /* Set direction to suck */
  void set_extrudeDelay(uint16_t);    /* Set speed of the timer delay for extrusion*/
  void set_speed(uint16_t);           /* Set speed of the timer delay */
  void fastforward(float);            /* Fast move in extrude direction */
  void fastbackward(float);           /* Fast move to let pressure off extrusion */
  void continuous_extrude(void);      /* Extrude continuously */
  void fastretract(void);			  /* Retract the Pump*/
  void resume_extrude(void);          /* If paused and plunger retracted this will resume extrusion */
  void pause_extrude(void);           /* Stops extrusion and backs off plunger pressure */
  void stop_extrude(void);            /* Full stop of extrusion by turning off TIMER1 */
  void take_steps(uint32_t, uint16_t);/* Take an exact number of microsteps at a given rate */
  void trapezoidMove(int);            /* Accelerate and decelerate extrusion based on number of steps */
};
#endif                                       /* Close out define TempDS18B20.h */
