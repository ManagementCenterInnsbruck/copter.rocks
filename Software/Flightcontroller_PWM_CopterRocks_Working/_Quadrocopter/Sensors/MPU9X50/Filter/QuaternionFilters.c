/* *****************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** MAAN         				                                              **
**                                                                            **
**                                                                            **
***************************************************************************** */

/* *****************************************************************************
**                      Revision Control History                              **
***************************************************************************** */
/* 
 * V0.0: 06.05.2015,MAAN:  Initial Version
 */
 
// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
/* *****************************************************************************
**                      Includes                                              **
***************************************************************************** */
#include "QuaternionFilters.h"
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

/* *****************************************************************************
**                      Global Function Definitions                           **
***************************************************************************** */
/**
 *  \brief Brief
 *  
 *  \param [in] q Parameter_Description
 *  \param [in] deltat Parameter_Description
 *  \param [in] ax Parameter_Description
 *  \param [in] ay Parameter_Description
 *  \param [in] az Parameter_Description
 *  \param [in] gx Parameter_Description
 *  \param [in] gy Parameter_Description
 *  \param [in] gz Parameter_Description
 *  \param [in] mx Parameter_Description
 *  \param [in] my Parameter_Description
 *  \param [in] mz Parameter_Description
 *  \return Return_Description
 *  
 *  \details Details
 */
