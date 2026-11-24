/*
 *	Project Owner	: TU Chemnitz
 *	Project Name	: ASE Tutorial Unit-2
 *	File Name		: can.c
 *	Author			: ASE Admin
 *  Created on		: 25 Feb 2024
 */

/* Includes ******************************************************************/
#include "can.h"
#include "xpc56el.h"

/*
 * @brief	CAN0 initialization function
 *
 * @param	void
 * @retval	void
 */
void CAN_Init(void){
	can_lld_start(&CAND1, &can_config_cfg0);


	/* TO-DO: pre-configure message buffer(s) for sending here ***********************/

	//CAN_0.BUF[...].MSG_CS.B.IDE = ...;		/* Message/frame format: 1 = Extended, 0 = Standard/Base */
	//CAN_0.BUF[...].MSG_ID.B.STD_ID = ...;		/* Message ID for standard frame */
	//CAN_0.BUF[...].MSG_ID.B.EXT_ID = ...;		/* Message ID for extended frame*/
	//CAN_0.BUF[...].MSG_CS.B.RTR = ...;		/* Remote Transmission Request; 0 = Data Frame */
	//CAN_0.BUF[...].CS.B.LENGTH = ...;			/* Length of Payload, 0 - 8*/

	/* Buffer 8: Engine On, extended CAN ID 0xDA0, no payload. */
	CAN_0.BUF[8].MSG_CS.B.CODE = 0x8;
	CAN_0.BUF[8].MSG_CS.B.IDE = 1;
	CAN_0.BUF[8].MSG_CS.B.SRR = 1;
	CAN_0.BUF[8].MSG_CS.B.RTR = 0;
	CAN_0.BUF[8].MSG_ID.B.STD_ID = 0;
	CAN_0.BUF[8].MSG_ID.B.EXT_ID = 0xDA0;
	CAN_0.BUF[8].MSG_CS.B.LENGTH = 0;

	/* Buffer 9: vehicle speed, standard CAN ID 0x5A0, one byte. */
	CAN_0.BUF[9].MSG_CS.B.CODE = 0x8;
	CAN_0.BUF[9].MSG_CS.B.IDE = 0;
	CAN_0.BUF[9].MSG_CS.B.SRR = 0;
	CAN_0.BUF[9].MSG_CS.B.RTR = 0;
	CAN_0.BUF[9].MSG_ID.B.STD_ID = 0x5A0;
	CAN_0.BUF[9].MSG_ID.B.EXT_ID = 0;
	CAN_0.BUF[9].MSG_CS.B.LENGTH = 1;

	/* Buffer 10: engine RPM, standard CAN ID 0x280, two bytes. */
	CAN_0.BUF[10].MSG_CS.B.CODE = 0x8;
	CAN_0.BUF[10].MSG_CS.B.IDE = 0;
	CAN_0.BUF[10].MSG_CS.B.SRR = 0;
	CAN_0.BUF[10].MSG_CS.B.RTR = 0;
	CAN_0.BUF[10].MSG_ID.B.STD_ID = 0x280;
	CAN_0.BUF[10].MSG_ID.B.EXT_ID = 0;
	CAN_0.BUF[10].MSG_CS.B.LENGTH = 2;

	/* Buffer 11: indicators, standard CAN ID 0x470, one byte. */
	CAN_0.BUF[11].MSG_CS.B.CODE = 0x8;
	CAN_0.BUF[11].MSG_CS.B.IDE = 0;
	CAN_0.BUF[11].MSG_CS.B.SRR = 0;
	CAN_0.BUF[11].MSG_CS.B.RTR = 0;
	CAN_0.BUF[11].MSG_ID.B.STD_ID = 0x470;
	CAN_0.BUF[11].MSG_ID.B.EXT_ID = 0;
	CAN_0.BUF[11].MSG_CS.B.LENGTH = 1;

	/*************************************************************************/
}


/*
 * @brief	CAN message transmit function
 *
 * @param	CANTxFrame	txMsg
 * @param	canmbx_t	mbx
 * @retval	void
 */
void CAN_TxMessage(CANTxFrame txMsg, canmbx_t mbx){
	// nop
}
