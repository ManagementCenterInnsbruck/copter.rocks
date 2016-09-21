/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** AM           Andreas Mark                                                  **
**                                                                            **
**                                                                            **
*******************************************************************************/

/* *****************************************************************************
**                      Revision Control History                              **
***************************************************************************** */
/* 
 * V0.0: 03.04.2015,MAAN:  Initial Version
  */
/* *****************************************************************************
**                      Includes                                              **
***************************************************************************** */
#include "MPU9250.h"
/* *****************************************************************************
**                      Private Constant Definitions to be changed            **
***************************************************************************** */

/* *****************************************************************************
**                      Private Macro Definitions                             **
***************************************************************************** */

/* *****************************************************************************
**                      Global Type Definitions                               **
***************************************************************************** */
XMC_GPIO_CONFIG_t i2c_sda_mpu =
{
  .mode =	XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN_ALT1,		/**< configures gpio mode of used pin for SDA*/
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM	/**< configures driver-strength of used pin*/
};


XMC_GPIO_CONFIG_t i2c_scl_mpu =
{
  .mode = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN_ALT1,			/**< configures gpio mode of used pin for SCL*/
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM	/**< configures driver-strength of used pin*/
};

XMC_I2C_CH_CONFIG_t i2c_cfg_mpu =
{
  .baudrate = 400000U,	/**< configures baud-rate of I2C-Interface*/
};
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
// Specify sensor full scale
uint8_t Gscale = GFS_500DPS; 	/**< Scale range of gyro */
uint8_t Ascale = AFS_4G;		/**< Scale range of accelerometer */
float aRes, gRes, mRes; /// scale resolutions per LSB for the sensors

int16_t accRaw[3];	/**< raw data from accelerometer */
int16_t gyroRaw[3];	/**< raw data from gyroscope */
int16_t magRaw[3];	/**< raw data from magnetometer  */

float acc[3] = {0.0f};	/**< corrected and usefull accelerometer data */
float gyro[3] = {0.0f};	/**< corrected and usefull gyro data */
float mag[3] = {0.0f};	/**< corrected and usefull magnetometer data */

int16_t accBias[3] = {0,0,0};
int16_t gyroBias[3] = {0,0,0};

float magCalibration[3] = {0.0f, 0.0f, 0.0f}, magbias[3] = {0.0f, 0.0f, 0.0f};  /// Factory mag calibration and mag bias
#if ATTITUDEALGORITHM == 1
KALMAN_t kalmanX, kalmanY, kalmanZ;		/**< Variables for Kalman filter */
float kalAngleX, kalAngleY, kalAngleZ;	/**< Variables for Kalman filter */
#elif ATTITUDEALGORITHM == 2
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    /// vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};       /// vector to hold integral error for Mahony method
#endif

uint32_t lastUpdate = 0; /// used to calculate integration interval
uint16_t counterSensor = 0; /// gets increased when the mpu sends new data
/* *****************************************************************************
**                      Private Variable Definitions                          **
***************************************************************************** */

/* *****************************************************************************
**                      Global Function Definitions                           **
***************************************************************************** */

/* *****************************************************************************
**                      Private Function Definitions                          **
***************************************************************************** */

/**
 *  \brief MPU I2C setup and selftests
 *
 *  
 *  \details Sets up I2C FIFO\n
 *  makes a selftest through reading the WHO_AM_I register\n
 *  
 */
void MPU9150_Setup(void)
{
	setupI2CInterfaceFIFO(MPU9x50_USIC,MPU9x50_USIC_CH_PIN_SDA,MPU9x50_USIC_CH_PIN_SCL,(XMC_GPIO_PORT_t *)PORT5_BASE,0,2,&i2c_sda_mpu,&i2c_scl_mpu,&i2c_cfg_mpu);

	// Read the WHO_AM_I register, this is a good test of communication
	uint8_t c = I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, WHO_AM_I_MPU9150);  // Read WHO_AM_I register for MPU-9150

	if (c == 0x68 || c == 0x71 || c == 0x73) // WHO_AM_I should always be 0x68 (MPU9150 or MPU9250)
	{
		if (MPU9150_SelfTest())
		{
			//MPU9150_Calibrate(); // Calibrate gyro and accelerometers, load biases in bias registers
			delay(1000);
			MPU9150_Init(); // Inititalize and configure accelerometer and gyroscope
			INTERRUPT_Enable(&MPU9X50_Ext_Int_ISR_Handle);
		}
	}
}

