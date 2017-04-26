/* Arduino bioSPI
 * 2015 by Steven Lammers
 *
 * This file is part of the Arduino bioSPI Library
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
 * along with the Arduino bioSPI Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef bioSPI_h
#define bioSPI_h

#include <avr/io.h> //for loop_until_bit_is_set

// ###AJK
// This should be placed in the INO file
// however there is a weird linker issue that
// will not set the flag at the right time
// to include the files in the correct order
#define BIO_SPI_SLAVE  1
#define BIO_SPI_MASTER 0

// Common Custom Flags
#define DEBUG_SPI_TIMEOUT		1
#define DEBUG_SPI_VERBOSE		1
#define DEBUG_SPI_CMD_TRACE		0

// Block Points (HW Changes)
#define BP_CURRENT		1		// Current BP Number, Change This to desired BP Number
#define BP_ENDSTOP		1		// BP 1 is for Endstop support, wire layout change

#if BIO_SPI_SLAVE

	#include <Arduino.h>

	// Pins and Code
	#define CHIP_SELECT_PIN

	// -------------------------------- //
	//	Function Prototypes SPI
	// -------------------------------- //
							  /* Init SPI to run bioSPI with phase, polarity = 0,0 */
	void slaveInitSPI(void);

	/*
	//------------------------------------------------------------------------------
	// SPI speed is F_CPU/2^(1 + index), 0 <= index <= 6
	//  Set SCK to max rate of F_CPU/2. See Sd2Card::setSckRate().
	uint8_t const SPI_FULL_SPEED = 0;
	//  Set SCK rate to F_CPU/4. See Sd2Card::setSckRate().
	uint8_t const SPI_HALF_SPEED = 1;
	//  Set SCK rate to F_CPU/8. See Sd2Card::setSckRate().
	uint8_t const SPI_QUARTER_SPEED = 2;
	//  Set SCK rate to F_CPU/16. See Sd2Card::setSckRate().
	uint8_t const SPI_EIGHTH_SPEED = 3;
	//  Set SCK rate to F_CPU/32. See Sd2Card::setSckRate().
	uint8_t const SPI_SIXTEENTH_SPEED = 4;
	//------------------------------------------------------------------------------
	*/

	// Global Variables

	// Set this for the max amount of time expected for an
	// SPI signal change to occur

	//###AJK
	// This is being defined multiple times
	// Why ???? (Wont Compile)
	/*
	#ifndef TIMEOUT_DELAY
	int TimedOutDelay = 100;
	#define TIMEOUT_DELAY 1
	#endif
	*/
#elif BIO_SPI_MASTER // #if BIO_SPI_SLAVE
//#if 0
	#include "Greenback.h"
	#include "bioSPI_pins.h"

	#define PELTIER_SLAVE_SELECT    digitalWrite(PELTIER_CHIP_SELECT_PIN, LOW)
	#define PELTIER_SLAVE_DESELECT  digitalWrite(PELTIER_CHIP_SELECT_PIN, HIGH)

	#define SYRINGE_SLAVE_SELECT    digitalWrite(SYRINGE_CHIP_SELECT_PIN, LOW)
	#define SYRINGE_SLAVE_DESELECT  digitalWrite(SYRINGE_CHIP_SELECT_PIN, HIGH)

	/*
	//------------------------------------------------------------------------------
	// SPI speed is F_CPU/2^(1 + index), 0 <= index <= 6
	//  Set SCK to max rate of F_CPU/2. See Sd2Card::setSckRate().
	uint8_t const SPI_FULL_SPEED = 0;
	//  Set SCK rate to F_CPU/4. See Sd2Card::setSckRate().
	uint8_t const SPI_HALF_SPEED = 1;
	//  Set SCK rate to F_CPU/8. See Sd2Card::setSckRate().
	uint8_t const SPI_QUARTER_SPEED = 2;
	//  Set SCK rate to F_CPU/16. See Sd2Card::setSckRate().
	uint8_t const SPI_EIGHTH_SPEED = 3;
	//  Set SCK rate to F_CPU/32. See Sd2Card::setSckRate().
	uint8_t const SPI_SIXTEENTH_SPEED = 4;
	//------------------------------------------------------------------------------
	*/

	// -------------------------------- //
	//	Function Prototypes SPI
	// -------------------------------- //
	void initBioPins(void);

					  /* Init SPI to run bioSPI with phase, polarity = 0,0 */
	void initBioSPI(void);

														/* Send Peltier Temp */
	void bioSPI_sendPeltNewTemp( uint8_t byte );

														/* Send Fan Speed */
	void bioSPI_sendFanSpeed( uint8_t byte );

														/* turn on an LED */
	void bioSPI_turnOnLED( uint8_t byte );

														/* turn off an LED */
	void bioSPI_turnOffLED();

														/* latch on an LED */
	void bioSPI_latchOnLED();

														/* Start Temp PID Loop*/
	void bioSPI_startTempPID();

	void bioSPI_stopTempPID();

	void bioSPI_startExtrude();

	void bioSPI_stopExtrude();

	void bioSPI_retractExtruder();

	void bioSPI_sendNewSyringeTemp( uint8_t SyringeTemp );

	void bioSPI_sendNewTubeTemp( uint8_t TubeTemp );

	void bioSPI_sendNewNeedleTemp( uint8_t NeedleTemp );

