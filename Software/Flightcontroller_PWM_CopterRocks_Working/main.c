/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** AM           Andreas Mark                                                  **
** DW           Dominik Wieland                                               **
** ES           Elias Steurer                                                 **
**                                                                            **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.0 : 25-12-2014, AM:  Initial Version
 * V0.0*: 15-08-2015, ES:  Minor Code-Changes
 * V0.1 : 21-07-2016, DW:  Port of SW from DAVE3 to DAVE4
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
	while(1U)
	{
	}
}
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
	static uint8_t USB_RX_Buffer[20];
	static uint8_t USB_TX_Buffer[20];

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
			break;
		case '2':
			break;
		case '3':
			break;
		case '4':
			break;
		case '5':
			break;
		case '6':
			break;
		case '7':
			break;
		case '8':
			break;
		case '9':
			break;
		}
		USBD_VCOM_SendString((const int8_t *) USB_TX_Buffer);
	}

	CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
	//------------------------------------------------------------------------------------------------------------------------------
}