/**
 *  \brief Initialises the MPU and configures gyro and accelerometer
 *
 *
 *  \details wakes up device (enables all sensors and clears sleep mode bit)\n
 *  Configures gyro and accelerometer (sets bandwith to 44 and 42 Hz, sample rate at 1 kHz)\n
 *  sets gyroscope full scale rage\n
 *  sets accelerometer configuration\n
 *  configures fifo\n
 *  self tests magnetometer(WHO_AM_I register)\n
 *  cofigures magnetometer for FIFO\n
 *  configures interrupts and bypass enable
 */
void MPU9150_Init(void)
{
	// wake up device
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
	delay(100); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt

	// get stable time source
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
	delay(200);

	// Configure Gyro and Accelerometer
	// Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
	// DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
	// Minimum delay time is 4.9 ms which sets the fastest rate at ~200 Hz

	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, CONFIG, 0x01); //MPU9150
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, CONFIG, 0x06); //MPU9250

	// Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, SMPLRT_DIV, 0x01);  // Use a 200 Hz rate; the same rate set in CONFIG above

	// Set gyroscope full scale range
	// Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
	//uint8_t c =  I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG);
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG, c & ~0xE0); // Clear self-test bits [7:5] (MPU9150)
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG, c & ~0x18); // Clear AFS bits [4:3] (MPU9150)
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro (MPU9150)
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG, Gscale << 3); // Set full scale range for the gyro (MPU9250)

	// Set accelerometer configuration
	//c =  I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG);
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG, c & ~0xE0); // Clear self-test bits [7:5] (MPU9150)
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG, c & ~0x18); // Clear AFS bits [4:3] (MPU9150)
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer (MPU9150)
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG, Ascale << 3); // Set full scale range for the accelerometer (MPU9250)

	// Configure FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_ENABLE, 0x00); // Disable all interrupts
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_EN, 0x00);    // Disable FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x02);  // Reset I2C master and FIFO and DMP
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x00);  // Disable FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_PIN_CFG, 0x02);
	delay(100);

	// Read the WHO_AM_I register of the magnetometer, this is a good test of communication
	//c = I2Cdev_readByte(MPU9x50_USIC,AK8975A_ADDRESS, WHO_AM_I_AK8975A);  // Read WHO_AM_I register for AK8975A
	//if (c == 0x48)
	//	MPU9150_InitAK8975A(magCalibration);

	// Configure Magnetometer for FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_EN, 0x78); // Enable all sensors for FIFO (MPU9250)
	/*I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_EN, 0x79); // Enable all sensors for FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_MST_CTRL, 0x5D);
	// Set up auxilliary communication with AK8975A for FIFO read
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV0_ADDR, 0x8C); // Enable and read address (0x0C) of the AK8975A
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV0_REG, 0x02);  // Register within AK8975A from which to start data read
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV0_CTRL, 0xD7); // Read six bytes and swap bytes
	// Initialize AK8975A for write
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV1_ADDR, 0x0C);  // Write address of AK8975A
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV1_REG, 0x0A);   // Register from within the AK8975 to which to write
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV1_DO, 0x01);    // Register that holds output data written into Slave 1 when in write mode
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV1_CTRL, 0x81);  // Enable Slave 1
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_SLV4_CTRL, 0x04);*/

	// Configure Interrupts and Bypass Enable
	// Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips
	// can join the I2C bus and all can be controlled by the Arduino as master
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_PIN_CFG, 0x00); //(MPU9250)
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_ENABLE, 0x11);  // Enable data ready (bit 0) interrupt (MPU9150)
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt (MPU9250)

	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_MST_DELAY_CTRL, 0x83); // Enable delay of external sensor data until all data registers have been read
	//I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x64);
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x44); //(MPU9250)

#if ATTITUDEALGORITHM == 1
	KALMAN_Init(&kalmanX, 0.01, 0.03, 0.3);
	KALMAN_Init(&kalmanY, 0.01, 0.03, 0.3);
	KALMAN_Init(&kalmanZ, 0.01, 0.03, 0.3);
