/**************************
  Control the pump rate, acceleration and temperature controls for a syringe pump.
  The system works on a latching control where an external signal turns the pump
  on and off. When the pump is initially turned on, it will accelerate up to the 
  dispense rate. When the pump is turned off, it will not decelerate but will come
  to a complete stop.

  Stepper motor control is controlled by Timer1 ISR. Timer1 controls the servo() library
  in Arduino, so be aware that the servo()library will not function properly in this 
  program. So don't use that servo here!
  
  Eventually the pump rate and on/off signal will be set by commands sent over SPI.
***************************/

// IMPORTANT: Need to turn on BIO_SPI_SLAVE
// in order to include the correct headers and declarations
#define BIO_SPI_SLAVE 1


// ------- Flags ----------- //
#define ENABLE_PID_LOOP_G_CODE_CONTROL 0

// ------- Preamble -------- //
/* These parameters will depend on your motor, what it's driving */
#define SYRINGE_HEATER_PIN  6  // PWM pin for MOSFET control via PID
#define TUBE_HEATER_PIN     5  // PWM pin for MOSFET control via PID
#define BIO_INTERRUPT_PIN   2  // Pin for hardware interrupt signaling bioprint state
#define BIO_INTERRUPT_NUM   0  // Pin for hardware interrupt signaling bioprint state
#define STEPPER_DIR_PN      4  // Pin for setting extruder direction
#define STEPPER_STEP_PIN    8  //5//3// Pin for setting extruder steps to motor controller (any digital pin, does not need PWM)
#define MAX_TUBE_TEMP       45 // Max temperature for tube heater, so it does not melt
#if BP_CURRENT >= BP_ENDSTOP
  #define NEEDLE_HEATER_PIN   9  // PWM pin for MOSFET control via PID
  #define ENDSTOP_FULL_PIN    A0
  #define ISR_ENSTOP_PIN      3
#else
  #define NEEDLE_HEATER_PIN   3  // PWM pin for MOSFET control via PID
#endif

#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <bioSPI.h>
#include "Temp_DS18B20.h"
#include "Pump_Stepper.h"
#include "Configuration.h"

// -------- Global Variables --------- //
volatile boolean EndStopReached = TRUE; // At boot this will be lowered if not reached
volatile boolean EndStopFull;
volatile int stepPin = 0; // 3
volatile uint16_t stepCounter = 0;
Pump_Stepper extruder1(STEPPER_DIR_PN, STEPPER_STEP_PIN, &stepPin, &stepCounter);
const int extruderPollDelay_ms = 1000; //5200;
int extruderPollLast_ms  = millis();
bool extruderDirFLAG = 1;
// Set up temperature probes 
Temp_DS18B20 tempProbe_0;
Temp_DS18B20 tempProbe_1;
Temp_DS18B20 tempProbe_2;
const int tempPollDelay_ms = 1000;
float currentTemp = 0.0; // Stores current temperature reading
int tempPollLast_ms = millis();
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// PID variables
double pid_setpoint_syringe, pid_input_syringe;
double pid_setpoint_tube, pid_input_tube;
double pid_setpoint_needle, pid_input_needle;
double pid_output_syringe = 40.0;                                          /* Initial setpoint for syringe PID */
double pid_output_tube = 20;                                               /* Initial setpoint for tube PID */
double pid_output_needle = 230.0;                                          /* Initial setpoint for needle PID */
// First try, way too high on P
// PID syringePID(&pid_input_syringe, &pid_output_syringe, &pid_setpoint_syringe,2,5,1, DIRECT);
// Second try
PID syringePID(&pid_input_syringe, &pid_output_syringe, &pid_setpoint_syringe,0.2,0.5,0.1, DIRECT);
PID tubePID(&pid_input_tube, &pid_output_tube, &pid_setpoint_tube,1.0,0.5,0.15, DIRECT); //.4,.5,.15
PID needlePID(&pid_input_needle, &pid_output_needle, &pid_setpoint_needle,0.4,0.5,0.15, DIRECT);
// Interrupt to trigger bioprint
volatile bool bioprint_FLAG = false;
bool bioprint_LATCH = false; //latch on for bioprint
bool TempPID_LATCH = false;
bool TempPID_RESET = false;
// ------------ Functions ------------ //


