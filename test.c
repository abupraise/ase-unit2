void PIT_Channel_1(void){
    static int blink_count = 0;
    static int blink_state = 1;

    PIT.CHANNEL[1].TFLG.R = 1U;

    if (engine_on) {
        can_send();

        if (lights_state == 0) {
            blink_count = 0;
            blink_state = 1;
            LED_U1 = 0;
            LED_U3 = 0;
        } else {
            blink_count++;

            if (blink_count >= 5) {
                blink_count = 0;
                blink_state = !blink_state;
            }

            LED_U1 = ((lights_state & 0x01) != 0)
                      && blink_state;
            LED_U3 = ((lights_state & 0x02) != 0)
                      && blink_state;
        }
    } else {
        blink_count = 0;
        blink_state = 1;
        LED_U1 = 0;
        LED_U3 = 0;
    }
}