#endif
}

/**
 *  \brief initals 3 axis magnetometer
 *
 *  \param [in] destination pointer on destinaton where to send the adjustment values
 * 
 *  \details returns the adjustment values for the 3 axis
 */
void MPU9150_InitAK8975A(float * destination)
{
  uint8_t rawData[3];  // x/y/z gyro register data stored here
  I2Cdev_writeByte(MPU9x50_USIC,AK8975A_ADDRESS, AK8975A_CNTL, 0x00); // Power down
  delay(10);
  I2Cdev_writeByte(MPU9x50_USIC,AK8975A_ADDRESS, AK8975A_CNTL, 0x0F); // Enter Fuse ROM access mode
  delay(10);
  I2Cdev_readBytes(MPU9x50_USIC,AK8975A_ADDRESS, AK8975A_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
  destination[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f; // Return x-axis sensitivity adjustment values
  destination[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
  destination[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
}


/**
 *  \brief calibrates the MPU
 *
 *  
 *  \details Function which accumulates gyro and accelerometer data after device initialization. \n It calculates the average
 of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
 */
void MPU9150_Calibrate(void)
{
	uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
	uint16_t ii, packet_count, fifo_count;
	int32_t gyro_bias[3]  = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

	// reset device, reset all registers, clear gyro and accelerometer bias registers
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
	delay(100);

	// get stable time source
	// Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, PWR_MGMT_1, 0x01);
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, PWR_MGMT_2, 0x00);
	delay(200);

	// Configure device for bias calculation
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
	delay(15);

	// Configure MPU6050 gyro and accelerometer for bias calculation
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

	uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

	// Configure FIFO to capture accelerometer and gyro data for bias calculation
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 1024 bytes in MPU-6050)
	delay(80); // accumulate 80 samples in 80 milliseconds = 960 bytes

	// At end of sample accumulation, turn off FIFO sensor read
	I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
	I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
	fifo_count = ((uint16_t)data[0] << 8) | data[1];
	packet_count =(uint16_t) (fifo_count/12);// How many sets of full gyro and accelerometer data for averaging

	for (ii = 0; ii < packet_count; ii++)
	{
		int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
		I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_R_W, 12, &data[0]); // read data for averaging
		accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
		accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
		accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
		gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
		gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
		gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

		accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
		accel_bias[1] += (int32_t) accel_temp[1];
		accel_bias[2] += (int32_t) accel_temp[2];
		gyro_bias[0]  += (int32_t) gyro_temp[0];
		gyro_bias[1]  += (int32_t) gyro_temp[1];
		gyro_bias[2]  += (int32_t) gyro_temp[2];
	}

	accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

    if(accel_bias[2] > 0L)
    	accel_bias[2] -= (int32_t) accelsensitivity;  // Remove gravity from the z-axis accelerometer bias calculation
    else
    	accel_bias[2] += (int32_t) accelsensitivity;

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] =(uint8_t) (( (int32_t)(-gyro_bias[0]/4)  >> 8) & 0xFF); // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] =(uint8_t) ((int32_t)(-gyro_bias[0]/4) & 0xFF); // Biases are additive, so change sign on calculated average gyro biases
    data[2] =(uint8_t) (( (int32_t)(-gyro_bias[1]/4)  >> 8) & 0xFF);
    data[3] =(uint8_t) ((int32_t)(-gyro_bias[1]/4) & 0xFF);
    data[4] =(uint8_t) (( (int32_t)(-gyro_bias[2]/4)  >> 8) & 0xFF);
    data[5] =(uint8_t) ((int32_t)(-gyro_bias[2]/4) & 0xFF);

    // Push gyro biases to hardware registers
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, XG_OFFS_USRH, data[0]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, XG_OFFS_USRL, data[1]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, YG_OFFS_USRH, data[2]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, YG_OFFS_USRL, data[3]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ZG_OFFS_USRH, data[4]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ZG_OFFS_USRL, data[5]);

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, YA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, ZA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(ii = 0; ii < 3; ii++)
    {
    	if(accel_bias_reg[ii] & mask)
    		mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -=(int32_t) (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -=(int32_t) (accel_bias[1]/8);
    accel_bias_reg[2] -=(int32_t) (accel_bias[2]/8);

    data[0] =(uint8_t) ((accel_bias_reg[0] >> 8) & 0xFF);
    data[1] =(uint8_t) ((accel_bias_reg[0])      & 0xFF);
    data[1] =(uint8_t) (data[1] | mask_bit[0]); // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] =(uint8_t) ((accel_bias_reg[1] >> 8) & 0xFF);
    data[3] =(uint8_t) ((accel_bias_reg[1])      & 0xFF);
    data[3] =(uint8_t) (data[3] | mask_bit[1]); // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] =(uint8_t) ((accel_bias_reg[2] >> 8) & 0xFF);
    data[5] =(uint8_t) ((accel_bias_reg[2])      & 0xFF);
    data[5] =(uint8_t) (data[5] | mask_bit[2]); // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Push accelerometer biases to hardware registers
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, XA_OFFSET_H, data[0]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, XA_OFFSET_L_TC, data[1]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, YA_OFFSET_H, data[2]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, YA_OFFSET_L_TC, data[3]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ZA_OFFSET_H, data[4]);
    I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ZA_OFFSET_L_TC, data[5]);
}

