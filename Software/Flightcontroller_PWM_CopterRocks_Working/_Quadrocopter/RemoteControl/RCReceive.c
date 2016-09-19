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


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "RCReceive.h"	/**<Header file*/


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
extern uint8_t* ReadBufBT;			/**< pointer to bluetooth input buffer */
extern ControlValue control_value;	/**< control value for bluetooth */
extern DataPacket dpacket;			/**< data packet value for bluetooth*/

uint8_t flightmode = 0;		/**<remote control switch; value can be: 0 -> RC or 1 -> Bluetooth*/
uint8_t RCTimeOut = 1; 		/**<Timeout remote control 0:OK 1:TimeOut */
uint8_t RCCount   = 0; 		/**<Counter for RC read (used for TimeOut)*/
uint8_t BTTimeOut = 1;		/**<Timeout Bluetooth; 0:OK 1:TimeOut */
uint8_t BTCount   = 0;		/**<Counting up, when receiving data */
// Software timer
uint32_t TimerWatchRC; /**<Timer for RC watchdog*/

//Joystick values (range between -1 and 1, 0 in middle position)
QUADROCOPTER_RC_RECEIVE_JOYSTICK_VALUES g_joystick_values =
{
		.throttle=0.0f,
		.rudder=0.0f,
		.elevator=0.0f,
		.aileron=0.0f
};

//Bluetooth control values
QUADROCOPTER_RC_RECEIVE_BLUETOOTH_VALUES g_bluetooth_values =
{
		.throttle=0,
		.heightControl=0,
		.rudder=0.0f,
		.aileron=0.0f,
		.elevator=0.0f
};



/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
/**
 *  \brief Fetches data from the Remote Control
 *  
 *  \param [in] power Pointer to variable for throttle value
 *  \param [in] height_control Pointer to variable for Height control
 *  \param [in] yaw_dot Pointer to variable for value of yaw_dot
 *  \param [in] pitch Pointer to variable for the value of pitch
 *  \param [in] roll Pointer to variable for the value of roll
 *  
 *  \details Starts taking actions after checking if the connection should be per bluetooth or remote control
 */
void GetRCData(float* power,uint8_t* height_control, float* yaw_dot, float* pitch, float* roll)
{
#ifdef BT_ONLY		//BT only controlled (no RC needed)
		if (BTTimeOut)
		{
			*power   = 0.0f;
			*yaw_dot = 0.0f;
			*pitch   = 0.0f;
			*roll   = 0.0f;
		}
		else
		{
			*power =(float) g_bluetooth_values.throttle;
			*height_control = g_bluetooth_values.heightControl;
			*pitch = g_bluetooth_values.elevator;
			*roll = -g_bluetooth_values.aileron;
			*yaw_dot = g_bluetooth_values.rudder;
			if (*pitch > 30.0f)
			{
				*pitch = 30.0f;
			}
			else if (*pitch < -30.0f)
			{
				*pitch = -30.0f;
			}

			if (*roll > 30.0f)
			{
				*roll = 30.0f;
			}
			else if (*roll < -30.0f)
			{
				*roll = -30.0f;
			}
		}

#else		//for RC Control with switch to BT Control
	if (flightmode == 0)
		{
			if (RCTimeOut)
			{
				*power   = 0.0f;
				*yaw_dot = 0.0f;
				*pitch   = 0.0f;
				*roll    = 0.0f;
			}
			else
			{
				*power = SCALE_POWER * g_joystick_values.throttle;
				if (g_joystick_values.rudder > 0.01f || g_joystick_values.rudder < -0.01f)
					*yaw_dot = g_joystick_values.rudder * SCALE_YAW;
				else
					*yaw_dot = 0.0f;
					*pitch=g_joystick_values.elevator*SCALE_PITCH;
					*roll=-g_joystick_values.aileron*SCALE_ROLL;
			}
		}
		else
		{
			if (BTTimeOut || RCTimeOut)
			{
				*power   = 0.0f;
				*yaw_dot = 0.0f;
				*pitch   = 0.0f;
				*roll    = 0.0f;
			}
			else
			{
				*power =(float) g_bluetooth_values.throttle;
				*height_control = g_bluetooth_values.heightControl;
				*pitch = g_bluetooth_values.elevator;
				*roll = -g_bluetooth_values.aileron;
				*yaw_dot = g_bluetooth_values.rudder;
				if (*pitch > 30.0f)
				{
					*pitch = 30.0f;
				}
				else if (*pitch < -30.0f)
				{
					*pitch = -30.0f;
				}

				if (*roll > 30.0f)
				{
					*roll = 30.0f;
				}
				else if (*roll < -30.0f)
				{
					*roll = -30.0f;
				}
			}
		}
#endif
}

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
 /**
 *  \brief Function for merging two Bytes into one Integer value
 *  
 *  \param [in] highBytes Variable which should embrace the high Bytes
 *  \param [in] lowBytes Variable which should embrace the low Bytes
 *  \return uint16_t variable with merged Bytes
 *  
 *  \details This is required for extracting two Bytes of the data frame into one variable
 */