ISR(TIMER1_COMPA_vect)
{
  digitalWrite(stepPin, LOW);  /* This LOW to HIGH change is what creates the */
  digitalWrite(stepPin, HIGH); /* "Rising Edge" so the easydriver knows to when 
                                                                     to step. */
  stepCounter++;                                         /* Count Steps Taken */
}

void initDallasTemp(void)
{

  // Begin sensors on One-Wire
  sensors.begin();

  // Create tempProbe_0 Object
  // For the Syringe
  tempProbe_0.setDallasTemperature(&sensors);
  //DeviceAddress tmpAddress = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};
  //uint8_t tmpAddress[]  = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};
  //SL uint8_t tmpAddress[]  = {0x28, 0xED, 0x71, 0xDE, 0x06, 0x00, 0x00, 0x18};
  //uint8_t tmpAddress[]  = {0x28, 0xAB, 0xF7, 0xDD, 0x06, 0x00, 0x00, 0x00};
  //uint8_t tmpAddress[]  = {0x28, 0xB7, 0x74, 0xDD, 0x06, 0x00, 0x00, 0xA3};
  //uint8_t tmpAddress[]  = {0x28, 0x26, 0x22, 0x20, 0x06, 0x00, 0x00, 0xC0};
  tempProbe_0.setTempAddress(tmpAddressSyringe);
  //tempProbe_0.requestTemperatures();

  // For the Tube
  tempProbe_1.setDallasTemperature(&sensors);
  //uint8_t tmpAddressTube[] = {0x28, 0xC2, 0x92, 0x20, 0x06, 0x00, 0x00, 0x7A};
  //uint8_t tmpAddressTube[] = {0x28, 0x55, 0x67, 0x21, 0x06, 0x00, 0x00, 0xB5};
  //uint8_t tmpAddressTube[] = {0x28, 0x6B, 0x7A, 0xD7, 0x06, 0x00, 0x00, 0xA3};
  //SL uint8_t tmpAddressTube[] = {0x28, 0x11, 0xA2, 0x94, 0x06, 0x00, 0x00, 0xFC};
  //uint8_t tmpAddressTube[] = {0x28, 0x11, 0xA2, 0x94, 0x06, 0x00, 0x00, 0xFC};
  //uint8_t tmpAddressTube[] = {0x28, 0x2B, 0xFA, 0xDD, 0x06, 0x00, 0x00, 0x06};
  //uint8_t tmpAddressTube[] = {0x28, 0xFC, 0x2E, 0xD6, 0x06, 0x00, 0x00, 0x92};
  tempProbe_1.setTempAddress(tmpAddressTube);

  //tempProbe_1.requestTemperatures();

  // For the Needle
  tempProbe_2.setDallasTemperature(&sensors);
  //uint8_t tmpAddressNeedle[] = {0x28, 0x68, 0x1C, 0x31, 0x02, 0x00, 0x00, 0xA9};
  //uint8_t tmpAddressNeedle[] = {0x28, 0x69, 0x96, 0xDE, 0x06, 0x00, 0x00, 0xA5};
  // SL uint8_t tmpAddressNeedle[] = {0x28, 0x66, 0x83, 0xDD, 0x06, 0x00, 0x00, 0xA7};
  //uint8_t tmpAddressNeedle[] = {0x28, 0x4A, 0xDD, 0xDD, 0x06, 0x00, 0x00, 0x5B}; 
  //uint8_t tmpAddressNeedle[] = {0x28, 0x94, 0x96, 0x94, 0x06, 0x00, 0x00, 0x66};
  //uint8_t tmpAddressNeedle[] = {0x28, 0xD6, 0xC4, 0x55, 0x07, 0x00, 0x00, 0x76};
  tempProbe_2.setTempAddress(tmpAddressNeedle);
  //tempProbe_2.requestTemperatures();
  

  tempProbe_0.requestTemperatures();

  delay(1000);
}