// Accelerometer and gyroscope self test; check calibration wrt factory settings
/**
 *  \brief Self test for the MPU
 *
 *  \return returns true if passed and false if not
 *
 *  \details Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
 */
bool MPU9150_SelfTest(void) // 
{
   uint8_t rawData[4];
   uint8_t selfTest[6];
   float factoryTrim[6];
   float result[6];

   // Configure the accelerometer for self-test
   I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, ACCEL_CONFIG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
   I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, GYRO_CONFIG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
   delay(250);  // Delay a while to let the device execute the self-test
   rawData[0] = I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, SELF_TEST_X); // X-axis self-test results
   rawData[1] = I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, SELF_TEST_Y); // Y-axis self-test results
   rawData[2] = I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, SELF_TEST_Z); // Z-axis self-test results
   rawData[3] = I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, SELF_TEST_A); // Mixed-axis self-test results
   // Extract the acceleration test results first
   selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // XA_TEST result is a five-bit unsigned integer
   selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 4 ; // YA_TEST result is a five-bit unsigned integer
   selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 4 ; // ZA_TEST result is a five-bit unsigned integer
   // Extract the gyration test results first
   selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
   selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
   selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer
   // Process results to allow final comparison with factory set values
   factoryTrim[0] = (4096.0f*0.34f)*(powf( (0.92f/0.34f) , (((float)selfTest[0] - 1.0f)/30.0f))); // FT[Xa] factory trim calculation
   factoryTrim[1] = (4096.0f*0.34f)*(powf( (0.92f/0.34f) , (((float)selfTest[1] - 1.0f)/30.0f))); // FT[Ya] factory trim calculation
   factoryTrim[2] = (4096.0f*0.34f)*(powf( (0.92f/0.34f) , (((float)selfTest[2] - 1.0f)/30.0f))); // FT[Za] factory trim calculation
   factoryTrim[3] =  ( 25.0f*131.0f)*(powf( 1.046f , ((float)selfTest[3] - 1.0f) ));             // FT[Xg] factory trim calculation
   factoryTrim[4] =  (-25.0f*131.0f)*(powf( 1.046f , ((float)selfTest[4] - 1.0f) ));             // FT[Yg] factory trim calculation
   factoryTrim[5] =  ( 25.0f*131.0f)*(powf( 1.046f , ((float)selfTest[5] - 1.0f) ));             // FT[Zg] factory trim calculation

   // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
   // To get to percent, must multiply by 100 and subtract result from 100
   for (int i = 0; i < 6; i++)
	   result[i] = 100.0f + 100.0f*((float)selfTest[i] - factoryTrim[i])/factoryTrim[i]; // Report percent differences

   if(result[0] < 1.0f && result[1] < 1.0f && result[2] < 1.0f && result[3] < 1.0f && result[4] < 1.0f && result[5] < 1.0f)
	   return true;
   else
	   return false;
}

