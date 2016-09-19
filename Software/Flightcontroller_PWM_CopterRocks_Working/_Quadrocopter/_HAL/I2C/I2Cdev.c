/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** RT           Raffael Tschinder                                             **
** DW           Dominik Wieland                                               **
** ES           Elias 	Steurer                                               **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.0: 28-04-2016, RT:  Initial Version
 * V0.1: 21-07-2016, DW:  Port of SW from DAVE3 to DAVE4
 */


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "I2Cdev.h"


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
 *  \brief I2C with FIFO-Buffer initialized using the XMC-Library
 *  
 *  \param [in] i2c_channel Constant pointer to the USIC communication channel
 *  \param [in] usic_sda_output USIC Data-PIN
 *  \param [in] usic_scl_output USIC Clock-PIN
 *  \param [in] port_config Constant pointer to the GPIO-Port
 *  \param [in] pin_number_sda PIN-number of data-PIN
 *  \param [in] pin_number_scl PIN-number of clock-PIN
 *  \param [in] gpio_config_sda Constant pointer to GPIO channel structure of the data-PIN
 *  \param [in] gpio_config_scl Constant pointer to GPIO channel structure of the clock-PIN
 *  \param [in] pin_config Constant pointer to I2C channel structure
 *  
 *  
 *  \details Initializes the I2C channel and the FIFO-Buffer
 *  \details Pin-Configuration is being done
 *  \details FIFO size: 32 words
 */
void setupI2CInterfaceFIFO(XMC_USIC_CH_t *const i2c_channel,const uint8_t usic_sda_output,const uint8_t usic_scl_output, XMC_GPIO_PORT_t *const port_config,const uint8_t pin_number_sda,const uint8_t pin_number_scl,
		const XMC_GPIO_CONFIG_t *const gpio_config_sda,const XMC_GPIO_CONFIG_t *const gpio_config_scl,const XMC_I2C_CH_CONFIG_t *const pin_config)
{
	XMC_I2C_CH_Init(i2c_channel,pin_config);
	XMC_I2C_CH_SetInputSource(i2c_channel, XMC_I2C_CH_INPUT_SDA, usic_sda_output);
	XMC_I2C_CH_SetInputSource(i2c_channel, XMC_I2C_CH_INPUT_SCL, usic_scl_output);
	XMC_USIC_CH_RXFIFO_Configure(i2c_channel,0,XMC_USIC_CH_FIFO_SIZE_32WORDS,0);
	XMC_I2C_CH_Start(i2c_channel);
	//Pin-Configuration
	XMC_GPIO_Init(port_config, (uint8_t)pin_number_sda, gpio_config_sda);
	XMC_GPIO_Init(port_config, (uint8_t)pin_number_scl, gpio_config_scl);
}
/**
 *  \brief I2C interface initialized using the XMC-Library
 *  
 *  \param [in] i2c_channel Constant pointer to the USIC communication channel
 *  \param [in] usic_sda_output USIC Data-PIN
 *  \param [in] usic_scl_output USIC Clock-PIN
 *  \param [in] port_config Constant pointer to the GPIO-Port
 *  \param [in] pin_number_sda PIN-number of data-PIN
 *  \param [in] pin_number_scl PIN-number of clock-PIN
 *  \param [in] gpio_config_sda Constant pointer to GPIO channel structure of the data-PIN
 *  \param [in] gpio_config_scl Constant pointer to GPIO channel structure of the clock-PIN
 *  \param [in] pin_config Constant pointer to I2C channel structure
 * 
 *  
 *  \details Initializes the I2C channel
 *  \details Pin-Configuration is being done
 *  \details NO FIFO-Buffer is included!
 */
void setupI2CInterface(XMC_USIC_CH_t *const i2c_channel,const uint8_t usic_sda_output,const uint8_t usic_scl_output, XMC_GPIO_PORT_t *const port_config,const uint8_t pin_number_sda,const uint8_t pin_number_scl,
		const XMC_GPIO_CONFIG_t *const gpio_config_sda,const XMC_GPIO_CONFIG_t *const gpio_config_scl,const XMC_I2C_CH_CONFIG_t *const pin_config)
{
	XMC_I2C_CH_Init(i2c_channel,pin_config);
	XMC_I2C_CH_SetInputSource(i2c_channel, XMC_I2C_CH_INPUT_SDA, usic_sda_output);
	XMC_I2C_CH_SetInputSource(i2c_channel, XMC_I2C_CH_INPUT_SCL, usic_scl_output);
	XMC_I2C_CH_Start(i2c_channel);
	//Pin-Configuration
	XMC_GPIO_Init(port_config, (uint8_t)pin_number_sda, gpio_config_sda);
	XMC_GPIO_Init(port_config, (uint8_t)pin_number_scl, gpio_config_scl);
}
/**
 *  \brief Function for writing a byte into a register of an I2C device using the XMC-Library
 *  \param [in] handle pointer to USIC channel structure
 *  \param [in] devAddr Contains the device address (slave address)
 *  \param [in] regAddr Contains the address of the desired register
 *  \param [in] data Data to be transmitted
 *  \return Return type is bool
 *  \return Returned value is always TRUE at the end
 *  
 *  \details Writes a byte into a register of an I2C device
 */
