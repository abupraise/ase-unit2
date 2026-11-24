/*
 *	Project Owner	: TU Chemnitz
 *	Project Name	: ASE Tutorial Unit-2
 *	File Name		: main.c
 *	Author			: ASE Admin
 *  Created on		: 25 Feb 2024
 */

/* Includes ******************************************************************/
#include "init.h"


/* Global variables **********************************************************/
int speed = 0;
int rpm = 0;

/* Additional state needed by the original template callbacks. */
volatile int engine_running = 0;
volatile int send_pending = 0;
int rpm_direction = -1;
int lights = 0;
int previous_lights = -1;
int blink_state = 1;
int blink_counter = 0;


void can_send_engine_on(void){

	/* TO-DO: your task implementations **************************************/

    // set payload bytes, if necessary
    // CAN_0.BUF[...].DATA.B[...] = ...;

	// write the correct CODE value to this register to instruct the CAN controller to transmit the CAN message
	// CAN_0.BUF[...].CS.B.CODE = ...;

	/* Send only when the chosen buffer is inactive (CODE = 0x8). */
	if (CAN_0.BUF[8].MSG_CS.B.CODE == 0x8) {
		CAN_0.BUF[8].MSG_CS.B.CODE = 0xC;
		LED_Tx = !LED_Tx;
	}
}

void can_send_speed(int value){
	/* TO-DO: your task implementations **************************************/
	if (value < 0) value = 0;
	if (value > 240) value = 240;

	if (CAN_0.BUF[9].MSG_CS.B.CODE == 0x8) {
		CAN_0.BUF[9].DATA.B[0] = (uint8_t)value;
		CAN_0.BUF[9].MSG_CS.B.CODE = 0xC;
	}

}

void can_send_rpm(int value){
	/* TO-DO: your task implementations **************************************/
	if (value < 0) value = 0;
	if (value > 8000) value = 8000;

	if (CAN_0.BUF[10].MSG_CS.B.CODE == 0x8) {
		/* RPM is little-endian: lower byte, followed by higher byte. */
		CAN_0.BUF[10].DATA.B[0] = (uint8_t)(value & 0xFF);
		CAN_0.BUF[10].DATA.B[1] = (uint8_t)((value >> 8) & 0xFF);
		CAN_0.BUF[10].MSG_CS.B.CODE = 0xC;
	}

}

void can_send_lights(void){
	/* TO-DO: your task implementations **************************************/
	if (CAN_0.BUF[11].MSG_CS.B.CODE == 0x8) {
		CAN_0.BUF[11].DATA.B[0] = (uint8_t)lights;
		CAN_0.BUF[11].MSG_CS.B.CODE = 0xC;
	}

}

void can_send(void){

	/* TO-DO: your task implementations **************************************/
	/* PIT channel 2 sets this flag once every 100 ms. */
	if (!send_pending) return;
	send_pending = 0;
	if (!engine_running) return;

	/* Convert the 12-bit potentiometer reading to 0..240 km/h. */
	speed = (int)((POT * 240U + 2047U) / 4095U);

	/* SW2 controls left (bit 0); SW1 controls right (bit 1). */
	lights = 0;
	if (SW2) lights |= 0x01;
	if (SW1) lights |= 0x02;

	/* Onboard indicator LEDs blink with a 500 ms on/off interval. */
	if (lights != previous_lights) {
		previous_lights = lights;
		blink_state = 1;
		blink_counter = 0;
	} else {
		blink_counter++;
		if (blink_counter >= 5) {
			blink_counter = 0;
			blink_state = !blink_state;
		}
	}
	LED_U1 = blink_state && (lights & 0x01);
	LED_U3 = blink_state && (lights & 0x02);

	/* Every frame is due once per 100 ms while the engine is running. */
	can_send_engine_on();
	can_send_speed(speed);
	can_send_rpm(rpm);
	can_send_lights();
	/*************************************************************************/
}


/*
 * @brief	Main program
 *
 * @param	void
 * @retval	int
 */
int main(void) {
	/* peripherals initialization */
	peripheralsInit();



	/* TO-DO: your task implementations **************************************/

	/* Configure and start timer channels */
	/* PIT timer channel 1, period = 1000 ms */
	PIT_ConfigureTimer(1, 1000);
	/* PIT timer channel 2, period = 100 ms */
	PIT_ConfigureTimer(2, 100);
	PIT_StartTimer(2);

	/* The ECU stays in idle mode until CAN ID 0x11 is received. */
	LED_P = 0;
	LED_Rx = 0;
	LED_Tx = 0;
	LED_U1 = 0;
	LED_U3 = 0;
	/*************************************************************************/


	/* main program */
	while(1){
		/* System function */
		systemFunction();

		/* TO-DO: your task implementations **********************************/

		can_send();

		/*********************************************************************/

		/* 10 ms OS delay */
		osalThreadDelayMilliseconds(10UL);
	}
}


/*
 * @brief	CAN receive interrupt
 *
 * @param   CANDriver   canp	The CAN controller which received a message
 * @param	CANRxFrame	crfp	The message which was received
 * @retval	void
 */
void can_receive(CANRxFrame crfp) {
    if(crfp.IDE == CAN_IDE_STD) // a standard message frame has been received
    {
        /* TO-DO: your task implementations ******************************/

        //if (crfp.SID == ...) // check the Message ID
        //{

        //}

		/* Only Start (0x11) and Stop (0x10) are accepted. */
		if (crfp.SID == 0x10 || crfp.SID == 0x11) {
			LED_Rx = !LED_Rx;
		}

		if (crfp.SID == 0x11) {
			LED_U1 = !LED_U1;
			if (!engine_running) {
				engine_running = 1;
				LED_P = 1;
				rpm = 8000;
				rpm_direction = -1;
				previous_lights = -1;
				send_pending = 0;
				/* Restart the one-second timer at engine start. */
				PIT_StopTimer(1);
				PIT_ConfigureTimer(1, 1000);
				PIT_StartTimer(1);
			}
		}

		if (crfp.SID == 0x10) {
			engine_running = 0;
			LED_P = 0;
			LED_U1 = 0;
			LED_U3 = 0;
			rpm = 0;
			speed = 0;
			lights = 0;
			previous_lights = -1;
			send_pending = 0;
			PIT_StopTimer(1);
		}
    }
}


/*
 * @brief	PIT timer channel 1 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_1(void){
	/* TO-DO: your task implementations **************************************/
	if (!engine_running) return;

	/* Each second, move RPM by 250 and reverse at 0 and 8000. */
	rpm += 250 * rpm_direction;
	if (rpm <= 0) {
		rpm = 0;
		rpm_direction = 1;
	}
	if (rpm >= 8000) {
		rpm = 8000;
		rpm_direction = -1;
	}

}


/*
 * @brief	PIT timer channel 2 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_2(void){
	/* TO-DO: your task implementations **************************************/
	send_pending = 1;

}


/*
 * @brief	PIT timer channel 3 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_3(void){
	/* TO-DO: your task implementations **************************************/


}