void bioprint_interrupt(void)
{
  bool pinVal = digitalRead(BIO_INTERRUPT_PIN);
  if   (pinVal){bioprint_FLAG = true;}
  else {bioprint_FLAG = false;}
} 

#if BP_CURRENT >= BP_ENDSTOP
//----------------------------------------------//
//  ISREndStopReached
//
//  Description: This function is an ISR 
//  activated when either of the endstops
//  have been triggered. This will check
//  to determine which pin was set
//  after it has stopped the extruder.
//
//----------------------------------------------//
void ISREndStopReached(void)
{
  extruder1.stop_extrude();
  EndStopReached = TRUE;
  
  // Determine which endstop was activated
  if( !digitalRead( ENDSTOP_FULL_PIN ) )
  {
    // It was the End stop full pin
    EndStopFull = FALSE;    
  }
  else
  {
    // It was the End stop empty pin
    EndStopFull = TRUE;
  }  

  return;
}
#endif


// -------------Setup ---------------- //
void setup() 
{
  // Initialize stepper motor for syring extrusion
  //pinMode(STEPPIN, OUTPUT);
  //initTimer();
  
  // Enable extruder
  extruder1.enable();
  
  // Initialize serial communications
  Serial.begin(9600);  
    
  // Initialize temperature probes
  initDallasTemp();

  // Initialize PID
  pinMode(SYRINGE_HEATER_PIN, OUTPUT);
  pid_setpoint_syringe = 38; // deg Celcius
  pinMode(TUBE_HEATER_PIN, OUTPUT);
  pid_setpoint_tube = 38; // deg Celcius
  pinMode(NEEDLE_HEATER_PIN, OUTPUT);
  pid_setpoint_needle = 38; // deg Celcius

  syringePID.SetMode(AUTOMATIC);
  tubePID.SetMode(AUTOMATIC);
  needlePID.SetMode(AUTOMATIC);
   
  // Initialize SPI communications
  slaveInitSPI();

  // Initialize Interrupt
  //attachInterrupt(digitalPinToInterrupt(BIO_INTERRUPT_PIN), bioprint_interrupt, CHANGE);
  //attachInterrupt(BIO_INTERRUPT_NUM, bioprint_interrupt, CHANGE);

#if BP_CURRENT >= BP_ENDSTOP
  // ISR Endstop
  pinMode(ISR_ENSTOP_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ISR_ENSTOP_PIN), ISREndStopReached, FALLING);

  // Check if the Endstop is being pressed
  if(!digitalRead(ISR_ENSTOP_PIN))
  {
    // ###AJK Should be able to just call the func to set bool(s)
    ISREndStopReached();
  }
#endif

  TempPID_LATCH = false;
  TempPID_RESET = false;

  //Serial.println("Setup Complete");
  //Serial.println("");
}

