/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** DW           Dominik Wieland                                               **
**                                                                            **
**                                                                            **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.0: 12-07-2016, DW:  Initial Version
 */


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "UARTdev.h"


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

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
/**
 *  \brief Transmits Data by UART
 *  
 *  \param [in] UART_Handle Pointer to USIC module used for transmission
 *  \param [in] DataPtr Pointer to the Data that needs to be transmitted
 *  \param [in] Count Number of Bytes to transmit
 *  
 *  \details Transmits Data via UART specified by UART_Handle\n
 *  Data transmission is aborted when UART Transmit-FIFO is full.\n
 */
void UART_WriteDataBytes(UART_t* UART_Handle, uint8_t* DataPtr, uint32_t Count)
{
	while (!UART_IsTXFIFOFull(UART_Handle) && Count)
	{
		UART_TransmitWord(UART_Handle, *DataPtr);
		Count--;
		DataPtr++;
	}
}
/**
 *  \brief Reads Data from UART
 *  
 *  \param [in] UART_Handle Pointer to USIC module used for receiving the Data
 *  \param [in] DataPtr Pointer to array to store received data
 *  \param [in] Count Number of Bytes to receive
 *  
 *  \details Reads out and deletes Count Bytes of Receive-FIFO or
 *   as many Bytes as possible when Count>Bytes received
 */
void UART_ReadDataBytes(UART_t* UART_Handle, uint8_t* DataPtr, uint32_t Count)
{
	while (!UART_IsRXFIFOEmpty(UART_Handle) && Count)
	{
		*DataPtr = (uint8_t) UART_GetReceivedWord(UART_Handle);
		Count--;
		DataPtr++;
	}
}

/**
 *  \brief Writes data in the FIFO buffer
 *  
 *  \param [in] UART_Handle Pointer to USIC module used for transmission
 *  \param [in] DataPtr Pointer to the Data that needs to be transmitted
 *  \param [in] Count Number of Bytes to transmit
 *  
 *  \details Transmits Data via UART specified by UART_Handle\n
 *  Data transmission is not aborted when UART Transmit-FIFO is full.\n
 *  Function waits if waits if Transmit-FIFO is full and enters Data afterwards
 */
void UART_WriteDataBuffer(UART_t* UART_Handle, uint8_t* DataPtr, uint32_t Count){
	while (Count)
	{
		while(UART_IsTXFIFOFull(UART_Handle));
		UART_TransmitWord(UART_Handle, *DataPtr);
		Count--;
		DataPtr++;
	}
}

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