bool I2Cdev_writeByte(XMC_USIC_CH_t* handle,uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
	XMC_I2C_CH_MasterStart(handle, devAddr, XMC_I2C_CH_CMD_WRITE);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
	//---------------------------------------------------------------------------------------------------------------------------------
	XMC_I2C_CH_MasterTransmit(handle, regAddr);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

	XMC_I2C_CH_MasterTransmit(handle, data);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

	XMC_I2C_CH_MasterStop(handle);
	return true;
}

/**
 *  \brief Function for reading a byte out of a register of an I2C device using the XMC-Library
 *  
 *  \param [in] handle pointer to USIC channel structure
 *  \param [in] devAddr Contains the device address (slave address)
 *  \param [in] regAddr Contains the address of the desired starting register
 *  \return Return type is int16_t
 *  \return Returned value is the data of a desired register address
 *  
 *  \details Reads a byte out of a register from an I2C device
 */
int16_t I2Cdev_readByte(XMC_USIC_CH_t* handle, uint8_t devAddr, uint8_t regAddr)
{
	XMC_I2C_CH_MasterStart(handle, devAddr, XMC_I2C_CH_CMD_WRITE);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);
	//---------------------------------------------------------------------------------------------------------------------------------
	XMC_I2C_CH_MasterTransmit(handle, regAddr);

	XMC_I2C_CH_MasterRepeatedStart(handle, devAddr,XMC_I2C_CH_CMD_READ);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

	XMC_I2C_CH_MasterReceiveNack(handle);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & (XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);

	uint8_t received_data = 0;
	received_data = XMC_I2C_CH_GetReceivedData(handle);

	XMC_I2C_CH_MasterStop(handle);

	return (int16_t)received_data;
}

/**
 *  \brief Function for reading an amount of bytes out of a register of an I2C device - with assistance of the XMC-Library
 *  
 *  \param [in] handle Constant pointer to USIC channel structure
 *  \param [in] devAddr Contains the device address (master address)
 *  \param [in] regAddr Contains the address of the desired register
 *  \param [in] length The number of bytes to be read
 *  \param [in] data Pointer to the variable for saving the read data
 *  \return Return type is int16_t
 *  \return Returned value is the amount of successfully read bytes
 *  
 *  \details Reads data (more bytes) from a register of an I2C device
 *  \details You can choose the start address and the amount of bytes to be read
 */
int16_t I2Cdev_readBytes(XMC_USIC_CH_t* handle,uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{
	XMC_I2C_CH_MasterStart(handle, devAddr, XMC_I2C_CH_CMD_WRITE);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

	XMC_I2C_CH_MasterTransmit(handle, regAddr);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

	//---------------------------------------------------------------------------------------------------------------------------------
	XMC_I2C_CH_MasterRepeatedStart(handle, devAddr,XMC_I2C_CH_CMD_READ);
	while ((XMC_I2C_CH_GetStatusFlag(handle) & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0U);
	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED);

	int16_t fifo_bytes;
	for(fifo_bytes = 0;fifo_bytes<length;fifo_bytes++)
	{
		if(fifo_bytes < (length-1))
			XMC_I2C_CH_MasterReceiveAck(handle);
		else
			XMC_I2C_CH_MasterReceiveNack(handle);
		while ((XMC_I2C_CH_GetStatusFlag(handle) & (XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) == 0U);
		XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);
	}

	XMC_I2C_CH_MasterStop(handle);

	fifo_bytes=0;
	while(!XMC_USIC_CH_RXFIFO_IsEmpty(handle))
	{
		data[fifo_bytes] = XMC_I2C_CH_GetReceivedData(handle);
		fifo_bytes++;
	}

	XMC_I2C_CH_ClearStatusFlag(handle,XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);
	return fifo_bytes;
}


/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