// --------------- Main -------------- //
void loop()
{  
  /* STEPPER CONTROLS */  
  /* Poll SS and if it goes Low there is an incomming message to deal with */

// ###AJK Need to add code to deal with EndStopReached
//
// Will want a function to pull syringe from empty to full quickly
// Will want a function that relays info to Master

// May also want to consider the below
//
// May want to consider halting cmd accepting 
//   (How to do this: Maybe ISR to Master, want to avoid hangs from not accepting code)
//   (Or maybe let master hang, and let slave continue after it is refilled ??? add button to slave to reset the extruder ???)
// May want to consider killing certain tasks and saving the state off, such as UV light ???
// May also just let it be, extruder should be full, let Master hang so it doesn't continue

// Check if the Endstop is being pressed
#if BP_CURRENT >= BP_ENDSTOP
  if(digitalRead(ISR_ENSTOP_PIN))
  {
    // Not being pressed. Reset Endstop Halt
    EndStopReached = FALSE;
  }
#endif

  // If SS goes low, there is an incomming transmission
  if (!digitalRead(SS)) {

    //Serial.println("Slave Enabled");
    Serial.println("SPI SS triggered: ");
    
    // Get byte from SPI  
    uint8_t msg_byte = 
        RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE ); 

    Serial.print("SPI recieved: ");
    Serial.println(msg_byte);

    if(msg_byte == SPI_EXTRUDER_START)
    {
    #if BP_CURRENT >= BP_ENDSTOP
      if( EndStopReached && !EndStopFull )
      {
        Serial.println("Can't Start Extruder, it is Empty"); 
      }
      else
    #endif
      {
        //bioprint_FLAG = 1;
        // Go forward 
        Serial.println("Start Extrude"); 
        extruder1.goforward();                   // Set the direction
        extruder1.continuous_extrude(); 
      }
    }
    else if(msg_byte == SPI_EXTRUDER_STOP) 
    {
      //bioprint_FLAG = 0;
      // Stop extrude
      Serial.println("Stop Extrude"); 
      extruder1.stop_extrude();
    }
    else if (msg_byte == SPI_EXTRUDER_RETRACT)
    {
    #if BP_CURRENT >= BP_ENDSTOP
      if( EndStopReached && EndStopFull )
      {
        Serial.println("Can't Retract Extruder, it is Full"); 
      }
      else
    #endif
      {
        Serial.println("Retract Extrude"); 
        // Reverse Extruder until pulled back all the way
        extruder1.fastretract();   
      }
    }
    else if(msg_byte == SPI_START_PID) 
    {
      TempPID_LATCH = true;
      Serial.println("Start PID Loop"); 
    }
    else if(msg_byte == SPI_STOP_RESET_PID) 
    {
      TempPID_LATCH = false;
      TempPID_RESET = true;
      Serial.println("Stop and Reset PID Loop"); 
    }
    else if(msg_byte == SPI_TEMP_SYRINGE) 
    {      
      // Write msg_byte^ACK
      RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
      
      // Read in new data from Master
      msg_byte = RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );

      pid_setpoint_syringe = msg_byte;
      Serial.println("Change Syringe Temp Set: "); 
      Serial.print(pid_setpoint_syringe);
    }
    else if(msg_byte == SPI_TEMP_TUBE) 
    { 
      // Write msg_byte^ACK
      RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
      
      // Read in new data from Master
      msg_byte = RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );

      pid_setpoint_tube = msg_byte;
      Serial.println("Change Tube Temp Set: "); 
      Serial.print(pid_setpoint_tube);
    }
    else if(msg_byte == SPI_TEMP_NEEDLE) 
    { 
      // Write msg_byte^ACK
      RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
      
      // Read in new data from Master
      msg_byte = RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );

      pid_setpoint_needle = msg_byte;
      Serial.println("Change Needle Temp Set: "); 
      Serial.print(pid_setpoint_needle);
    } 
    else 
    {
      bioSPI_writeByte(255);
    }
    // Note: play with this delay to determine how long it needs to be
    // delay(500); //works but is probably too slow
    // delay(50); //works
    delay(10); // This delay allows for the master to return SS to High before next loop

    RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );

  } // if(!digitalRead(SS))
  
  
  /* TEMPERATURE CONTROLS */  
  
  // Read from Master to see if cmd has been sent.
  // If no cmd sent keep current state.
  // If disable cmd sent, set diable variable to true
  //   also reset the PID loop.
  // If enable cmd sent, set diable variable to true
  // 
  // Additionally if any cmd sent echo it to validate it is correct
  //
  
  // Hook to not check temperatures until a G code cmd has been 
  // sent. Additionally re-disable temperature check if disable
  // G code cmd is sent (if disabling, fake PID to think that it is at 
  // desired temperature, as a saftey percaution).
  if( TempPID_RESET )
  {
    Serial.println("Resetting Temp PID");
    Serial.println("PID outputs: ");
    
    pid_input_needle = pid_setpoint_tube;
    tubePID.Compute();
    Serial.print(pid_output_tube);
    Serial.print(",");
        
    pid_input_needle = pid_setpoint_syringe;
    syringePID.Compute();
    Serial.print(pid_output_syringe);
    Serial.print(",");
    
    pid_input_needle = pid_setpoint_needle;
    needlePID.Compute();
    Serial.print(pid_output_needle);
    Serial.println("");
    
    TempPID_RESET = false;
  }
  
  /* Check temperature probes if sufficient delay time has passed and PID check is allowed*/
  int tempPollCurrent_ms = millis();
  if( ( (tempPollCurrent_ms - tempPollLast_ms) > tempPollDelay_ms) && 
      ( TempPID_LATCH ) )
  {
    // Update current time for poll
    tempPollLast_ms = tempPollCurrent_ms;
    
    // Syringe
    if (tempProbe_0.checkReadyTemp())
    {
      currentTemp = tempProbe_0.getTemp();
      pid_input_syringe = currentTemp;
      if (currentTemp != -127)     /* if probe reads -127, it is a bad reading */
      {
        syringePID.Compute();
        analogWrite(SYRINGE_HEATER_PIN, pid_output_syringe);
        //Serial.print("Temperature[0]: ");
        Serial.print(currentTemp);
        Serial.print(",");
        //Serial.print("PID control signal: ");
        Serial.print(pid_output_syringe);
        Serial.print(",");
      }
    }
    // Request temperatures so new value is calculated before next call
    // tempProbe_0.requestTemperatures();

    // Tube
    if (tempProbe_1.checkReadyTemp())
    {
      currentTemp = tempProbe_1.getTemp();
      if (currentTemp != -127)     /* if probe reds -127, it is a bad reading */
      {
        pid_input_tube = currentTemp;
        tubePID.Compute();
        // Safety heater shutdown if the tube is getting too hot to avoid melting
        if (currentTemp >= MAX_TUBE_TEMP)
        {
          analogWrite(TUBE_HEATER_PIN, 0);
        }
        else
        {
          analogWrite(TUBE_HEATER_PIN, pid_output_tube);
        }
        Serial.print(currentTemp);
        Serial.print(",");
        Serial.print(pid_output_tube);
        Serial.print(",");
      }
    }
    // Request temperatures so new value is calculated before next call
    //tempProbe_1.requestTemperatures();

    // Needle
    if (tempProbe_2.checkReadyTemp())
    {
      currentTemp = tempProbe_2.getTemp();
      if (currentTemp != -127)     /* if probe reds -127, it is a bad reading */
      {
        pid_input_needle = currentTemp;
        needlePID.Compute();
        analogWrite(NEEDLE_HEATER_PIN, pid_output_needle);
        Serial.print(currentTemp);
        Serial.print(",");
        Serial.println(pid_output_needle);
      }
    }
    // Request temperatures so new value is calculated before next call
    //tempProbe_2.requestTemperatures();

    // Request temperatures so new value is calculated before next call
    tempProbe_0.requestTemperatures();
  }
  
  
  /* INTERRUPT CONTROL OF EXTRUSION */
  if (bioprint_FLAG && (!bioprint_LATCH)) // flag is true and latch is false
  {
    bioprint_LATCH = true;
    //extruder1.continuous_extrude();
    extruder1.resume_extrude();
    Serial.println("Int Continuous Extrude");
  }
  else if (bioprint_LATCH && (!bioprint_FLAG)) // latch is true and flag is false
  {
    bioprint_LATCH = false;
    //extruder1.stop_extrude();
    extruder1.pause_extrude();
    Serial.println("Int Stop Extrusion");
  }


  /* SERIAL CONTROLS */
  if (Serial.available() > 0)
  {
    char serVal[6];
    char tempVal[3];
    int extrudeDelayVal = 0;
    //unsigned int incommingByte = Serial.read();
    Serial.readBytesUntil('\r', serVal, sizeof(serVal));
    Serial.print("Char recieved");
    Serial.println(serVal[0]);
    switch(serVal[0])
    {
      case 101: // "e" char
        // extrude continuous
      #if BP_CURRENT >= BP_ENDSTOP
        if( EndStopReached && !EndStopFull )
        {
          Serial.println("Can't Extrude, it is Empty"); 
        }
        else
      #endif
        {
          Serial.println("Continuous Extrude");
          extruder1.continuous_extrude();
          break;
        }

      case 119: // "w" char
        // extrude with fast resume continuous
      #if BP_CURRENT >= BP_ENDSTOP
        if( EndStopReached )
        {
          Serial.println("Can't Extrude, Endstop set. Try 'f' or 'b'"); 
        }
        else
      #endif
        {
          Serial.println("Resume Extrusion");
          extruder1.resume_extrude();
          break;
        }

      case 112: // "p" char
        // Pause Extrusion
        Serial.println("Pause Extrusion");
        extruder1.pause_extrude();
        break;
          
      case 115: // "s" char
        // stop and fast back
        Serial.println("Stop Extrusion");
        extruder1.stop_extrude();
        break;
          
      case 102: // "f" char
        // Fast forward 5 rev
      #if BP_CURRENT >= BP_ENDSTOP
        if( EndStopReached && !EndStopFull )
        {
          Serial.println("Can't Fast Extrude, Endstop Empty"); 
        }
        else
      #endif
        {
          Serial.println("Fast Forward 5 rev");
          extruder1.fastforward(5);
          break;
        }
          
      case 98: // "b" char
        // Fast backward 5 rev
      #if BP_CURRENT >= BP_ENDSTOP
        if( EndStopReached && EndStopFull )
        {
          Serial.println("Can't Fast Back, Endstop Full"); 
        }
        else
      #endif
        {        
          Serial.println("Fast Backward 5 rev");
          extruder1.fastbackward(5);
          break;
        }

      case 114: // "r" char
      #if BP_CURRENT >= BP_ENDSTOP
        if( EndStopReached && EndStopFull )
        {
          Serial.println("Can't Retract, Endstop Full"); 
        }
        else
      #endif
        {      
          Serial.println("Retract Extrude"); 
          // Reverse Extruder until pulled back all the way
          extruder1.fastretract();
          break;
        }

      case 116: // "t" char
        // Display tempewrature
        Serial.print("Temperature[0]: ");
        Serial.print(pid_input_syringe);
        Serial.print(",");
        Serial.print("PID control signal: ");
        Serial.println(pid_output_syringe);
        Serial.print("Temperature[1]: ");
        Serial.print(pid_input_tube);
        Serial.print(",");
        Serial.print("PID control signal: ");
        Serial.println(pid_output_tube);
        Serial.print("Temperature[2]: ");
        Serial.print(pid_input_needle);
        Serial.print(",");
        Serial.print("PID control signal: ");
        Serial.println(pid_output_needle);
        break;

      case 120: // "x" char
        // Set extruder speed
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        extrudeDelayVal = atoi(tempVal);
        Serial.print("Extrude Delay Val: ");
        Serial.println(extrudeDelayVal);
        Serial.println("Set extruder speed");
        extruder1.set_extrudeDelay(uint16_t(extrudeDelayVal));
        extruder1.set_speed(extrudeDelayVal);
        break;
        
      case 121: // "y" char
        // Set syringe temp
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        pid_setpoint_syringe = atoi(tempVal);
        Serial.println("Set Syringe Temp");
        Serial.println(pid_setpoint_syringe);
        break;
        
      case 117: // "u" char
        // Set tube temp
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        pid_setpoint_tube = atoi(tempVal);
        Serial.println("Set Tube Temp");
        Serial.println(pid_setpoint_tube);
        break;
        
      case 110: // "n" char
        // Set needle temp
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        pid_setpoint_needle = atoi(tempVal);
        Serial.println("Set Needle Temp");
        Serial.println(pid_setpoint_needle);
        break;
        
      case 108: // "l" char
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        
        if(*tempVal == 0)
        {
          // Turn off PID Loop 
          TempPID_RESET = 1;
          TempPID_LATCH = 0;
          Serial.println("Reset and Disable PID Loop");
        }
        if(*tempVal == 1)
        {
          // Turn on PID Loop 
          TempPID_LATCH = 1;
          Serial.println("Activate PID Loop");
        }
        else
        {
          Serial.println("Invalid PID Loop Input");
        }
        break;

      default:
        // Stop extrusion
        //TIMSK1 &= ~(1 << OCIE1A);                                /* Turn back off */
        break;
    }
  }
  
}
