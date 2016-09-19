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


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "AttitudeController.h"

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
extern const ADC_MEASUREMENT_ISR_t backgnd_rs_intr_handle; /**< global ADC-NVIC struct  */

CONTROLLERPOLYNOMIALS polynoms;/**< global structure for controler polynomials*/

//DPS3100 Pressure-Sensor
float pressure = 0;/**< global variable for storing current pressure value*/
float temperature = 0;/**< global variable for storing current temperature value*/
struct structFIR PressureFIR;/**< global struct for filtering pressure values*/

float powerD = 0.0f;/* throttle value of remote, range: 0 - 100*/
float yawD_dot = 0.0f;/* yaw value of remote, range: -90 - +90*/
float pitchD = 0.0f;/* pitchD value of remote, range: -30 - +30*/
float rollD = 0.0f;/* rollD value of remote, range: -30 - +30*/
float YPR[3];
float u_yaw_dot = 0.0f;/* output of angle-rate controller for yaw*/
float u_pitch = 0.0f;/* output of angle controller for pitch*/
float u_roll = 0.0f;/* output of angle controller for roll*/
float PWM_percent[4];/* scaled motor power from controller output, range: 0 - 100*/

#ifdef CopterRocks
CONTROLLERPARAMETER parameter =
{
		.T=0.002f,
		.P_roll=30.0f,
		.I_roll=0.2f,
		.D_roll=1.0f,			//1.1f for BlHeli / 1.0f for standard CopterRocks
		.N_roll=400.0,
		.P_pitch=30.0,
		.I_pitch=0.2f,
		.D_pitch=1.0f, 			//1.1f for BlHeli / 1.0f for standard CopterRocks
		.N_pitch=400.0f,
		.P_yaw=100.0f
};
#endif


/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/

/**
 *  \brief Interrupt-Service-Routine of the Control_Timer
 *
 *  \details Central Control Routine for Quadcopter\n
 *  This routine gets the momentary position and the desired position \n
 *  and calculates new output for motors and transmits it via DaisyChain to the \n
 *  Electric Speed Controller
 *
 */

void Control_Timer_ISR(void)
{


	static float x_pitch[CONTROL_ORDER];
	static float x_roll[CONTROL_ORDER];


	uint32_t PWM_APP_OUTPUT[4]={0,0,0,0};
	uint8_t height_control = 0;/* for enabling height-control, activated: 0xff, disabled: 0x00*/

	GetAngles(YPR);
	GetRCData(&powerD, &height_control, &yawD_dot, &pitchD, &rollD);
	//yaw control
	AngleRateController(&yawD_dot, &YPR[0], &parameter.P_yaw, &u_yaw_dot);
	//pitch control
	AngleController(&pitchD, &YPR[1], CONTROL_ORDER, polynoms.a_pitch, polynoms.b_pitch, x_pitch, &u_pitch);
	//roll control
	AngleController(&rollD, &YPR[2], CONTROL_ORDER, polynoms.a_roll, polynoms.b_roll, x_roll, &u_roll);
	//generate actuator values
	CreatePulseWidth(&u_roll, &u_pitch, &u_yaw_dot, &powerD, PWM_percent);

	if (powerD > 5.0f){
		PWM_percent[0]=0.45*PWM_percent[0]+45;
		PWM_percent[1]=0.45*PWM_percent[1]+45;
		PWM_percent[2]=0.45*PWM_percent[2]+45;
		PWM_percent[3]=0.45*PWM_percent[3]+45;

		PWM_APP_OUTPUT[0]= (uint32_t) (PWM_percent[0]*100u);
		PWM_APP_OUTPUT[1]= (uint32_t) (PWM_percent[1]*100u);
		PWM_APP_OUTPUT[2]= (uint32_t) (PWM_percent[2]*100u);
		PWM_APP_OUTPUT[3]= (uint32_t) (PWM_percent[3]*100u);

		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_1,PWM_APP_OUTPUT[2]);
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_2,PWM_APP_OUTPUT[0]);
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_3,PWM_APP_OUTPUT[1]);
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_4,PWM_APP_OUTPUT[3]);
	}else
	{
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_1,4500u);
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_2,4500u);
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_3,4500u);
		PWM_CCU4_SetDutyCycle(&PWM_OUTPUT_4,4500u);
	}

}

/**
 *  \brief PID controller for angle based quadcopter stabilization
 *  
 *  \param [in] r Reference inputs (RC)
 *  \param [in] y Current output value of the sensor
 *  \param [in] n Degree of control polynomial
 *  \param [in] a Coefficient of the numerator and denominator polynomials of the controller
 *  \param [in] b Coefficient of the numerator and denominator polynomials of the controller
 *  \param [in] x
 *  \param [in] u Controller output
 *  
 */