/**
 *  \brief sets gRes variable for calculating gyro value
 *
 *
 *  \details Function sets a global variable
 */
void getGres(void)
{
	switch (Gscale)
	{
		// Possible gyro scales (and their register bit settings) are:
		// 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    	case GFS_250DPS:
    		gRes = 250.0f/32768.0f;
    		break;
    	case GFS_500DPS:
    		gRes = 500.0f/32768.0f;
    		break;
    	case GFS_1000DPS:
    		gRes = 1000.0f/32768.0f;
    		break;
    	case GFS_2000DPS:
    		gRes = 2000.0f/32768.0f;
    		break;
	}
}

/**
 *  \brief sets Ares variable for calculating accelerometer value
 *  
 *  \details Function sets a global variable
 */
void getAres(void)
{
	switch (Ascale)
	{
		// Possible accelerometer scales (and their register bit settings) are:
		// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    	case AFS_2G:
    		aRes = 2.0f/32768.0f;
    		break;
    	case AFS_4G:
    		aRes = 4.0f/32768.0f;
    		break;
    	case AFS_8G:
    		aRes = 8.0f/32768.0f;
    		break;
    	case AFS_16G:
    		aRes = 16.0f/32768.0f;
    		break;
	}
}

/**
 *  \brief getter for SensorCount
 *
 *  \return current value of SensorCount
 *
 *  \details sensor count is a counter which gets elevated each time the mpu sends an interrupt
 */
uint16_t GetSensorCount(void)
{
	return counterSensor;
}

/**
 *  \brief calculates roll and pitch angles and angular velocity of yaw axis
 *
 *  \param [in] angles pointer to the output
 *
 *  \details calculates the angles with the selected algorithm
 */
void GetAngles(float* angles)
{

#if ATTITUDEALGORITHM == 1
	/*if (YAW_OFFSET == 0.0f)
		YAW_OFFSET = kalAngleZ;
	angles[0]=kalAngleZ-YAW_OFFSET;*/
	angles[0] = gyro[2]*RAD_TO_DEG;
	angles[1] = kalAngleY;
	angles[2] = kalAngleX;
#elif ATTITUDEALGORITHM == 2
	/*if (YAW_OFFSET == 0.0)
		YAW_OFFSET = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]) * RAD_TO_DEG;
	angles[0]=atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]) * RAD_TO_DEG - YAW_OFFSET;*/
	angles[0] = gyro[2]*RAD_TO_DEG;
	angles[1] = asinf(2.0f * (q[1] * q[3] - q[0] * q[2])) * RAD_TO_DEG;
	angles[2] = -atan2f(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]) * RAD_TO_DEG;
#endif
}

/**
 *  \brief getter for magnetic data
 *
 *  \param [in] pt returned data gets written here
 *
 *  \details writes the magnetometer values on the array with first position pt in a float[3] array
 */
void GetMagData(float* pt)
{
	pt[0] = mag[0];
	pt[1] = mag[1];
	pt[2] = mag[2];
}

/**
 *  \brief getter for gyro data
 *
 *  \param [in] pt returned data gets written here
 *
 *  \details returns the current gyro data in degree in a float[3] array
 */
void GetGyroData(float* pt)
{
	pt[0] = gyro[0]*RAD_TO_DEG;
	pt[1] = gyro[1]*RAD_TO_DEG;
	pt[2] = gyro[2]*RAD_TO_DEG;
}

/**
 *  \brief getter for acceleration data
 *
 *  \param [in] pt returned data gets written here 
 *  
 *  \details returns the current data in a float[3] array
 */
void GetAccData(float* pt)
{
	pt[0] = acc[0];
	pt[1] = acc[1];
	pt[2] = acc[2];
}

int dcounter = 0;

/**
 *  \brief Interrupt service routine of the externel MPU interrupt
 *
 *  
 *  \details gets called when the mpu has new data and sets the interrupt pin\n
 *  reads in the raw data and processes it
 */
