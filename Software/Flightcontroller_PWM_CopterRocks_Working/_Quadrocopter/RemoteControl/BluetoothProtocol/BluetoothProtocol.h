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
 * V0.0: 28.01.2015, SE   :  Initial Version
  */
 
 
/* *****************************************************************************
**                      Includes                                              **
***************************************************************************** */
#include <DAVE.h>
#include <stdlib.h>
#include <string.h>

#ifndef SERIAL_CMD_H_
#define SERIAL_CMD_H_
/* *****************************************************************************
**                      Private Constant Definitions to be changed            **
***************************************************************************** */

/* *****************************************************************************
**                      Private Macro Definitions                             **
***************************************************************************** */
#define BLUETOOTH_INPUT_BUFFER 32 	/**< bluetooth input buffer size */
#define HARDWARE_BUFFER_SIZE 32		/**< Hardware buffer size */
#define HARDWARE_BUFFER_WORD_SIZE 8	/**<  */
#define PACKET_SIZE 19				/**< size of each Bluetooth Packet */
#define PACKET_HEADER 1				/**< size of the header of a Bluetooth Packet */
#define PACKET_CHECKSUM 4			/**< how many checksum bytes */
#define DATA_SIZE 14				/**< how many bytes are used for a data package */

#define RECEIVED_DATA_PACKET 1		/**< received a data packet */
#define RECEIVED_DATA_PACKET_N_C 2	/**< received a part of a data packet, another read-operation must be done */
#define RECEIVED_CONTROL_PACKET 0	/**< received control packet */
#define CHECKSUM_ERROR -1			/**< error handling: wrong checksum */
#define UNDEFINED_ERROR -2			/**< error handling: undefined error */
/* *****************************************************************************
**                      Global Type Definitions                               **
***************************************************************************** */
/**
 * @brief Structure for one data packet
 */
typedef struct _DATA_PACKET_
{
uint8_t cmd[DATA_SIZE*3];	/**< array for Data */
uint8_t character_count;	/**< counter for characters */
}DataPacket;		
/**
 * @brief Structure with pointers to the diffrent parts of the received packet
 */
typedef struct _CONTROL_VALUE_
{
uint8_t* header;		/**< pointer to the header */
uint8_t* height_control;/**< pointer to the height_control */
uint8_t* speed;			/**< pointer to the speed value */
float* z_rotate;		/**< pointer to the rotation value */
float* x_pitch;			/**< pointer to the pitch value */
float* y_roll;			/**< pointer to the roll value */
uint32_t* checksum;		/**< 4 bytes checksum */
}ControlValue;
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

/* *****************************************************************************
**                      Global Function Definitions                           **
***************************************************************************** */
void initBluetoothStorage();
int32_t maintainBluetoothInputBuffer(uint8_t input_buffer[BLUETOOTH_INPUT_BUFFER],ControlValue *control_value, DataPacket* packet);
/* *****************************************************************************
**                      Private Function Definitions                          **
***************************************************************************** */
#endif /* SERIAL_CMD_H_ */
