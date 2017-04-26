#include "bioSPI.h"

#if BIO_SPI_SLAVE

	void slaveInitSPI(void)
	{
	  Serial.println("slaveInitSPI");
	  // Initialize SPI pins
	  pinMode(MISO, OUTPUT);
	  pinMode(MOSI, INPUT);
	  pinMode(SCK, INPUT);
	  pinMode(SS, INPUT);
	  // Enable SPI as slave
	  /* Don't have to set phase, polarity b/c
	   * default works with Arduino Uno */
	  //SPCR |= (1 << SPR1);      /* div 16, safer for breadboards */
	  //SPCR |= (1 << MSTR);                        /* clockmaster */
	  SPCR |= (1 << SPE);

	#if DEBUG_SPI_VERBOSE
	  Serial.println("Display Slave SPI Pins:");
	  Serial.print("MISO ");
	  Serial.println(MISO);
	  Serial.print("MOSI ");
	  Serial.println(MOSI);
	  Serial.print("SCK ");
	  Serial.println(SCK);
	  Serial.print("SS ");
	  Serial.println(SS);
	  Serial.println("");
	#endif

	}

#elif BIO_SPI_MASTER // #if BIO_SPI_SLAVE
//#if 0
	// Variables
	bool LED_latch_FLAG = false; // 0-not latched, 1-latched

	void initBioPins(void)
	{
	  SERIAL_ECHOLN("initBioPins");
	  // set pin modes

	  pinMode(SYRINGE_CHIP_SELECT_PIN, OUTPUT);      // Set Chip Select to Output
	  SYRINGE_SLAVE_DESELECT;
	  pinMode(PELTIER_CHIP_SELECT_PIN, OUTPUT);      // Set Chip Select to Output
	  PELTIER_SLAVE_DESELECT;
	  pinMode(SPI_MISO_PIN, INPUT);
	  pinMode(SPI_MOSI_PIN, OUTPUT);
	  pinMode(SPI_SCK_PIN, OUTPUT);

	  // ###AJK Need to do above for SYRINGE_CHIP_SELECT_PIN

	#if DEBUG_SPI_VERBOSE
	  SERIAL_ECHOLN("Display Master SPI Pins:");
	  SERIAL_ECHO("MISO ");
	  SERIAL_ECHOLN(MISO);
	  SERIAL_ECHO("MOSI ");
	  SERIAL_ECHOLN(MOSI);
	  SERIAL_ECHO("SCK ");
	  SERIAL_ECHOLN(SCK);
	  SERIAL_ECHO("SS Peltier ");
	  SERIAL_ECHOLN(PELTIER_CHIP_SELECT_PIN);
	  SERIAL_ECHO("SS Syringe ");
	  SERIAL_ECHOLN(SYRINGE_CHIP_SELECT_PIN);
	  SERIAL_ECHOLN("");
	#endif


	  //pinMode(SPI_SS_PIN, OUTPUT); // Why does this pin need to be set to output when it appears not to be used?

	  /* Set MOSI and SCK output, all others input */
	  //DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	  /* Enable SPI, Master, set clock rate fck/16 */
	  //SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);
	}

	void initBioSPI(void)
	{
		SERIAL_ECHOLN("initBioSPI");
		/* Don't have to set phase, polarity b/c
		 * default works with Arduino Uno */
		SPCR |= (1 << SPR1);                /* div 16, safer for breadboards */
		// SPCR |= (1 << SPR0);                /* div 16, safer for breadboards */
		SPCR |= (1 << MSTR);                                  /* clockmaster */
		SPCR |= (1 << SPE);                                        /* enable */
	}

	//----------------------------------------------//
	//	bioSPI_sendFanSpeed
	//
	//	Description: This function takes in the new
	//	speed and will send the data over SPI. Order
	//  of SPI Communication is as follows: Master
	//	Sends SPI_FAN_SPEED (cmd to change fan speed).
	//	Receives ACK^SPI_FAN_SPEED
	//	Sends New Fan Speed
	//	Receives ACK^FanSpeed.
	//
	//----------------------------------------------//
	void bioSPI_sendFanSpeed( uint8_t FanSpeed )
	{
		uint8_t tmp_ack = 0;
		delay(30);

		SERIAL_ECHO("Fan Speed: ");
		SERIAL_ECHOLN((int)FanSpeed);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_FAN_SPEED, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_FAN_SPEED, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK SPI Fan Cmd: ");
		SERIAL_ECHOLN((int)(tmp_ack));
		delay(SLAVE_SELECT_DELAY);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( FanSpeed, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( FanSpeed, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK Fan Speed: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}

	//----------------------------------------------//
	//	bioSPI_sendNewTemp
	//
	//	Description: This function takes in the new
	//	Temp and will send the data over SPI. Order
	//  of SPI Communication is as follows: Master
	//	Sends SPI_PELT_TEMP (cmd to change peltier temp).
	//	Receives ACK^SPI_PELT_TEMP
	//	Sends New Peltier Temp
	//	Receives ACK^PeltierTemp.
	//
	//----------------------------------------------//
	void bioSPI_sendPeltNewTemp( uint8_t PeltTemp )
	{
		uint8_t tmp_ack = 0;
		delay(30);

		SERIAL_ECHO("Pelt Temp: ");
		SERIAL_ECHOLN((int)PeltTemp);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_PELT_TEMP, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_PELT_TEMP, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK SPI Peltier Cmd: ");
		SERIAL_ECHOLN((int)(tmp_ack));
		delay(SLAVE_SELECT_DELAY);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( PeltTemp, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( PeltTemp, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK Peltier Temp: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}

	//----------------------------------------------//
	//	bioSPI_turnOnLED
	//
	//	Description: This function send LED State
	//	data over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_LED_ON (cmd to turn on LED).
	//	Receives ACK^SPI_LED_ON
	//
	//----------------------------------------------//
	void bioSPI_turnOnLED( uint8_t UVIntensity)
	{
		uint8_t tmp_ack = 0;
		delay(30);

		SERIAL_ECHO("LED on: ");
		SERIAL_ECHOLN((int)SPI_LED_ON);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_LED_ON, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_LED_ON, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));
		SERIAL_ECHO("LED Intensity: ");
		SERIAL_ECHOLN((int)UVIntensity);
		delay(SLAVE_SELECT_DELAY);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( UVIntensity, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( UVIntensity, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));

	}

	//----------------------------------------------//
	//	bioSPI_turnOffLED
	//
	//	Description: This function send LED State
	//	data over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_LED_OFF (cmd to turn off LED).
	//	Receives ACK^SPI_LED_OFF
	//	Function also sets the latch flag to low.
	//
	//----------------------------------------------//
	void bioSPI_turnOffLED()
	{
		uint8_t tmp_ack = 0;
		delay(30); // 50 works well Note: play with this delay to see how low it can be

		SERIAL_ECHO("LED off: ");
		SERIAL_ECHOLN((int)SPI_LED_OFF);

		PELTIER_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_LED_OFF, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_LED_OFF, READ, MASTER );
		PELTIER_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));

		//Unlatch LED_latch_FLAG
		LED_latch_FLAG = false;

	}

	//----------------------------------------------//
	//	bioSPI_latchOnLED
	//
	//	Description: If the latch flag is not set
	//	This function set the latch flag and sends LED
	//	State data over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_LED_ON (cmd to turn on LED).
	//
	//----------------------------------------------//
	void bioSPI_latchOnLED()
	{
		uint8_t tmp_ack = 0;

		// If latch flag is not set, latch on
		SERIAL_ECHOLN("LATCH!!!!");

		if (!LED_latch_FLAG)
		{
		  LED_latch_FLAG = true;

		  PELTIER_SLAVE_SELECT;
		  delay(30);
		  RW_WaitForACKMsgOrTimeout( SPI_LED_ON, WRITE, MASTER );
		  tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_LED_ON, READ, MASTER );
		  delay(30);
		  PELTIER_SLAVE_DESELECT;

		  SERIAL_ECHO("Hash ACK: ");
		  SERIAL_ECHOLN((int)(tmp_ack));

		}
		// else just leave it latched, unlatch with bioSPI_turnOffLED()

	}


	//----------------------------------------------//
	//	bioSPI_startTempPID
	//
	//	Description: This function send Start Temp PID
	//	loop over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_START_PID (cmd to start temp PID).
	//	Receives ACK^SPI_START_PID
	//	Function also sets the latch flag to low.
	//
	//----------------------------------------------//
	void bioSPI_startTempPID()
	{
		uint8_t tmp_ack = 0;
		delay(30); // 50 works well Note: play with this delay to see how low it can be

		SERIAL_ECHO("Start Temp PID Loop: ");
		SERIAL_ECHOLN((int)SPI_START_PID);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_START_PID, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_START_PID, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));

	}


	//----------------------------------------------//
	//	bioSPI_stopTempPID
	//
	//	Description: This function send Stop Temp PID
	//	loop over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_START_PID (cmd to stop temp PID).
	//	Receives ACK^SPI_START_PID
	//	Function also sets the latch flag to low.
	//
	//----------------------------------------------//
	void bioSPI_stopTempPID()
	{
		uint8_t tmp_ack = 0;
		delay(30); // 50 works well Note: play with this delay to see how low it can be

		SERIAL_ECHO("Stop Temp PID Loop: ");
		SERIAL_ECHOLN((int)SPI_STOP_RESET_PID);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_STOP_RESET_PID, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_STOP_RESET_PID, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}



	//----------------------------------------------//
	//	bioSPI_startExtrude
	//
	//	Description: This function send Start Extrude
	//	over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_EXTRUDER_START (cmd to start extrude).
	//	Receives ACK^SPI_EXTRUDER_START
	//	Function also sets the latch flag to low.
	//
	//----------------------------------------------//
	void bioSPI_startExtrude()
	{
		uint8_t tmp_ack = 0;
		delay(30); // 50 works well Note: play with this delay to see how low it can be

		SERIAL_ECHO("Stop Start Extrude: ");
		SERIAL_ECHOLN((int)SPI_EXTRUDER_START);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_EXTRUDER_START, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_EXTRUDER_START, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}


	//----------------------------------------------//
	//	bioSPI_stopExtrude
	//
	//	Description: This function send Stop Extrude
	//	over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_EXTRUDER_STOP (cmd to stop extrude).
	//	Receives ACK^SPI_EXTRUDER_STOP
	//	Function also sets the latch flag to low.
	//
	//----------------------------------------------//
	void bioSPI_stopExtrude()
	{
		uint8_t tmp_ack = 0;
		delay(30); // 50 works well Note: play with this delay to see how low it can be

		SERIAL_ECHO("Stop Extrude: ");
		SERIAL_ECHOLN((int)SPI_EXTRUDER_STOP);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_EXTRUDER_STOP, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_EXTRUDER_STOP, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}


	//----------------------------------------------//
	//	bioSPI_stopExtrude
	//
	//	Description: This function send retract Extrude
	//	over SPI. Order of SPI Communication
	//	is as follows: Master
	//	Sends SPI_EXTRUDER_RETRACT (cmd to retract extrude).
	//	Receives ACK^SPI_EXTRUDER_RETRACT
	//	Function also sets the latch flag to low.
	//
	//----------------------------------------------//
	void bioSPI_retractExtruder()
	{
		uint8_t tmp_ack = 0;
		delay(30); // 50 works well Note: play with this delay to see how low it can be

		SERIAL_ECHO("Retract Extrude: ");
		SERIAL_ECHOLN((int)SPI_EXTRUDER_RETRACT);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_EXTRUDER_RETRACT, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_EXTRUDER_RETRACT, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}

	//----------------------------------------------//
	//	bioSPI_sendNewSyringeTemp
	//
	//	Description: This function takes in the new
	//	Temp and will send the data over SPI. Order
	//  of SPI Communication is as follows: Master
	//	Sends SPI_TEMP_SYRINGE (cmd to change syringe temp).
	//	Receives ACK^SPI_TEMP_SYRINGE
	//	Sends New Syringe Temp
	//	Receives ACK^SyringeTemp.
	//
	//----------------------------------------------//
	void bioSPI_sendNewSyringeTemp( uint8_t SyringeTemp )
	{
		uint8_t tmp_ack = 0;
		delay(30);

		SERIAL_ECHO("Syringe Temp: ");
		SERIAL_ECHOLN((int)SyringeTemp);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_TEMP_SYRINGE, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_TEMP_SYRINGE, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK SPI Syringe Cmd: ");
		SERIAL_ECHOLN((int)(tmp_ack));
		delay(SLAVE_SELECT_DELAY);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SyringeTemp, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SyringeTemp, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK Syringe Temp: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}

	//----------------------------------------------//
	//	bioSPI_sendNewTubeTemp
	//
	//	Description: This function takes in the new
	//	Temp and will send the data over SPI. Order
	//  of SPI Communication is as follows: Master
	//	Sends SPI_TEMP_TUBE (cmd to change Tube temp).
	//	Receives ACK^SPI_TEMP_TUBE
	//	Sends New Tube Temp
	//	Receives ACK^TubeTemp.
	//
	//----------------------------------------------//
	void bioSPI_sendNewTubeTemp( uint8_t TubeTemp )
	{
		uint8_t tmp_ack = 0;
		delay(30);

		SERIAL_ECHO("Tube Temp: ");
		SERIAL_ECHOLN((int)TubeTemp);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_TEMP_TUBE, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_TEMP_TUBE, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK SPI Tube Cmd: ");
		SERIAL_ECHOLN((int)(tmp_ack));
		delay(SLAVE_SELECT_DELAY);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( TubeTemp, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( TubeTemp, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK Tube Temp: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}

	//----------------------------------------------//
	//	bioSPI_sendNewNeedleTemp
	//
	//	Description: This function takes in the new
	//	Temp and will send the data over SPI. Order
	//  of SPI Communication is as follows: Master
	//	Sends SPI_TEMP_NEEDLE (cmd to change Needle temp).
	//	Receives ACK^SPI_TEMP_NEEDLE
	//	Sends New Needle Temp
	//	Receives ACK^NeedleTemp.
	//
	//----------------------------------------------//
	void bioSPI_sendNewNeedleTemp( uint8_t NeedleTemp )
	{
		uint8_t tmp_ack = 0;
		delay(30);

		SERIAL_ECHO("Needle Temp: ");
		SERIAL_ECHOLN((int)NeedleTemp);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( SPI_TEMP_NEEDLE, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( SPI_TEMP_NEEDLE, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK SPI Needle Cmd: ");
		SERIAL_ECHOLN((int)(tmp_ack));
		delay(SLAVE_SELECT_DELAY);

		SYRINGE_SLAVE_SELECT;
		RW_WaitForACKMsgOrTimeout( NeedleTemp, WRITE, MASTER );
		tmp_ack = RW_WaitForACKMsgOrTimeout( NeedleTemp, READ, MASTER );
		SYRINGE_SLAVE_DESELECT;

		SERIAL_ECHO("Hash ACK Needle Temp: ");
		SERIAL_ECHOLN((int)(tmp_ack));
	}


#endif // #elif BIO_SPI_MASTER #if BIO_SPI_SLAVE

// ------------------------------- //
// COMMON CODE MASTER/SLAVE
// ------------------------------- //

//----------------------------------------------//
//	bioSPI_IsSlaveSelected
//
//	Description: This function returns a true if
//	the Master called the function and it has a
//	Slave selected. Or if a Slave called the
// function and that slave is being selected.
//
//----------------------------------------------//
boolean bioSPI_IsSlaveSelected( void )
{
#if BIO_SPI_MASTER
	return ( ( !digitalRead( PELTIER_CHIP_SELECT_PIN ) ) || ( !digitalRead( SYRINGE_CHIP_SELECT_PIN ) ) );
#elif BIO_SPI_SLAVE
	return ( !digitalRead(SS) );
#endif
}

//----------------------------------------------//
//	bioSPI_tradeByte
//
//	Description: This function swaps the
//  Xfr byte with the Xfr byte from the other
//  board.
//
//----------------------------------------------//
uint8_t bioSPI_tradeByte(uint8_t tData)
{
#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.println("");
	Serial.print("     Trade: Write: ");
	Serial.print((int)tData);
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHOLN("");
	SERIAL_ECHO("     Trade: Write: ");
	SERIAL_ECHO((int)tData);
#endif

  SPDR = tData;                       /* SPI starts sending immediately */
  //loop_until_bit_is_set(SPSR, SPIF);                /* wait until done */
  while(!(SPSR & (1<<SPIF)));
                                /* SPDR now contains the received byte */

#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.print(" Read: ");
	Serial.println((int)SPDR);
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHO(" Read: ");
	SERIAL_ECHOLN((int)SPDR);
#endif

  return(SPDR);
}

//----------------------------------------------//
//	bioSPI_tradeByteSkip
//
//	Description: This function swaps the
//  Xfr byte with the Xfr byte from the other
//  board. If slave select is high (not selected)
//	then the Xfr bails out, avoids hangs.
//
//----------------------------------------------//
uint8_t bioSPI_tradeByteSkip(uint8_t tData)
{
#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.println("");
	Serial.print("     TradeSkip: WriteSkip: ");
	Serial.print((int)tData);
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHOLN("");
	SERIAL_ECHO("     TradeSkip: WriteSkip: ");
	SERIAL_ECHO((int)tData);
#endif

  SPDR = tData;                       /* SPI starts sending immediately */
  //loop_until_bit_is_set(SPSR, SPIF);                /* wait until done */
  while(!(SPSR & (1<<SPIF)))
  {
	  //Check if the Slave is Selected. If not just bail
	  // to avoid being caught in infinite loop
	  if( !bioSPI_IsSlaveSelected() )
	  {
		  return ( NOP );
	  }
  }
  /* SPDR now contains the received byte */
#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.print(" ReadSkip: ");
	Serial.println((int)SPDR);
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHO(" ReadSkip: ");
	SERIAL_ECHOLN((int)SPDR);
#endif

  return(SPDR);
}

//----------------------------------------------//
//	bioSPI_readByteSendACKEnd
//
//	Description: This function reads a byte
//  and transmits the ACK END signal.
//
//----------------------------------------------//
uint8_t bioSPI_readByteSendACKEnd(void)
{
  return bioSPI_tradeByte(ACK_SPI_END);
}

//----------------------------------------------//
//	bioSPI_readByteSendACKStart
//
//	Description: This function reads a byte
//  and transmits the ACK START signal.
//
//----------------------------------------------//
uint8_t bioSPI_readByteSendACKStart(void)
{
  return bioSPI_tradeByte(ACK_SPI_START);
}

//----------------------------------------------//
//	bioSPI_writeByte
//
//	Description: This function reads a byte
//  and transmits the Xfr data.
//
//----------------------------------------------//
uint8_t bioSPI_writeByte(uint8_t wData)
{
  //SPDR = byte;
  return bioSPI_tradeByte(wData);
}

//----------------------------------------------//
//	ResyncSPIStart
//
//	Description: This function resyncs both the
//  slave and the master encase they are out of
//  sync. It loops through sending the ACK START
//  and waits for the other board to transmit the
//  ACK START as well. Then transmits it a few
//  more times (fix for when one board sends
//  ACK START but doesn't see it, even though it
//	was transmitted, this causes one board to wait
//	while the other moves on).
//
//----------------------------------------------//
void ResyncSPIStart( void )
{
	int i;

	// It appears that the MISO line is much faster than
	// the MOSI line, as a result the Master will see the slave
	// sync cmd and send many more before the slave sees the
	// masters sync. If SYNC_LOOP_INDEX or SYNC_LOOP_DELAY is to low
	// then this can cause a hang.

	while( ACK_SPI_START != bioSPI_readByteSendACKStart( ) )
	{
		delay(SYNC_LOOP_DELAY);
	}

	for( i = 0; i < SYNC_LOOP_INDEX; i++ )
	{
		// Now that one received the ACK_SPI_START
		// Write a few more times, just encase
		// the other board wrote the NOP but
		// didn't see the one that was transmitted.
		bioSPI_readByteSendACKStart( );
		delay(SYNC_LOOP_DELAY);
	}
#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.println(" ResyncSPIStart ... done");
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHOLN(" ResyncSPIStart ... done");
#endif
}

//----------------------------------------------//
//	ResyncSPIMid
//
//	Description: This function resyncs both the
//  slave and the master encase they are out of
//  sync. It loops through sending the ACK START
//	expecting ACK END to be sent if the Master,
//  and the reverse if the Slave.
//  Then transmits it a few
//  more times (fix for when one board sends
//  ACK START but doesn't see it, even though it
//	was transmitted, this causes one board to wait
//	while the other moves on).
//
//----------------------------------------------//
void ResyncSPIMid( Board MasterHost )
{
	int i;
	uint8_t WriteByte;
	uint8_t ReadByte;

	if( MasterHost )
	{
		WriteByte = ACK_SPI_START;
		ReadByte = ACK_SPI_END;
	}
	else
	{
		WriteByte = ACK_SPI_END;
		ReadByte = ACK_SPI_START;
	}

	while(ReadByte != bioSPI_writeByte( WriteByte ))
	{
		delay(SYNC_LOOP_DELAY);
	}

	for( i = 0; i < SYNC_LOOP_INDEX; i++)
	{
		// Now that one received the WriteByte
		// Write a few more times, just encase
		// the other board wrote the ReadByte but
		// didn't see the one that was transmitted.
		bioSPI_writeByte( WriteByte );
		delay(SYNC_LOOP_DELAY);
	}
#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.println(" ResyncSPIMid ... done");
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHOLN(" ResyncSPIMid ... done");
#endif
}

//----------------------------------------------//
//	ResyncSPIEnd
//
//	Description: This function resyncs both the
//  slave and the master encase they are out of
//  sync. It loops through sending the ACK END
//  and waits for the other board to transmit the
//  ACK END as well. Then transmits it a few
//  more times. Resync end will not hold up the
//  sees that the master no longer has it selected
//  this is to avoid hangs on the slave side.
//  Additionally the slave will always transmit until
//  master is done, so that the master does not hang.
//
//----------------------------------------------//
void ResyncSPIEnd( void )
{
	int i;

	#if BIO_SPI_SLAVE
		while( ACK_SPI_END != bioSPI_tradeByteSkip( ACK_SPI_END ) )
	#elif BIO_SPI_MASTER
		while( ACK_SPI_END != bioSPI_tradeByte( ACK_SPI_END ) )
	#endif
		{
		#if BIO_SPI_SLAVE
		  //Check if the Slave is Selected. If not just bail
		  // to avoid being caught in infinite loop
		  if( !bioSPI_IsSlaveSelected() )
		  {
			  break;
		  }
		#endif
		  delay(SYNC_LOOP_DELAY);
		}

	for( i = 0; i < SYNC_LOOP_INDEX; i++ )
	{
		// Now that one received the ACK_SPI_END
		// Write a few more times, just encase
		// the other board wrote the NOP but
		// didn't see the one that was transmitted.
#if BIO_SPI_SLAVE
		bioSPI_tradeByteSkip( ACK_SPI_END );
#elif BIO_SPI_MASTER
		bioSPI_tradeByte( ACK_SPI_END );
#endif
#if BIO_SPI_SLAVE
		//Check if the Slave is Selected. If not just bail
		// to avoid being caught in infinite loop
		if( !bioSPI_IsSlaveSelected() )
		{
			break;
		}
#endif
		delay(SYNC_LOOP_DELAY);

	}
#if DEBUG_SPI_CMD_TRACE && BIO_SPI_SLAVE
	Serial.println(" ResyncSPIEnd ... done");
#elif DEBUG_SPI_CMD_TRACE && BIO_SPI_MASTER
	SERIAL_ECHOLN(" ResyncSPIEnd ... done");
#endif
}

//----------------------------------------------//
//	SendNOP
//
//	Description: This function transmits a NOP
//  (No operation) msg. This is a filler function
//  that is used for syncs.
//
//----------------------------------------------//
void SendNOP( void )
{
	bioSPI_writeByte( NOP );
	delay(SYNC_LOOP_DELAY);
}

//-------------------------------------------------//
//	RW_WaitForACKMsgOrTimeout
//
//	Input: Msg        - The Msg that will be evaluated or
//						manipulated.
//		   WriteCmd   - Input to wither to write or
//						or read.
//		   MasterHost - True if master, false if slave
//
//	Description: This function regulates the inputs
//	and outputs from Master/Slave SPI Communication.
//	It exits when the Msg was successfully sent or
//  received or the cmd had a timeout.
//
//	Rules: Master Write send Msg and receives ACK END
//		   Master Read sends ACK START and receives Msg^ACK END
//		   Slave Write sends Msg^ACK END and receives ACK END
//		   Slave Read sends ACK END and receives Msg
//
//	Xfr Order: Sync with ACK START
//			   Master Writes Cmd
//			   Slave Reads
//			   Sync with Mid ACK
//			   Slave Write Acknowledgment
//			   Master Reads Acknowledgment
//			   Sync with ACK END
//
//	COM Exmp:	Master		Slave
//				W 	R		W	R
//	Sync Start *STR STR	   *STR STR
//			   *Msg STR	    STR Msg
//			    Msg END	   *END Msg
//	Sync Mid   *STR END	   *END STR
//				STR M^E	   *M^E	STR
// 			   *END M^E		M^E END
//  Sync End   *END END	   *END END
//
// NOTE: STR (start ACK), END (end ACK)
//		 M^E (Msg ^ End ACK)
//		 * is who sending signal
//
//
//	Return: If Write set then NULL, else what was read.
//
//-------------------------------------------------//
uint8_t RW_WaitForACKMsgOrTimeout(uint8_t Msg,
								  RW WriteCmd,
								  Board MasterHost )
{
	uint8_t msg_byte;
	int TimeoutItterator = 0;
	uint8_t ExpectedData;

	if( ( WriteCmd && MasterHost ) || ( !WriteCmd && !MasterHost ) )
	{
		// Wait until both Master and Slave are in Sync
		// Occurs at the start of every Master Initiated Cmd.
		ResyncSPIStart();
	}

	// If the Master is reading from the slave
	// then the input message to the slave
	// XOR with ACK is what is expected to be returned
	// to the Master.
	// Note: Skipping Slave Read
	// it is set later down, because all inputs
	// are valid except ACK signals.
	if( !WriteCmd && MasterHost )
	{
		ExpectedData = ACK_SPI_END^Msg;
	}
	else if( WriteCmd && MasterHost )
	{
		ExpectedData = ACK_SPI_END;
	}
	else if( WriteCmd && !MasterHost )
	{
		ExpectedData = ACK_SPI_END;
	}

	// If the Slave is writing to the Master
	// then the input message to the Master should be
	// XOR with ACK.
	if( WriteCmd && !MasterHost )
	{
		// Slave Write sends Msg^ACK
		// NOTE: Do not put inside loop
		// will cause the the data to be XOR
		// multiple times.
		Msg ^= ACK_SPI_END;
	}

	// Loop through SPI transmission of the message
	// until the data was sent or the timeout expired.
	do
	{
	  TimeoutItterator++;

	  if( WriteCmd )
	  {
		  msg_byte = bioSPI_writeByte( Msg );
	  }
	  else
	  {
		  if( !MasterHost )
		  {
			  // This is a Slave read, just send ACK Start
			  msg_byte = bioSPI_readByteSendACKEnd();

			  if( ( msg_byte == ACK_SPI_START ) || ( msg_byte == ACK_SPI_END ) )
			  {
				  // Slave received and ACK msg
				  // It should only receive a Cmd at this point
				  // Set the expected data to something else
				  // so that it reads again.
				  ExpectedData = msg_byte^0b11111111;
			  }
			  else
			  {
				  // The Slave receive a Cmd
				  // exit the loop
				  break;
			  }

		  }
		  else
		  {
			  msg_byte = bioSPI_readByteSendACKEnd();
		  }

	  }

	  // Give a brief moment for the boards to sync
	  delay(1000);

	}while( ( TimeoutItterator < TIMEOUT_DELAY ) &&
			( msg_byte != ExpectedData ) );


	#if ( DEBUG_SPI_VERBOSE || DEBUG_SPI_TIMEOUT )
	#if ( DEBUG_SPI_TIMEOUT && !DEBUG_SPI_VERBOSE )
	  // If the Timeout expired this means that either
	  // the data being sent was the same as the ACK value
	  // or the data was not updated in time
	  if( TimeoutItterator == TIMEOUT_DELAY )
	#endif
	  {
	  #if BIO_SPI_SLAVE
	  #if DEBUG_SPI_VERBOSE
		if ( TimeoutItterator == TIMEOUT_DELAY )
		{
		  Serial.println(" Timeout or sent ACK data");
		}
	  #endif
	    Serial.print("Msg: ");
	    Serial.println((int)Msg);
	    Serial.print("Write: ");
	    Serial.println((int)WriteCmd);
	    Serial.print("Master: ");
	    Serial.println((int)MasterHost);
	    Serial.print("msg_byte: ");
	    Serial.println((int)msg_byte);
	    Serial.print("TimeoutItterator: ");
	    Serial.println((int)TimeoutItterator);
	  #elif BIO_SPI_MASTER
	  #if DEBUG_SPI_VERBOSE
		if ( TimeoutItterator == TIMEOUT_DELAY )
		{
			SERIAL_ECHOLN(" Timeout or sent ACK data");
		}
	  #endif
	    SERIAL_ECHO("Msg: ");
	    SERIAL_ECHOLN((int)Msg);
	    SERIAL_ECHO("Write: ");
	    SERIAL_ECHOLN((int)WriteCmd);
	    SERIAL_ECHO("Master: ");
	    SERIAL_ECHOLN((int)MasterHost);
	    SERIAL_ECHO("msg_byte: ");
	    SERIAL_ECHOLN((int)msg_byte);
	    SERIAL_ECHO("TimeoutItterator: ");
	    SERIAL_ECHOLN((int)TimeoutItterator);
	  #endif

	  }
	#endif

	if( ( !WriteCmd && MasterHost ) || ( WriteCmd && !MasterHost ) )
	{
		// Wait until both Master and Slave are in Sync
		// Occurs at the end of every return ACK
		// for a Master Initiated Cmd.
		ResyncSPIEnd();

		if( !MasterHost && WriteCmd )
		{
			// If Slave and at the end of the transmission
			// then loop through NOP until Slave Select pin
			// goes high (deselected).
			// This is so that neither the master or slave
			// are waiting for a transmission and hang
			// (sync loops can result in this), thus
			// let the Master decide when done with Xfr.
			while( bioSPI_IsSlaveSelected() )
			{
				// ###AJK test that this code is working
				// Also may not be needed, since sync end loops
				// now bail if slave select is low ???
				bioSPI_tradeByteSkip( NOP );
			}
		}
	}
	else if( ( WriteCmd && MasterHost ) || ( !WriteCmd && !MasterHost ) )
	{
		// Wait until both Master and Slave are in Sync
		// Occurs at the end of every
		// Master Initiated Cmd.
		// Note: Passing in MasterHost, remember
		// MasterHost holds wither it is a Master
		// or Slave. Passing this in is equivalent
		// to passing in SLAVE if Slave, and MASTER if Master
		ResyncSPIMid( MasterHost );
	}
#if DEBUG_SPI_VERBOSE
#if BIO_SPI_SLAVE
	Serial.println(" Msg ACK ... done");
#elif BIO_SPI_MASTER
	SERIAL_ECHOLN(" Msg ACK ... done");
#endif
#endif

	if( WriteCmd )
	{
		// If writing no need to pass data
		// to the interface.
		return ( /*NULL*/ 0 );
	}

	// Cmd was Read, return what was read.
	return ( msg_byte );
}
