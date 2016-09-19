/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** AM           Andreas Mark                                                  **
** ES			Elias Steurer												  **
** DW           Dominik Wieland                                               **
**                                                                            **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.0  : 22-12-2014, AM:  Initial Version
 * V0.0.1: 10-07-2015, ES:  Added Bluetooth Support
 * V0.1  : 21-07-2016, DW:  Port of SW from DAVE3 to DAVE4
 */


#ifndef RCRECEIVE_H_
#define RCRECEIVE_H_


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <DAVE.h>			/**<Declarations from DAVE Code Generation (includes SFR declaration) */
#include <xmc_usic.h>		/**<For XMC usic functions */
#include <xmc_uart.h>		/**<For XMC uart function */
#include "../_HAL/GPIO.h"	/**<Getting global Macro definitions */
#include "../_HAL/UART/UARTdev.h"	/**<For reading / writing via UART */
#include "BluetoothProtocol/BluetoothProtocol.h"	/**<Bluetooth */


/*******************************************************************************
**                      Private Constant Definitions to be changed            **
*******************************************************************************/
//#define BT_ONLY		//Only Bluetooth-App-Control needed


/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/
/**
 * @brief Structure for receiving the joystick values
 */
typedef struct QUADROCOPTER_RC_RECEIVE_JOYSTICK_VALUES_
{
	float throttle; /**< throttle value */
	float rudder;	/**< rudder value */
	float elevator;	/**< elevator value */
	float aileron;	/**< aileron value */
}QUADROCOPTER_RC_RECEIVE_JOYSTICK_VALUES;

/**
 * @brief Structure for receiving the bluetooth values
 */
typedef struct QUADROCOPTER_RC_RECEIVE_BLUETOOTH_VALUES_
{
	uint16_t throttle;		/**< throttle value */
	uint8_t heightControl;	/**< value for height control */
	float rudder;			/**< rudder value */
	float aileron;			/**< aileron value */
	float elevator;			/**< elevator value */
}QUADROCOPTER_RC_RECEIVE_BLUETOOTH_VALUES;

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/
//Scale factors for angle and power	// other divisions tried with good stabilization
#define SCALE_YAW	90.0f			/**< Scale factor for angle and power | from -10..+10 - divided by 3 */
#define SCALE_PITCH	30.0f			/**< Scale factor for angle and power | from -10..+10 - divided by 10 */ 
#define SCALE_ROLL	30.0f			/**< Scale factor for angle and power | from -10..+10	- divided by 10*/  
#define SCALE_POWER	100.0f			/**< Scale factor for angle and power | from 0..100 */ 

//Scale factors for joysticks to norm vaue between -1 and 1
#define THROTTLE_MIN	306		/**< Scale factor for joysticks to norm value between -1 and 1 */ 
#define THROTTLE_MAX	1783	/**< Scale factor for joysticks to norm value between -1 and 1 */ 
#define RUDDER_MIN		6450	/**< Scale factor for joysticks to norm value between -1 and 1 */ 
#define RUDDER_MAX		7884	/**< Scale factor for joysticks to norm value between -1 and 1 */ 
#define ELEVATOR_MIN	4400	/**< Scale factor for joysticks to norm value between -1 and 1 */
#define ELEVATOR_MAX	5834	/**< Scale factor for joysticks to norm value between -1 and 1 */
#define AILERON_MIN		2354	/**< Scale factor for joysticks to norm value between -1 and 1 */ 
#define AILERON_MAX		3788	/**< Scale factor for joysticks to norm value between -1 and 1 */ 

//Values for Flightmode ON/OFF
#define FLIGHTMODE0			8498	/**< Value for Flightmode OFF */ 
#define FLIGHTMODE1			9930	/**< Value for Flightmode ON */


/*******************************************************************************
**                      Private Constant Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void GetRCData(float* power,uint8_t* height_control, float* yaw, float* pitch, float* roll);

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
void RC_Receive_Int_Handler();
void BT_Receive_Int_Handler();
void WatchRC(void* Temp);
void WatchRC_Init();
uint8_t GetRCCount();
int mergeBytes (uint8_t a, uint8_t b);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif /* RCRECEIVE_H_ */