#endif // #elif BIO_SPI_MASTER #if BIO_SPI_SLAVE

// ------------------------------- //
// COMMON CODE MASTER/SLAVE
// ------------------------------- //

/* Which pin selects BIOSPI as slave? */
#define SLAVE_SELECT    digitalWrite(CHIP_SELECT_PIN, LOW)
#define SLAVE_DESELECT  digitalWrite(CHIP_SELECT_PIN, HIGH)

// -------------------------------- //
//	End SPI Instruction Set
// -------------------------------- //
//  If following lower 5-bits you are
//  using that device. Upper 3-bits
//  are for sub cmds.
//
//	0: NOP
//	1: Status Registers
//	2: LED
//	3: Peltier Temp
//	4: Fan Speed
//	5: Extruder
//	6: Syringe Heater Elements
//
// -------------------------------- //
#define NOP						0b0000000			  /* No Operation */
#define ACK_SPI_END        		0b00011111			  /* ACK, Sub Code 0, Cmd MAX */
#define ACK_SPI_START			0b11100000			  /* ACK, Sub Code MAX, Cmd 0 */

#define BIOSPI_RDSR      		0b00100001            /* read status register */
#define BIOSPI_WRSR      		0b00000001            /* write status register */

#define SPI_LED_ON       		0b00100010            /* Turn on LED */
#define SPI_LED_OFF      		0b00000010            /* Turn Off LED */

#define SPI_PELT_TEMP	 		0b00100011			  /* Set Peltier Temp */
#define SPI_PELT_TEMP_QUERY 	0b01000011			  /* Get Temp Reading of Peltier */

#define SPI_FAN_SPEED	 		0b00100100			  /* Set Fan Speed */

#define SPI_EXTRUDER_STOP		0b00000101			  /* Stop Extruding */
#define SPI_EXTRUDER_START		0b00100101			  /* Begin Extruding */
#define SPI_EXTRUDER_RETRACT 	0b01000101			  /* Retract Extruder */

#define SPI_TEMP_SYRINGE		0b00100111			  /* Change Set Temp for Tube */
#define SPI_TEMP_TUBE			0b01000111			  /* Change Set Temp for Tube */
#define SPI_TEMP_NEEDLE			0b01100111			  /* Change Set Temp for Tube */

#define SPI_START_PID			0b00101000			  /* Begin the PID Temp Loops */
#define SPI_STOP_RESET_PID		0b01001000			  /* Stop and reset PID Loops */

// -------------------------------- //
//	Static Values SPI
// -------------------------------- //

#define TIMEOUT_DELAY	 	10					  /* Itterator Before Timeout Exceeded and accepts cmd */
#define SYNC_LOOP_INDEX  	4					  /* Indicates how many times Sync Xfr is sent after sync confirmation */
#define SYNC_LOOP_DELAY     200					  /* Indicates how long to wait between each Sync Xfr in post loop */
#define SLAVE_SELECT_DELAY  200					  /* Indicates how long to wait between each Sync Xfr in post loop */

// Note: For performance increases SYNC_LOOP values can be lowered but risk having issues.
// SYNC_LOOP_INDEX is a saftey percaution, may work at 0. Ideal keep low values > 6
// SYNC_LOOP_DELAY will not work at 0, Master is to fast. If SYNC_LOOP_DELAY is lowered consider raising SYNC_LOOP_INDEX.

// -------------------------------- //
//	Default Start Values
// -------------------------------- //
#define DEFAULT_UV_INTENSITY		80


// -------------------------------- //
//	Enum SPI
// -------------------------------- //
typedef enum { SLAVE, MASTER } Board;			  /* Indicates wither Slave or Master */
typedef enum { READ, WRITE} RW;					  /* Indicates wither SPI Read or Write */

// -------------------------------- //
//	Function Prototypes SPI
// -------------------------------- //
/* Generic.  Just loads up HW SPI register and waits */
uint8_t bioSPI_tradeByte(uint8_t byte);

/* gets a byte from a given memory location */
uint8_t bioSPI_readByte();

/* writes a byte to a given memory location */
uint8_t bioSPI_writeByte(uint8_t byte);

void SendNOP( void );

/* COM For Write or Read from Master or Slave Function */
uint8_t RW_WaitForACKMsgOrTimeout(uint8_t Msg,
		  RW WriteCmd,
		  Board MasterHost );

#endif //bioSPI