void MPU9X50_Ext_Int_ISR(void)
{
	static uint8_t calib=1;
	static int16_t calib_cnt=0;
	//uint8_t data[19];
	uint8_t data[12];
	uint8_t mpuIntStatus;
	//uint8_t magDRDY;

	dcounter++;

		mpuIntStatus = I2Cdev_readByte(MPU9x50_USIC,MPU9150_ADDRESS, INT_STATUS);
		//FIFO-Count
		I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_COUNTH, 2, data);
		uint16_t fifoCount = (((uint16_t)data[0]) << 8) | data[1];

		if ((mpuIntStatus & 0x10) || fifoCount == 1024)
		{
			//Reset FIFO
			I2Cdev_writeByte(MPU9x50_USIC,MPU9150_ADDRESS, USER_CTRL, 0x64);
		}
		else if (mpuIntStatus & 0x01)
		{
			//Read from FIFO
			//if (I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_R_W, 19, data))
			if (I2Cdev_readBytes(MPU9x50_USIC,MPU9150_ADDRESS, FIFO_R_W, 12, data))
			{
				// Read the x/y/z adc values
				accRaw[0]=((int16_t)data[0] << 8) | data[1];
				accRaw[1]=((int16_t)data[2] << 8) | data[3];
				accRaw[2]=((int16_t)data[4] << 8) | data[5];

				// Read the x/y/z adc values
				gyroRaw[0]=((int16_t)data[6] << 8) | data[7];
				gyroRaw[1]=((int16_t)data[8] << 8) | data[9];
				gyroRaw[2]=((int16_t)data[10] << 8) | data[11];

				if (calib)
				{
					calib_cnt++;

					accBias[0]+=accRaw[0];
					accBias[1]+=accRaw[1];

					//Remove gravity
					switch (Ascale)
					{
						case AFS_2G:
							accBias[2]+=accRaw[2]-16384;
							break;
						case AFS_4G:
							accBias[2]+=accRaw[2]-8192;
							break;
						case AFS_8G:
							accBias[2]+=accRaw[2]-4096;
							break;
						case AFS_16G:
							accBias[2]+=accRaw[2]-2048;
							break;
					}

					gyroBias[0]+=gyroRaw[0];
					gyroBias[1]+=gyroRaw[1];
					gyroBias[2]+=gyroRaw[2];

					if (calib_cnt > 512)
					{
						switch (Ascale)
						{
							case AFS_2G:
								accBias[0] = accBias[0] >> 12;
								accBias[1] = accBias[1] >> 12;
								accBias[2] = accBias[2] >> 12;
								break;
							case AFS_4G:
								accBias[0] = accBias[0] >> 11;
								accBias[1] = accBias[1] >> 11;
								accBias[2] = accBias[2] >> 11;
								break;
							case AFS_8G:
								accBias[0] = accBias[0] >> 10;
								accBias[1] = accBias[1] >> 10;
								accBias[2] = accBias[2] >> 10;
								break;
							case AFS_16G:
								accBias[0] = accBias[0] >> 9;
								accBias[1] = accBias[1] >> 9;
								accBias[2] = accBias[2] >> 9;
								break;
						}

						int32_t acc_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
						I2Cdev_readBytes(MPU9x50_USIC, MPU9150_ADDRESS, XA_OFFSET_H, 2, data); // Read factory accelerometer trim values
						acc_bias_reg[0] = ((int16_t) ((int16_t)data[0] << 8) | data[1]) >> 1;
						I2Cdev_readBytes(MPU9x50_USIC, MPU9150_ADDRESS, YA_OFFSET_H, 2, data);
						acc_bias_reg[1] = ((int16_t) ((int16_t)data[0] << 8) | data[1]) >> 1;
						I2Cdev_readBytes(MPU9x50_USIC, MPU9150_ADDRESS, ZA_OFFSET_H, 2, data);
						acc_bias_reg[2] = ((int16_t) ((int16_t)data[0] << 8) | data[1]) >> 1;

						// Construct total accelerometer bias, including calculated average accelerometer bias from above
						acc_bias_reg[0] -= accBias[0]; // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
						acc_bias_reg[1] -= accBias[1];
						acc_bias_reg[2] -= accBias[2];

						acc_bias_reg[0] = acc_bias_reg[0] << 1;
						acc_bias_reg[1] = acc_bias_reg[1] << 1;
						acc_bias_reg[2] = acc_bias_reg[2] << 1;

						data[0] = (acc_bias_reg[0] >> 8) & 0xFF;
						data[1] = (acc_bias_reg[0])      & 0xFF;
						data[2] = (acc_bias_reg[1] >> 8) & 0xFF;
						data[3] = (acc_bias_reg[1])      & 0xFF;
						data[4] = (acc_bias_reg[2] >> 8) & 0xFF;
						data[5] = (acc_bias_reg[2])      & 0xFF;

						// Push accelerometer biases to hardware registers
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, XA_OFFSET_H, data[0]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, XA_OFFSET_L_TC, data[1]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, YA_OFFSET_H, data[2]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, YA_OFFSET_L_TC, data[3]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, ZA_OFFSET_H, data[4]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, ZA_OFFSET_L_TC, data[5]);

						switch (Gscale)
						{
							case GFS_250DPS:
								gyroBias[0]=-(gyroBias[0]>>11);
								gyroBias[1]=-(gyroBias[1]>>11);
								gyroBias[2]=-(gyroBias[2]>>11);
								break;
							case GFS_500DPS:
								gyroBias[0]=-(gyroBias[0]>>10);
								gyroBias[1]=-(gyroBias[1]>>10);
								gyroBias[2]=-(gyroBias[2]>>10);
								break;
							case GFS_1000DPS:
								gyroBias[0]=-(gyroBias[0]>>9);
								gyroBias[1]=-(gyroBias[1]>>9);
								gyroBias[2]=-(gyroBias[2]>>9);
								break;
							case GFS_2000DPS:
								gyroBias[0]=-(gyroBias[0]>>8);
								gyroBias[1]=-(gyroBias[1]>>8);
								gyroBias[2]=-(gyroBias[2]>>8);
								break;
						}

						// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
						data[0] = (gyroBias[0]  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
						data[1] = (gyroBias[0])       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
						data[2] = (gyroBias[1]  >> 8) & 0xFF;
						data[3] = (gyroBias[1])       & 0xFF;
						data[4] = (gyroBias[2]  >> 8) & 0xFF;
						data[5] = (gyroBias[2])       & 0xFF;

						// Push gyro biases to hardware registers
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, XG_OFFS_USRH, data[0]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, XG_OFFS_USRL, data[1]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, YG_OFFS_USRH, data[2]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, YG_OFFS_USRL, data[3]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, ZG_OFFS_USRH, data[4]);
						I2Cdev_writeByte(MPU9x50_USIC, MPU9150_ADDRESS, ZG_OFFS_USRL, data[5]);

						calib = 0;
						calib_cnt=0;
					}
				}
				else
				{
					getAres();
					getGres();

					// Now we'll calculate the accleration value into actual g's
					acc[0] = (float)accRaw[0]*aRes;  // get actual g value, this depends on scale being set
					acc[1] = (float)accRaw[1]*aRes;
					acc[2] = (float)accRaw[2]*aRes;
					//transformation(acc);

					// Calculate the gyro value into actual degrees per rad
					gyro[0] = (float)gyroRaw[0]*gRes * DEG_TO_RAD;  // get actual gyro value, this depends on scale being set
					gyro[1] = (float)gyroRaw[1]*gRes * DEG_TO_RAD;
					gyro[2] = (float)gyroRaw[2]*gRes * DEG_TO_RAD;
					//transformation(gyro);

					uint32_t Now = getDelaymillis();
					float dt = ((Now - lastUpdate)/1000.0f); // set integration time by time elapsed since last filter update
					lastUpdate = Now;

#if ATTITUDEALGORITHM == 1

					float roll = atan2(acc[1], acc[2]) * RAD_TO_DEG;
					float pitch = atan(-acc[0] / sqrt(acc[1] * acc[1] + acc[2] * acc[2])) * RAD_TO_DEG;

					// This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
					if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90))
					{
						KALMAN_setAngle(&kalmanX, roll);
						kalAngleX = roll;
					}
					else
						kalAngleX = KALMAN_getAngle(&kalmanX, roll, gyro[0], dt); // Calculate the angle using a Kalman filter

					if (abs(kalAngleX) > 90)
						gyro[1] = -gyro[1]; // Invert rate, so it fits the restricted accelerometer reading

					kalAngleY = KALMAN_getAngle(&kalmanY, pitch, gyro[1], dt);

					//Tilt compensation
					if (magDRDY & 0x01)
					{
						float magX, magY, magZ;
						float magNorm = sqrt(mag[0]*mag[0]+mag[1]*mag[1]+mag[2]*mag[2]);
						magX=mag[1]/magNorm;
						magY=mag[0]/magNorm;
						magZ=-mag[2]/magNorm;

						float MY=magZ*sin(kalAngleX*DEG_TO_RAD)-magY*cos(kalAngleX*DEG_TO_RAD);
						float MX=magX*cos(kalAngleY*DEG_TO_RAD)+magY*sin(kalAngleY*DEG_TO_RAD)*sin(kalAngleX*DEG_TO_RAD)+magZ*sin(kalAngleY*DEG_TO_RAD)*cos(kalAngleX*DEG_TO_RAD);

						float yaw = atan2(MY,MX)*RAD_TO_DEG;

						// This fixes the transition problem when the yaw angle jumps between -180 and 180 degrees
						if ((yaw < -90 && kalAngleZ > 90) || (yaw > 90 && kalAngleZ < -90))
						{
							KALMAN_setAngle(&kalmanZ, yaw);
							kalAngleZ = yaw;
						}
						else
							kalAngleZ = KALMAN_getAngle(&kalmanZ, yaw, gyro[2], dt); // Calculate the angle using a Kalman filter
					}