int mergeBytes(uint8_t highBytes, uint8_t lowBytes)
{
	int c = highBytes;
	return (c << 8) | lowBytes;
}


 /**
 *  \brief Timer Interrupt-Service-Routine for Bluetooth Keep-Alive Messages
 *  
 *  
 *  \details This function is for messaging the connected device that the connection is alive
 */
void GeneralPurpose_Timer_Bluetooth_Keep_Alive_ISR()
{
	uint8_t keep_alive = 0xFF;
	UART_WriteDataBytes(&Bluetooth_Handle, &keep_alive, 1);
}

 /**
 *  \brief Interrupt-Service-Routine from UART FIFO in buffer
 *  
 *  
 *  \details When buffer is full, new data is ready to read
 */
void RemoteControl_RX_ISR()
{
	//Raw values from RC
	int throttleRaw;
	int rudderRaw;
	int elevatorRaw;
	int aileronRaw;
	int flightmodeRaw;

	uint8_t ReadBufRC[32]; //Readbuffer
	int start = 0; //Index of start byte (contains 0x30)


	//Read data from UART buffer
	UART_ReadDataBytes(&RemoteControl_Handle, ReadBufRC, 32);

	//Search for start byte and check static values
	while (ReadBufRC[start] != 0x30 || ReadBufRC[start + 1] != 0x00
			|| ReadBufRC[start + 5] != 0xA2 || ReadBufRC[start + 8] != 0x2B
			|| ReadBufRC[start + 9] != 0xFE)
	{
		if (start++ > 16)
		{
			//Communication check bytes not in buffer
			return;
		}
	}
	//Get data from stream
	//get raw values
	throttleRaw = mergeBytes(ReadBufRC[start + 2], ReadBufRC[start + 3]);
	aileronRaw = mergeBytes(ReadBufRC[start + 6], ReadBufRC[start + 7]);
	elevatorRaw = mergeBytes(ReadBufRC[start + 10], ReadBufRC[start + 11]);
	rudderRaw = mergeBytes(ReadBufRC[start + 14], ReadBufRC[start + 15]);
	flightmodeRaw = mergeBytes(ReadBufRC[start + 12],ReadBufRC[start + 13]);
	//remap raw values to control values
	g_joystick_values.throttle = map(throttleRaw, THROTTLE_MIN, THROTTLE_MAX, 0, 60000)/ 60000.0f;
	g_joystick_values.aileron = map(aileronRaw, AILERON_MIN, AILERON_MAX, -30000, 30000)/ 30000.0f;
	g_joystick_values.elevator = map(elevatorRaw, ELEVATOR_MIN, ELEVATOR_MAX, -30000, 30000)/ 30000.0f;
	g_joystick_values.rudder = map(rudderRaw, RUDDER_MIN, RUDDER_MAX, -30000, 30000)/ 30000.0f;

	if (flightmodeRaw == FLIGHTMODE0)
		flightmode = 0;
	if (flightmodeRaw == FLIGHTMODE1)
		flightmode = 1;
	//Set values for RC Timeout check
	RCTimeOut = 0;
	RCCount++;

}
/**
 *  \brief Interrupt-Service-Routine for receiving Bluetooth Bytes
 *  
 *  
 *  \details reads data bytes, analyzes the data and checks if error occurs
 *  \details if errors occur -> motors are stopped
 */
