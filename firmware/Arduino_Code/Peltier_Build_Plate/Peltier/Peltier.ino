/* BY: Steve Lammers, www.stevelammers.com
  
  NOTES: 
    Update to use timer1 for the peltier.
    Sets a 1 Sec cycly, with mark/space ratio set
    as before using values 0 - 255
    AJC
    - Since TIMER1 has been repurposed, can no 
    longer use pins 9 or 10 for PWM or timer
    associated funceions
    
  TODO: 
      dutycyclePin is currently set to pin 13?  
*/

// IMPORTANT: Need to turn on BIO_SPI_SLAVE
// in order to include the correct headers and declarations
#define BIO_SPI_SLAVE 1

#include <bioSPI.h>

// int peltierPin = 5;     /* PWM Pin for peltier and fan */
int peltierPin = 9;
int peltierVal = 20;//160;     /* PWM duty cycle */
int fanPin = 6;
int fanVal = 200;
int UV_LED_Pin = 3;            /* PWM signal to LED Driver */
int UV_LED_Val = DEFAULT_UV_INTENSITY;

//int dutycyclePin = 13;    // timebase for duty cycle (Currently ~ 1 sec)
int initCount1 = 3036;

char serVal[5];
char tempVal[3];

// Called at the end of each duty cycle
ISR(TIMER1_OVF_vect) 
{
  TCNT1 = initCount1;
  //digitalWrite(dutycyclePin, digitalRead(dutycyclePin) ^ 1);
  digitalWrite(peltierPin, 0);          // turn peltier off
}


// Called when OCR1A val == TCNT1
ISR(TIMER1_COMPA_vect) 
{
  digitalWrite(peltierPin, 1);    // turn peltier on

  // peltierVal (0-255) needs to be scaled to (0 - (65536 - 3036))
  OCR1A = ((255 - peltierVal) * ((65536 - 3036) / 255)) + 3036;
}


void tmr1_init() 
{
  TCCR1A = 0x00;    // Set timer for normal mode (comparators do not reset count)
  TCCR1B = 0x04;    // set prescaler to / 256 (gives 16uS per tick)

  // 1 S duty cycle =
  // 65536 - (1000000 / 16) = 3036
  TCNT1 = initCount1;             // set counter for 3036 (~1.0 sec to overflow)
  TIMSK1 |= (1 << TOIE1);   // Enable overflow interrupt
  TIMSK1 |= (1 << OCIE1A);  // Enable comparator A intrrupt
}

void setup() 
{
  // put your setup code here, to run once:
  // pinMode(dutycyclePin, OUTPUT);
  pinMode(peltierPin, OUTPUT);
  pinMode(UV_LED_Pin, OUTPUT);
  analogWrite(UV_LED_Pin, 0);         /* Start with UV_LED off */

  Serial.begin(9600);
  Serial.println("Peltier Running: p020");
  analogWrite(fanPin, fanVal);
  Serial.println("Fan Running: f200");


  Serial.println(" SS: ");
  Serial.println(SS);


  // Initialize SPI communications
  slaveInitSPI();

  tmr1_init();
  
  OCR1A = 32000;
  
  interrupts();
}


void loop() 
{
  // put your main code here, to run repeatedly:

  /* SERIAL CONTROLS */
  if (Serial.available() > 0)
  {
    Serial.readBytes(serVal, sizeof(serVal));
    switch(serVal[0])
    {
      case 112: // "p" char
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        peltierVal = atoi(tempVal);
        if(peltierVal == 255) 
        {
          peltierVal--;
        }
        OCR1A = ((255 - peltierVal) * ((65536 - 3036) / 255)) + 3036;
        Serial.print("Peltier Val: ");
        Serial.println(peltierVal);
        break;
      case 102: // "f" char
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        fanVal = atoi(tempVal);
        Serial.print("Fan Val: ");
        Serial.println(fanVal);
        break;
      case 108: // "l" char
        tempVal[0]=serVal[1];
        tempVal[1]=serVal[2];
        tempVal[2]=serVal[3];
        
        if(*tempVal == 0)
        {
          // Turn off UV_LED 
          //Serial.println("Turn off UV_LED");
          analogWrite(UV_LED_Pin, 0);
          Serial.println("LED Off");
        }
        if(*tempVal == 1)
        {
          // Turn off UV_LED 
          //Serial.println("Turn off UV_LED");
          analogWrite(UV_LED_Pin, 1);
          Serial.println("LED On");
        }
        else
        {
          Serial.println("Invalid LED Input");
        }
        break;
      default:
        break;
    }
  }
  // analogWrite(peltierPin, peltierVal);
  analogWrite(fanPin, fanVal);
  
  delay(100);

  // peltierVal = 20; // testing only

  // ---------------- SPI COMMUNICATIONS ---BEGIN--------------
  // If SS goes low, there is an incomming transmission
  if(!digitalRead(SS)) 
  {

    Serial.println("SPI SS triggered: ");
    
    // Get byte from SPI  
    uint8_t msg_byte = 
        RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );

    Serial.print("SPI recieved: ");
    Serial.println(msg_byte);

    // Light LED if msg_byte valid command
    if(msg_byte == SPI_LED_ON)
    {
      // Write msg_byte^ACK
      RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
      
      // Read in new data from Master
      msg_byte = RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );
      UV_LED_Val = msg_byte;

      // Turn on UV_LED and set value
      Serial.print("Turn on UV_LED and set to ");
      Serial.println( UV_LED_Val );
      analogWrite(UV_LED_Pin, UV_LED_Val); 
    }
    else if(msg_byte == SPI_LED_OFF) 
    {
      // Turn off UV_LED 
      //Serial.println("Turn off UV_LED");
      analogWrite(UV_LED_Pin, 0); 
    }
    else if(msg_byte == SPI_PELT_TEMP)
    {
      // Write msg_byte^ACK
      RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
      
      // Read in new data from Master
      msg_byte = RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );
      peltierVal = msg_byte;

      // Change OCR1A based off new Peltier Value
      //Serial.println("Change Peltier Temp");
      OCR1A = ((255 - peltierVal) * ((65536 - 3036) / 255)) + 3036;           
    }
    else if(msg_byte == SPI_FAN_SPEED)
    {
      // Write msg_byte^ACK
      RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
      
      // Read in new data from Master
      msg_byte = RW_WaitForACKMsgOrTimeout( /*NULL*/ 0, READ, SLAVE );
      fanVal = msg_byte;
      
      // Write Speed to Fan 
      //Serial.println("Change Fan Speed");
      analogWrite(fanPin, fanVal);            
    }
    else 
    {
      bioSPI_writeByte(255);
    }
    // Note: play with this delay to determine how long it needs to be
    // delay(500); //works but is probably too slow
    // delay(50); //works

    delay(10); // This delay allows for the master to return SS to High before next loop
    // Respond with byte recieved
    // bioSPI_writeByte(msg_byte);
    // bioSPI_writeByte(ACK_SPI2);
    
    RW_WaitForACKMsgOrTimeout( msg_byte, WRITE, SLAVE );
    
    
    //Serial.print("Respond:");
    //Serial.println( (ACK_SPI^msg_byte) );
  } // if(!digitalRead(SS))
  // ---------------- SPI COMMUNICATIONS ---END----------------

}