#elif ATTITUDEALGORITHM == 2

					// Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of the magnetometer;
					// the magnetometer z-axis (+ down) is opposite to z-axis (+ up) of accelerometer and gyro!
					// We have to make some allowance for this orientation mismatch in feeding the output to the quaternion filter.
					// For the MPU-9150, we have chosen a magnetic rotation that keeps the sensor forward along the x-axis just like
					// in the LSM9DS0 sensor. This rotation can be modified to allow any convenient orientation convention.
					// This is ok by aircraft orientation standards!
					// Pass gyro rate as rad/s
					MadgwickQuaternionUpdateWithouMag(q, dt, acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2]);
					//MadgwickQuaternionUpdate(q, dt, acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2], mag[0], mag[1], mag[2]);
					//MahonyQuaternionUpdate(q, eInt, dt, acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2], mag[0], mag[1], mag[2]);

#endif
					counterSensor++;
				}
			}
		}
}

/**
 *  \brief Transformation for MPU values
 *
 *  \param [in] values Parameter_Description
 *
 *  \details Transforms the positioning from Prototype PCP to the Larix PCB
 */
void transformation(float* values)
{
	//transformation matrix
	float transformation_matrix[3][3] =
	{
			{-1.0f, 0.0f, 0.0f},
			{0.0f, -1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
//		{ -0.70710678118654752440084436210485f, -0.70710678118654752440084436210485f,0.0f },
//		{ 0.70710678118654752440084436210485f,-0.70710678118654752440084436210485f, 0.0f },
//		{ 0.0f, 0.0f, 1.0f }
	};
	//calculation
	float result[3] = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result[i] += transformation_matrix[i][j] * values[j];

	for (int i = 0; i < 3; ++i)
		values[i] = result[i];
}
/**
 *  \brief Transformation for MPU values
 *
 *  \param [in] values Parameter_Description
 *
 *  \details Transforms the positioning from Prototype PCP to the Larix PCB
 */
void transformation_mag(float* values)
{
	float calibration_matrix[3][3] =
	{
			{-1.0f, 0.0f, 0.0f},
			{0.0f, -1.0f, 0.0f},
			{0.0f, 0.0f, -1.0f}
//		{ -0.707106781186548f, -0.707106781186547f, 0.0f },
//		{ -0.707106781186547f, 0.707106781186548f, 0.0f },
//		{ 0.0f, 0.0f, -1.0f }
	};

	float bias[3] =
	{ -0.557f, 217.116f, -446.63f };
	//calculation
	for (int i = 0; i < 3; ++i)
		values[i] = values[i] - bias[i];
	float result[3] = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result[i] += calibration_matrix[i][j] * values[j];
	for (int i = 0; i < 3; ++i)
		values[i] = result[i];
}

