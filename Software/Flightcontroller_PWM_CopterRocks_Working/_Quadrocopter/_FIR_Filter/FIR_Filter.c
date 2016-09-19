/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** HPL          Heinz-Peter	Liechtenecker                                     **
** DW           Dominik 	Wieland                                           **
**                                                                            **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V0.0: 30-01-2016, HPL: Initial Version
 * V0.1: 21-07-2016, DW:  Port of SW from DAVE3 to DAVE4
 */


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "FIR_Filter.h"


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
 *  \brief adds new value in buffer and applies FIR-Filter
 *  
 *  \param [in] temp current FIR struct
 *  \param [in] NewValue value to add
 *  \return renewed FIR struct
 *  
 */
struct structFIR FIR_FILTER(struct structFIR temp, float NewValue)
{
	//Neuen Wert in den Ringbuffer schreiben
	temp.CIRC_BUFF[temp.POS] = NewValue;

	//Neuen Zeiger auf das Array berrechnen
	temp.POS = ((temp.POS + 1) % FIR_SIZE);

	//Rückgabewert erstmal resetten
	temp.VALUE = 0;

	//Eigentliches Werteberechnung durch Faltungsintegral
	for (int i = 0; i < FIR_SIZE; i++)
	{
		temp.VALUE += (temp.FIR_COEFF[i]
				* temp.CIRC_BUFF[(temp.POS + i) % FIR_SIZE]);
	}
	//Rueckgabewert des neuen Buffers
	return temp;
}
/**
 *  \brief Initializes FIR Filter
 *  
 *  \param [in] temp structFir to initialize
 *  \param [in] type type of filter coefficients used  
 *  \return initialized structFir
 *  
 */
struct structFIR Initialize_FIR_Filter(struct structFIR temp, int type)
{
	//Alle Werte im Ringbuffer auf 0 setzen
	for (int i = 0; i < FIR_SIZE; i++)
		temp.CIRC_BUFF[i] = 0.0f;

	//Pointer im Circ-Buffer auf 0 setzen
	temp.POS = 0;

	//Filter-Koeffizienten erstellen und im Array befuellen
	switch (type)
	{
	case MOVING_AVERAGE:
	default:
		for (int i = 0; i < FIR_SIZE; i++)
			temp.FIR_COEFF[i] = 1.0f / FIR_SIZE;
		break;
	}
	return temp;
}


/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/
