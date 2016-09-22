/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** AM           Andreas Mark                                                  **
** DW           Dominik Wieland                                               **
** ES           Elias Steurer  												  **
** ML           Michael Lamp                                                  **
** RT			Raffael Tschinder                                             **
*******************************************************************************/
/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.01 : 25-12-2014, AM:  Initial Version
 * V0.01*: 15-08-2015, ES:  Minor Code-Changes
 * V0.02 : 21-07-2016, DW:  Port of SW from DAVE3 to DAVE4
 * V0.03 : 14-09-2016, ML:  New MPU Files (MPU9250) + Micrium Debug Interface
 * V0.04 : 16-09-2016, RL:  Added I2C-TimeOut Counter
 */
/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <DAVE.h>	//Declarations from DAVE Code Generation (includes SFR declaration)
#include "_Quadrocopter/Attitude_Control/AttitudeController.h"

/*******************************************************************************
**                      Private Constant Definitions to be changed            **
*******************************************************************************/

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
void USB_Debug_Task(void);
/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

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

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/


/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and the setup of the Quadrocopter hardware.
 */
int main(void)
{
	setup();
	XMC_USBD_Enable();
	USBD_VCOM_Init(USBD_VCOM_handle);
	USBD_VCOM_Connect();
	// XMC_USBD_Init(XMC_USBD_t *obj);
	while(1U)
	{
		 USB_Debug_Task();
	}
}

extern float pressure;/**< global variable for storing current pressure value*/
extern float temperature;/**< global variable for storing current temperature value*/
extern struct structFIR PressureFIR;/**< global struct for filtering pressure values*/

extern float powerD;/* throttle value of remote, range: 0 - 100*/
extern float yawD_dot;/* yaw value of remote, range: -90 - +90*/
extern float pitchD;/* pitchD value of remote, range: -30 - +30*/
extern float rollD;/* rollD value of remote, range: -30 - +30*/
extern float YPR[3];
extern float u_yaw_dot;/* output of angle-rate controller for yaw*/
extern float u_pitch;/* output of angle controller for pitch*/
extern float u_roll;/* output of angle controller for roll*/
extern float PWM_percent[4];/* scaled motor power from controller output, range: 0 - 100*/
extern float aRes, gRes, mRes; /// scale resolutions per LSB for the sensors

extern int16_t accRaw[3];	/**< raw data from accelerometer */
extern int16_t gyroRaw[3];	/**< raw data from gyroscope */
extern int16_t magRaw[3];	/**< raw data from magnetometer  */

extern float acc[3];	/**< corrected and usefull accelerometer data */
extern float gyro[3];	/**< corrected and usefull gyro data */
extern float mag[3] ;	/**< corrected and usefull magnetometer data */

extern int16_t accBias[3];
extern int16_t gyroBias[3];


/**
 *  \brief Debug-Function using USB(experimental)
 *
 *
 *  \details Should be placed in while-loop of main function due to CDC_Device_USBTask(see USBD_VCOM)
 */
void USB_Debug_Task(void)
{
	//------------------------------------------------------------------------------------------------------------------------------
	//		for serial communication (USB)
	// Check number of bytes received
	static uint8_t USB_RX_Buffer[50];
	static uint8_t USB_TX_Buffer[50];

	uint32_t USB_Bytes_rec = USBD_VCOM_BytesReceived();

	if (USB_Bytes_rec != 0)
	{
		for (uint32_t USB_nByte = 0; USB_nByte < USB_Bytes_rec; USB_nByte++)
		{
			// Receive Byte
			if (USBD_VCOM_ReceiveByte((int8_t*)&USB_RX_Buffer[0]) != USBD_VCOM_STATUS_SUCCESS)
			{
				//Error
			}
		}
		switch (USB_RX_Buffer[0])
		{
		case '1':
			sprintf(USB_TX_Buffer,
					"Throttle: %f   Rudder: %f   Elevator: %f   Aileron: %f\r\n",
					powerD, yawD_dot, pitchD, rollD);
			break;
		case '2':
			//PWMSP001_Start(&MagCalib_Timer);
			break;
		case '3':
			sprintf(USB_TX_Buffer, "Y:%1.2f P:%1.2f R:%1.2f\r\n",
					YPR[0], YPR[1], YPR[2]);
			break;
		case '4':
			sprintf(USB_TX_Buffer, "Y_dot:%1.2f\r\n", u_yaw_dot);
			break;
		case '5':
			sprintf(USB_TX_Buffer, "PWM1:%f PWM2:%f PWM3:%f PWM4:%f\r\n",
					PWM_percent[0], PWM_percent[1], PWM_percent[2], PWM_percent[3]);
			break;
		case '6':
			//sprintf(USB_TX_Buffer, "PWM1:%f PWM2:%f PWM3:%f PWM4:%f\r\n",
			//		actuator_speed_percent[0], actuator_speed_percent[1],
			//		actuator_speed_percent[2], actuator_speed_percent[3]);
			break;
		case '7':
			sprintf(USB_TX_Buffer, "eY:%f eP:%f eR:%f\r\n",
					yawD_dot - u_yaw_dot, pitchD - YPR[1], rollD - YPR[2]);
			break;
		case '8':
			//sprintf(USB_TX_Buffer,
			//		"TimerSensor:%d TimerMain:%d TimerRC:%d\r\n",
			//		(int) GetSensorCount(), (int) counter_main,
			//		(int) GetRCCount());
			break;
		case '9':
			//updateValues(&p, &t);
			sprintf(USB_TX_Buffer,
					"Pressure: %d Temperature: %d (%f C) Counter: %d\r\n",
					(int) pressure, (int) temperature, ((int) temperature) / 32768.0,
					(int) 0 /*DPS310_INT_counter*/);
			break;
		case '0':
			sprintf(USB_TX_Buffer, "gyro0: %f gyro1: %f gyro2: %f\r\n",
					gyro[0], gyro[1], gyro[2]);
			break;
		}
		USBD_VCOM_SendString((const int8_t *) USB_TX_Buffer);
		USB_TX_Buffer[0] = 0;
	}

	CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
	//------------------------------------------------------------------------------------------------------------------------------
}
