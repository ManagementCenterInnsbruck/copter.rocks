
/**
 * @cond
 ***********************************************************************************************************************
 *
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2014-06-16:
 *     - Initial version<br>
 * 2015-08-28:
 *     - Added CLOCK_XMC1_Init conditionally
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "DAVE.h"

/***********************************************************************************************************************
 * API IMPLEMENTATION
 **********************************************************************************************************************/

/*******************************************************************************
 * @brief This function initializes the APPs Init Functions.
 *
 * @param[in]  None
 *
 * @return  DAVE_STATUS_t <BR>
 ******************************************************************************/
DAVE_STATUS_t DAVE_Init(void)
{
  DAVE_STATUS_t init_status;
  
  init_status = DAVE_STATUS_SUCCESS;
     /** @Initialization of APPs Init Functions */
     init_status = (DAVE_STATUS_t)CLOCK_XMC4_Init(&CLOCK_XMC4_0);

  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance Util_Timer */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&Util_Timer); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance Util_Timer_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&Util_Timer_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of UART APP instance Bluetooth_Handle */
	 init_status = (DAVE_STATUS_t)UART_Init(&Bluetooth_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance Bluetooth_RX_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&Bluetooth_RX_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of UART APP instance RemoteControl_Handle */
	 init_status = (DAVE_STATUS_t)UART_Init(&RemoteControl_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance RemoteControl_RX_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&RemoteControl_RX_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_DETECTOR APP instance DPS310_ERU_In */
	 init_status = (DAVE_STATUS_t)EVENT_DETECTOR_Init(&DPS310_ERU_In); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_GENERATOR APP instance DPS310_ERU_Out */
	 init_status = (DAVE_STATUS_t)EVENT_GENERATOR_Init(&DPS310_ERU_Out); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance DPS310_Ext_Int_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&DPS310_Ext_Int_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DPS310_Ext_Int_Pin */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DPS310_Ext_Int_Pin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_DETECTOR APP instance MPU9X50_ERU_In */
	 init_status = (DAVE_STATUS_t)EVENT_DETECTOR_Init(&MPU9X50_ERU_In); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance MPU9X50_Ext_Int_Pin */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&MPU9X50_Ext_Int_Pin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_GENERATOR APP instance MPU9X50_ERU_Out */
	 init_status = (DAVE_STATUS_t)EVENT_GENERATOR_Init(&MPU9X50_ERU_Out); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance MPU9X50_Ext_Int_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&MPU9X50_Ext_Int_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT APP instance VBat_Measurement_Handle */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_Init(&VBat_Measurement_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance Control_Timer */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&Control_Timer); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance Control_Timer_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&Control_Timer_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of USBD_VCOM APP instance USBD_VCOM_0 */
	 init_status = (DAVE_STATUS_t)USBD_VCOM_Init(&USBD_VCOM_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance MagnetometerCal_Timer */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&MagnetometerCal_Timer); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance GeneralPurpose_Timer */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&GeneralPurpose_Timer); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance MagnetometerCal_Timer_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&MagnetometerCal_Timer_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance GeneralPurpose_Timer_ISR_Handle */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&GeneralPurpose_Timer_ISR_Handle); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of SYSTIMER APP instance RC_Watch_Timer */
	 init_status = (DAVE_STATUS_t)SYSTIMER_Init(&RC_Watch_Timer); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance PWM_OUTPUT_1 */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&PWM_OUTPUT_1); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance PWM_OUTPUT_2 */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&PWM_OUTPUT_2); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance PWM_OUTPUT_3 */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&PWM_OUTPUT_3); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance PWM_OUTPUT_4 */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&PWM_OUTPUT_4); 
   }  
  return init_status;
} /**  End of function DAVE_Init */

