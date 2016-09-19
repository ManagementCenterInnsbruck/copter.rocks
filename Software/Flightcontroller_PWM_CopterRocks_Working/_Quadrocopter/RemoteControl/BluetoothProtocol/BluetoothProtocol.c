/* *****************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** SE           Steurer Elias                                                 **
**                                                                            **
**                                                                            **
***************************************************************************** */

/* *****************************************************************************
**                      Revision Control History                              **
***************************************************************************** */
/* 
 * V0.0: 30.01.2015, SE   :  Initial Version
 * V0.1: 08.04.2016, SE   :  modified Version
  */
 
 
/* *****************************************************************************
**                      Includes                                              **
***************************************************************************** */
 #include "BluetoothProtocol.h"
/* *****************************************************************************
**                      Private Constant Definitions to be changed            **
***************************************************************************** */

/* *****************************************************************************
**                      Private Macro Definitions                             **
***************************************************************************** */

/* *****************************************************************************
**                      Global Type Definitions                               **
***************************************************************************** */

/* *****************************************************************************
**                      Private Type Definitions                              **
***************************************************************************** */

/* *****************************************************************************
**                      Global Function Declarations                          **
***************************************************************************** */

/* *****************************************************************************
**                      Private Function Declarations                         **
***************************************************************************** */

/* *****************************************************************************
**                      Global Constant Definitions                           **
***************************************************************************** */

/* *****************************************************************************
**                      Private Constant Definitions                          **
***************************************************************************** */

/* *****************************************************************************
**                      Global Variable Definitions                           **
***************************************************************************** */

/* *****************************************************************************
**                      Private Variable Definitions                          **
***************************************************************************** */
ControlValue control_value;	/**< struct full of pointers for Bluetooth receiving */
DataPacket dpacket;			/**< struct for received data */
uint8_t* ReadBufBT;			/**< Pointer to Bluetooth read Buffer location */
/* *****************************************************************************
**                      Global Function Definitions                           **
***************************************************************************** */
/**
 *  \brief Allocates storage for Bluetooth
 *  
 *  \details alllocates storage for ReadBufBT and all control_value components
 */
void initBluetoothStorage()
{
	ReadBufBT = malloc(sizeof(uint8_t)*BLUETOOTH_INPUT_BUFFER);
	control_value.header = malloc(sizeof(uint8_t));
	control_value.height_control = malloc(sizeof(uint8_t));
	control_value.speed = malloc(sizeof(uint8_t));
	control_value.x_pitch = malloc(sizeof(float));
	control_value.y_roll = malloc(sizeof(float));
	control_value.z_rotate = malloc(sizeof(float));
	control_value.checksum = malloc(sizeof(uint32_t));
}

/**
 *  \brief receives the Bluetooth packages
 *  
 *  \param [in] input_buffer 
 *  \param [in] control_value used as return for control values
 *  \param [in] packet Parameter_Description
 *  \return returns if reading was successful and gives information about received package
 *  
 *  \details processes received bytes from Bluetooth-UART Module and assigns it to corresponding remote-control<br>
 *  values.
 */
int32_t maintainBluetoothInputBuffer(
		uint8_t input_buffer[BLUETOOTH_INPUT_BUFFER],
		ControlValue *control_value, DataPacket* packet)
{
	static int packet_counter = 0;
	static int packets_to_be_received = 0;
	uint32_t checksum;
	checksum = input_buffer[0];
	checksum ^= ((input_buffer[1] << 8 | input_buffer[2]) & 0xFFFF);
	for (int i = 3; i < (PACKET_SIZE - PACKET_CHECKSUM) - 1; i += 4)
	{
		checksum ^= (input_buffer[i] << 24 | input_buffer[i + 1] << 16
				| input_buffer[i + 2] << 8 | input_buffer[i + 3]);
	}
	for(int j = 0;j < 4;j++)
	{
		*((uint8_t*)(control_value->checksum) + 3-j) = input_buffer[15+j];
	}
	control_value->header = (uint8_t*)&input_buffer[0];
	if (*(control_value->checksum) == checksum)//received checksum is the same as calculated checksum
	{
		if (*control_value->header)//Got a valid Data-Package
			{
			if (packets_to_be_received == 0)
			{
				packets_to_be_received = input_buffer[0];
			}
			int current_val = packet_counter;
			for (; packet_counter < (current_val + DATA_SIZE);
				packet_counter++)
			{
				packet->cmd[packet_counter] = input_buffer[packet_counter
						- current_val + PACKET_HEADER];
			}
			if ((input_buffer[0]
					<= (PACKET_SIZE - PACKET_HEADER - PACKET_CHECKSUM)))
			{ //all cmd-bytes are within this data-package
				packet->character_count = packets_to_be_received;
				packet_counter = 0;
				packets_to_be_received = 0;
				//Cleanup Buffer//
				memset(input_buffer, 0x00,PACKET_SIZE);
				return RECEIVED_DATA_PACKET; //Got a complete and valid Data-Package
			} else {
				memset(input_buffer, 0x00,PACKET_SIZE);
				return RECEIVED_DATA_PACKET_N_C; //must do another read operation
			}
		} else
		{
			*((uint8_t*)(control_value->height_control)) = input_buffer[1];
			*((uint8_t*)(control_value->speed)) = input_buffer[2];
			for(int k = 0;k < 4;k++)
			{
				*((uint8_t*)(control_value->z_rotate) + 3-k) = input_buffer[3+k];
				*((uint8_t*)(control_value->x_pitch) + 3-k) = input_buffer[7+k];
				*((uint8_t*)(control_value->y_roll) + 3-k) = input_buffer[11+k];
			}
			return RECEIVED_CONTROL_PACKET; //Got a valid Control-Package
		}
	} else {
			packet_counter = 0; //if it happens between data-packages...
			//Cleanup Buffer//
			memset(input_buffer, 0x00,PACKET_SIZE);
			return CHECKSUM_ERROR; //Wrong checksum
	}
	return UNDEFINED_ERROR; //error by default but in fact never reachable
}
/* *****************************************************************************
**                      Private Function Definitions                          **
***************************************************************************** */
 
 