void AngleController(float *r, float *y, int n, const float *a, const float *b,float *x, float *u)
{
	//PID-Controller

	//control error
	float e = (*r - *y) * (float)M_PI / 180.0f;

	//calculate plant input
	*u = x[n - 1] + b[n] * e;

	//calculate new coefficients
	for (int i = n - 1; i > 0; i--)
		x[i] = b[i] * e - a[i] * (*u) + x[i - 1];

	x[0] = b[0] * e - a[0] * (*u);

	*u /= 4.0f;
}
/**
 *  \brief This function represents the proportional controller for yaw-movement of the quadcopter
 *  
 *  \param [in] r Reference inputs (RC)
 *  \param [in] y Current output value of the sensor
 *  \param [in] P Value of P_yaw
 *  \param [in] u Controller output
 *  
 */
void AngleRateController(float *r, float *y, const float *P, float *u)
{
	//P-Controller
	*u = (*r - *y) * M_PI / (180.0f * 4.0f) * *P;
}
/**
 *  \brief This function calculates and limits the PWM signals of the 4 motors, based on the 3 controller outputs
 *  
 *  \param [in] u_phi Output of third controller (u_roll)
 *  \param [in] u_theta Output of second controller (u_pitch)
 *  \param [in] u_psi_dot Output of third controller (u_yaw_dot)
 *  \param [in] u_hover Power 
 *  \param [in] PWM_width Desired width of the PWM
 *  
 *  \details Roll, pitch and yaw movements are are based on the linearized mathematical model of the system
 */
void CreatePulseWidth(float *u_phi, float *u_theta, float *u_psi_dot,float *u_hover, float *PWM_width)
{
	float saturationMax = 100.0f;
	float saturationMin = 10.0f;

	if (*u_hover > 5.0f)
	{
		PWM_width[0] = -*u_theta + *u_phi - *u_psi_dot + *u_hover;
		PWM_width[1] = -*u_theta - *u_phi + *u_psi_dot + *u_hover;
		PWM_width[2] = *u_theta + *u_phi + *u_psi_dot + *u_hover;
		PWM_width[3] = *u_theta - *u_phi - *u_psi_dot + *u_hover;
	}
	else
	{
		PWM_width[0] = *u_hover;
		PWM_width[1] = *u_hover;
		PWM_width[2] = *u_hover;
		PWM_width[3] = *u_hover;
	}

	if (PWM_width[0] > saturationMax)
		PWM_width[0] = saturationMax;

	if (PWM_width[0] < saturationMin)
		PWM_width[0] = saturationMin;

	if (PWM_width[1] > saturationMax)
		PWM_width[1] = saturationMax;

	if (PWM_width[1] < saturationMin)
		PWM_width[1] = saturationMin;

	if (PWM_width[2] > saturationMax)
		PWM_width[2] = saturationMax;

	if (PWM_width[2] < saturationMin)
		PWM_width[2] = saturationMin;

	if (PWM_width[3] > saturationMax)
		PWM_width[3] = saturationMax;

	if (PWM_width[3] < saturationMin)
		PWM_width[3] = saturationMin;
}

 /**
 *  \brief This function initializes the hardware
 *
 *
 *  \details Sensors are set up, polynomials are calculated and other general preparations are made
 */