void MadgwickQuaternionUpdate(float* q, float deltat, float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   /**<short name local variable for readability */ 
	float norm;
	float hx, hy, _2bx, _2bz;
	float s1, s2, s3, s4;
	float qDot1, qDot2, qDot3, qDot4;

	float beta = sqrtf(3.0f / 4.0f) * GYROMEASERROR;

	// Auxiliary variables to avoid repeated arithmetic
	float _2q1mx;
	float _2q1my;
	float _2q1mz;
	float _2q2mx;
	float _4bx;
	float _4bz;
	float _2q1 = 2.0f * q1;
	float _2q2 = 2.0f * q2;
	float _2q3 = 2.0f * q3;
	float _2q4 = 2.0f * q4;
	float _2q1q3 = 2.0f * q1 * q3;
	float _2q3q4 = 2.0f * q3 * q4;
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrtf(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 0.1f/norm;
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrtf(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 0.001f/norm;
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	_2q1mx = 2.0f * q1 * mx;
	_2q1my = 2.0f * q1 * my;
	_2q1mz = 2.0f * q1 * mz;
	_2q2mx = 2.0f * q2 * mx;
	hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
	hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
	_2bx = sqrtf(hx * hx + hy * hy);
	_2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
	_4bx = 2.0f * _2bx;
	_4bz = 2.0f * _2bz;

	// Gradient decent algorithm corrective step
	s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	norm = sqrtf(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
	norm = 1.0f/norm;
	s1 *= norm;
	s2 *= norm;
	s3 *= norm;
	s4 *= norm;

	// Compute rate of change of quaternion
	qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
	qDot2 = 0.5f * ( q1 * gx + q3 * gz - q4 * gy) - beta * s2;
	qDot3 = 0.5f * ( q1 * gy - q2 * gz + q4 * gx) - beta * s3;
	qDot4 = 0.5f * ( q1 * gz + q2 * gy - q3 * gx) - beta * s4;

	// Integrate to yield quaternion
	q1 += qDot1 * deltat;
	q2 += qDot2 * deltat;
	q3 += qDot3 * deltat;
	q4 += qDot4 * deltat;
	norm = sqrtf(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
	norm = 1.0f/norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;
}

/* *****************************************************************************
**                      Private Function Definitions                          **
***************************************************************************** */

 // Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
 // measured ones.
//void MahonyQuaternionUpdate(float* q, float* eInt, float deltat, float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
//{
//	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
//	float norm;
//	float hx, hy, bx, bz;
//	float vx, vy, vz, wx, wy, wz;
//	float ex, ey, ez;
//	float pa, pb, pc;
//
//	// Auxiliary variables to avoid repeated arithmetic
//	float q1q1 = q1 * q1;
//	float q1q2 = q1 * q2;
//	float q1q3 = q1 * q3;
//	float q1q4 = q1 * q4;
//	float q2q2 = q2 * q2;
//	float q2q3 = q2 * q3;
//	float q2q4 = q2 * q4;
//	float q3q3 = q3 * q3;
//	float q3q4 = q3 * q4;
//	float q4q4 = q4 * q4;
//
//	// Normalise accelerometer measurement
//	norm = sqrt(ax * ax + ay * ay + az * az);
//	if (norm == 0.0f) return; // handle NaN
//	norm = 1.0f / norm;        // use reciprocal for division
//	ax *= norm;
//	ay *= norm;
//	az *= norm;
//
//	// Normalise magnetometer measurement
//	norm = sqrt(mx * mx + my * my + mz * mz);
//	if (norm == 0.0f) return; // handle NaN
//	norm = 1.0f / norm;        // use reciprocal for division
//	mx *= norm;
//	my *= norm;
//	mz *= norm;
//
//	// Reference direction of Earth's magnetic field
//	hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
//	hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
//	bx = sqrt((hx * hx) + (hy * hy));
//	bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);
//
//	// Estimated direction of gravity and magnetic field
//	vx = 2.0f * (q2q4 - q1q3);
//	vy = 2.0f * (q1q2 + q3q4);
//	vz = q1q1 - q2q2 - q3q3 + q4q4;
//	wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
//	wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
//	wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);
//
//	// Error is cross product between estimated direction and measured direction of gravity
//	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
//	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
//	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
//	if (Ki > 0.0f)
//	{
//		eInt[0] += ex;      // accumulate integral error
//		eInt[1] += ey;
//		eInt[2] += ez;
//	}
//	else
//	{
//		eInt[0] = 0.0f;     // prevent integral wind up
//		eInt[1] = 0.0f;
//		eInt[2] = 0.0f;
//	}
//
//	// Apply feedback terms
//	gx = gx + Kp * ex + Ki * eInt[0];
//	gy = gy + Kp * ey + Ki * eInt[1];
//	gz = gz + Kp * ez + Ki * eInt[2];
//
//	// Integrate rate of change of quaternion
//	pa = q2;
//	pb = q3;
//	pc = q4;
//	q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
//	q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
//	q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
//	q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);
//
//	// Normalise quaternion
//	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
//	norm = 1.0f / norm;
//	q[0] = q1 * norm;
//	q[1] = q2 * norm;
//	q[2] = q3 * norm;
//	q[3] = q4 * norm;
//}

void MadgwickQuaternionUpdateWithouMag(float* q, float deltat, float a_x, float a_y, float a_z, float w_x, float w_y, float w_z)
{
	// Local system variables
	float norm; // vector norm
	float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; // quaternion derrivative from gyroscopes elements
	float f_1, f_2, f_3; // objective function elements
	float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; // objective function Jacobian elements
	float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4; // estimated direction of the gyroscope error
	// Axulirary variables to avoid reapeated calcualtions
	float halfSEq_1 = 0.5f * q[0];
	float halfSEq_2 = 0.5f * q[1];
	float halfSEq_3 = 0.5f * q[2];
	float halfSEq_4 = 0.5f * q[3];
	float twoSEq_1 = 2.0f * q[0];
	float twoSEq_2 = 2.0f * q[1];
	float twoSEq_3 = 2.0f * q[2];

	float beta = sqrtf(3.0f / 4.0f) * GYROMEASERROR;

	// Normalise the accelerometer measurement
	norm = sqrtf(a_x * a_x + a_y * a_y + a_z * a_z);
	a_x /= norm;
	a_y /= norm;
	a_z /= norm;
	// Compute the objective function and Jacobian
	f_1 = twoSEq_2 * q[3] - twoSEq_1 * q[2] - a_x;
	f_2 = twoSEq_1 * q[1] + twoSEq_3 * q[3] - a_y;
	f_3 = 1.0f - twoSEq_2 * q[1] - twoSEq_3 * q[2] - a_z;
	J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
	J_12or23 = 2.0f * q[3];
	J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
	J_14or21 = twoSEq_2;
	J_32 = 2.0f * J_14or21; // negated in matrix multiplication
	J_33 = 2.0f * J_11or24; // negated in matrix multiplication
	// Compute the gradient (matrix multiplication)
	SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
	SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
	SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
	SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;
	// Normalise the gradient
	norm = sqrtf(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
	SEqHatDot_1 /= norm;
	SEqHatDot_2 /= norm;
	SEqHatDot_3 /= norm;
	SEqHatDot_4 /= norm;
	// Compute the quaternion derrivative measured by gyroscopes
	SEqDot_omega_1 = -halfSEq_2 * w_x - halfSEq_3 * w_y - halfSEq_4 * w_z;
	SEqDot_omega_2 = halfSEq_1 * w_x + halfSEq_3 * w_z - halfSEq_4 * w_y;
	SEqDot_omega_3 = halfSEq_1 * w_y - halfSEq_2 * w_z + halfSEq_4 * w_x;
	SEqDot_omega_4 = halfSEq_1 * w_z + halfSEq_2 * w_y - halfSEq_3 * w_x;
	// Compute then integrate the estimated quaternion derrivative
	q[0] += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
	q[1] += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
	q[2] += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
	q[3] += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;
	// Normalise quaternion
	norm = sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	q[0] /= norm;
	q[1] /= norm;
	q[2] /= norm;
	q[3] /= norm;
}
