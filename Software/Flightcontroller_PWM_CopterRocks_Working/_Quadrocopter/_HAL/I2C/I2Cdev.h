/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** RT           Rafael  Tschinder                                             **
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


#ifndef I2C_E_H_
#define I2C_E_H_


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <xmc_usic.h>
#include <xmc_i2c.h>
#include <xmc_gpio.h>


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
void setupI2CInterfaceFIFO(XMC_USIC_CH_t *const i2c_channel,const uint8_t usic_sda_output,const uint8_t usic_scl_output, XMC_GPIO_PORT_t *const port_config,const uint8_t pin_number_sda,const uint8_t pin_number_scl,
		const XMC_GPIO_CONFIG_t *const gpio_config_sda,const XMC_GPIO_CONFIG_t *const gpio_config_scl,const XMC_I2C_CH_CONFIG_t *const pin_config);
void setupI2CInterface(XMC_USIC_CH_t *const i2c_channel,const uint8_t usic_sda_output,const uint8_t usic_scl_output, XMC_GPIO_PORT_t *const port_config,const uint8_t pin_number_sda,const uint8_t pin_number_scl,
		const XMC_GPIO_CONFIG_t *const gpio_config_sda,const XMC_GPIO_CONFIG_t *const gpio_config_scl,const XMC_I2C_CH_CONFIG_t *const pin_config);
bool 	I2Cdev_writeByte(XMC_USIC_CH_t* handle,uint8_t devAddr, uint8_t regAddr, uint8_t data);
int16_t I2Cdev_readByte (XMC_USIC_CH_t* handle, uint8_t devAddr, uint8_t regAddr);
int16_t I2Cdev_readBytes(XMC_USIC_CH_t* handle,uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);

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

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/

#endif /* I2C_E_H_ */