void setup(void)
{
	initBluetoothStorage();//initialize space for variables used for Bluetooth Communication
	//-------------------Dave Setup---------------------
	DAVE_STATUS_t status;
	status = DAVE_Init();//Initialization of DAVE APPs
	if (status == DAVE_STATUS_FAILURE)
	{
		/* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
		XMC_DEBUG("DAVE APPs initialization failed\n");
		while (1U);
	}
	disableIRQ();//disables all Interrupts
	delay(2000);//waits 2000ms
	enableIRQ();//enables configurated Interrupts
	setup_STATE_LEDs();//setup Status-LED's
	WatchRC_Init();//initialize RC-Watchdog
	setup_UART_Trigger_limits();//setup Trigger-Limits of UART-FIFO
	PressureFIR = Initialize_FIR_Filter(PressureFIR, MOVING_AVERAGE);//initialize FIR Filter

	//setupDPS310I2C();//initialize DPS310
	//setupDPS310();//setup DPS Hardware
	//getCoefficients();//get Coefficients of DPS310

	MPU9150_Setup();//configures the IMU
	delay(3000);//wait 3000ms to wait for ESC's to startup
	// initialize controller polynomials
	polynoms.b_roll[0]=parameter.P_roll-parameter.I_roll*parameter.T-parameter.P_roll*parameter.N_roll*parameter.T+parameter.N_roll*parameter.I_roll*parameter.T*parameter.T+parameter.D_roll*parameter.N_roll;
	polynoms.b_roll[1]=parameter.I_roll*parameter.T-2*parameter.P_roll+parameter.P_roll*parameter.N_roll*parameter.T-2*parameter.D_roll*parameter.N_roll;
	polynoms.b_roll[2]=parameter.P_roll+parameter.D_roll*parameter.N_roll;
	polynoms.a_roll[0]=1-parameter.N_roll*parameter.T;
	polynoms.a_roll[1]=parameter.N_roll*parameter.T-2;

	polynoms.b_pitch[0]=parameter.P_pitch-parameter.I_pitch*parameter.T-parameter.P_pitch*parameter.N_pitch*parameter.T+parameter.N_pitch*parameter.I_pitch*parameter.T*parameter.T+parameter.D_pitch*parameter.N_pitch;
	polynoms.b_pitch[1]=parameter.I_pitch*parameter.T-2*parameter.P_pitch+parameter.P_pitch*parameter.N_pitch*parameter.T-2*parameter.D_pitch*parameter.N_pitch;
	polynoms.b_pitch[2]=parameter.P_pitch+parameter.D_pitch*parameter.N_pitch;
	polynoms.a_pitch[0]=1-parameter.N_pitch*parameter.T;
	polynoms.a_pitch[1]=parameter.N_pitch*parameter.T-2;

	TIMER_Start(&Control_Timer);//start Timer for Controller
	PWM_CCU4_Start(&PWM_OUTPUT_1);
	PWM_CCU4_Start(&PWM_OUTPUT_2);
	PWM_CCU4_Start(&PWM_OUTPUT_3);
	PWM_CCU4_Start(&PWM_OUTPUT_4);
}
/**
 *  \brief Function for setup of the pins for the Battery-LEDs on the LARIX-Board
 */
void setup_STATE_LEDs(void)
{
	Control_P3_0(OUTPUT_PP_GP, VERYSTRONG);	//Configure Pin 3.0 -->BatteryState1 (See: _Quadrocopter/BatterySafety/BatterySafety.h)
	Control_P3_1(OUTPUT_PP_GP, VERYSTRONG); //Configure Pin 3.1 -->BatteryState2
	Control_P3_2(OUTPUT_PP_GP, VERYSTRONG); //Configure Pin 3.2 -->BatteryState3
}

/**
 *  \brief Function for setting the trigger limits
 *
 *  \details Receive FIFO trigger limit is configured for Bluetooth & the Remote Control
 *  \details When the FIFO filling level rises above the trigger limit -> Interrupt will be generated
 */
void setup_UART_Trigger_limits(void)
{
	UART_SetRXFIFOTriggerLimit(&RemoteControl_Handle, 31);
	UART_SetRXFIFOTriggerLimit(&Bluetooth_Handle, 18);
}
/**
 *  \brief Function for disabling all Interrupt-Service-Routines
 *  *
 *  \details This function disables all Interrupt-Service-Routines\n
 *  except the Util_Timer_ISR
 */
void disableIRQ(void)
{
	INTERRUPT_Disable(&Control_Timer_ISR_Handle);
	INTERRUPT_Disable(&GeneralPurpose_Timer_ISR_Handle);
	INTERRUPT_Disable(&MagnetometerCal_Timer_ISR_Handle);
	NVIC_DisableIRQ(backgnd_rs_intr_handle.node_id);//Disable ADC Interrupt
	INTERRUPT_Disable(&DPS310_Ext_Int_ISR_Handle);
	INTERRUPT_Disable(&MPU9X50_Ext_Int_ISR_Handle);
	INTERRUPT_Disable(&Bluetooth_RX_ISR_Handle);
	INTERRUPT_Disable(&RemoteControl_RX_ISR_Handle);
}
/**
 *  \brief Function for enabling all Interrupt-Service-Routines
 *
 *  \details This function enables all Interrupt-Service-Routines
 */
void enableIRQ(void)
{
	INTERRUPT_Enable(&GeneralPurpose_Timer_ISR_Handle);
	NVIC_EnableIRQ(backgnd_rs_intr_handle.node_id); //Enables ADC Interrupt
	INTERRUPT_Enable(&Control_Timer_ISR_Handle);
    INTERRUPT_Enable(&Bluetooth_RX_ISR_Handle);
    INTERRUPT_Enable(&DPS310_Ext_Int_ISR_Handle);
	INTERRUPT_Enable(&RemoteControl_RX_ISR_Handle);
}
