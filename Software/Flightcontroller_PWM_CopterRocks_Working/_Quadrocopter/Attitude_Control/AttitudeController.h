/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** AM           Andreas Mark                                                  **
** DW           Dominik Wieland                                               **
**                                                                            **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.0: 29-12-2014, AM:  Initial Version
 * V0.1: 21-07-2016, DW:  Port of SW from DAVE3 to DAVE4
 */


#ifndef ATTITUDECONTROLLER_H_
#define ATTITUDECONTROLLER_H_

#define CONTROL_ORDER 2

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <math.h>

#include "../_HAL/Delay/util.h"
#include "../_HAL/GPIO.h"
#include "../RemoteControl/RCReceive.h"
#include "../_FIR_Filter/FIR_Filter.h"
#include "../Sensors/DPS310/DPS310.h"
#include "../Sensors/MPU9X50/MPU9250.h"
#include "../_HAL/I2C/I2Cdev.h"
#include "../Attitude_Control/AttitudeController.h"

/*******************************************************************************
**                      Private Constant Definitions to be changed            **
*******************************************************************************/
//Define which Frame is used:
#define CopterRocks
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
void Control_Timer_ISR(void);
void AngleController(float *r, float *y, int n, const float *a, const float *b, float *x, float *u);
void AngleRateController(float *r, float *y, const float *P, float *u);
void CreatePulseWidth(float *u_phi, float *u_deta, float *u_psi_dot, float *u_hover, float *PWM_width);
void setup(void);
void setup_STATE_LEDs(void);
void setup_UART_Trigger_limits(void);
void disableIRQ(void);			//Disables all App-Controlled Interrupts
void enableIRQ(void);			//Enables  all App-Controlled Interrupts


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

/*******************************************************************************
**                      Global Structures Definitions                         **
*******************************************************************************/

/**
 * @brief Structure for Controllerpolynomials
 * 
 */
typedef struct
{
	float a_roll[CONTROL_ORDER];
	float a_pitch[CONTROL_ORDER];
	float b_roll[CONTROL_ORDER+1];
	float b_pitch[CONTROL_ORDER+1];
}CONTROLLERPOLYNOMIALS;


/**
 * @brief Structure for controllerparameter - Controllerparameter depending on frame
 */
typedef struct
{
	const float T;
	const float P_roll;
	const float I_roll;
	const float D_roll;
	const float N_roll;
	const float P_pitch;
	const float I_pitch;
	const float D_pitch;
	const float N_pitch;
	const float P_yaw;
}CONTROLLERPARAMETER;



#endif /* ATTITUDECONTROlLER_H_ */