void Bluetooth_RX_ISR()
{
	UART_ReadDataBytes(&Bluetooth_Handle, ReadBufBT, PACKET_SIZE);
	BTTimeOut = 0;
	BTCount++;
	int32_t rec_mode = maintainBluetoothInputBuffer(ReadBufBT,&control_value, &dpacket);
	switch (rec_mode)
	{
	case RECEIVED_CONTROL_PACKET:
		g_bluetooth_values.throttle = (uint16_t)*control_value.speed;
		g_bluetooth_values.heightControl = *control_value.height_control;
		g_bluetooth_values.aileron = (*control_value.x_pitch);
		g_bluetooth_values.elevator = (-*control_value.y_roll);
		g_bluetooth_values.rudder = (*control_value.z_rotate);
		break;
	case RECEIVED_DATA_PACKET:
		break;
	case CHECKSUM_ERROR:
		g_bluetooth_values.throttle = 0;
		g_bluetooth_values.aileron  = 0.0f;
		g_bluetooth_values.elevator = 0.0f;
		g_bluetooth_values.rudder   = 0.0f;
		XMC_USIC_CH_RXFIFO_Flush(Bluetooth_Handle.channel);
		//Todo: Add Error-Handling
		break;
	case UNDEFINED_ERROR:
		g_bluetooth_values.throttle = 0;
		g_bluetooth_values.aileron  = 0.0f;
		g_bluetooth_values.elevator = 0.0f;
		g_bluetooth_values.rudder   = 0.0f;
		XMC_USIC_CH_RXFIFO_Flush(Bluetooth_Handle.channel);
		//Todo: Add Error-Handling
		break;
	}
}


 /**
 *  \brief This function is for checking if new data has been arrived since last function call
 *  
 *  \param [in] Temp Needed due to SYSTIMER-App - but not in use 
 *  
 *  \details This function is called from software Timer "TimerWatchRC"
 *  \details
 */
void WatchRC(void* Temp)
{
	static uint8_t lastCount;
	static uint8_t lastBTCount;

	if (lastCount == RCCount)
		RCTimeOut = 1;
	lastCount = RCCount;

	if (lastBTCount == BTCount)
	{
		BTTimeOut = 1;
		XMC_USIC_CH_RXFIFO_Flush(Bluetooth_Handle.channel);
	}
	lastBTCount = BTCount;
}


 /**
 *  \brief Initialize RC watchdog
 *  
 *  
 *  \details Timer overflows every 0.2 seconds
 *  \details Further it's been checked if the timer is running
 */
void WatchRC_Init()
{
	TimerWatchRC = (uint32_t)SYSTIMER_CreateTimer(200000,SYSTIMER_MODE_PERIODIC,(void*)WatchRC,NULL);
	if (TimerWatchRC != 0U)
	{
		//Timer is created successfully
		// Start/Run Software Timer
		if (SYSTIMER_StartTimer(TimerWatchRC) == SYSTIMER_STATUS_SUCCESS)
		{
			// Timer is running
		}
	}
}
/**
 *  \brief Getter-function for getting the value of RCCount
 *  
 *  \return Return value type is uint8_t
 *  \return Return value is the value of the variable "RCCount"
 *  
 *  \details The variable RCCount is used for TimeOut 
 */
uint8_t GetRCCount()
{
	return RCCount;
}


/**
 *  \brief Function for converting a value within a range to the same value according to the target range
 *  
 *  \param [in] x Value to be scaled
 *  \param [in] in_min Minimum of the first value range
 *  \param [in] in_max Maximum of the first value range
 *  \param [in] out_min Minimum of the second value range 
 *  \param [in] out_max Maximum of the second value range
 *  
 *  \return if x==in_min it returns out_min
 * 	\return if x==in_max it returns out_max
 *  
 *  \details This function is used to scale the value of x from the value range [in_min ... in_max] to the value range [out_min ... out_max]
 */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
