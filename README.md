# button_lib for STM32F303/F103 But can be used for any STM32 devices with HAL lib.
- Library provides function to detect single press, double press and long press.
- To see example usage, go to core/src/main.c
- The buttons must be configured as EXTI, with rising and falling edge trigger.
- User can set DEBOUNCE_DURATION, LONG_PRESS_DURATION and DOUBLE_PRESS_WINDOW in mS.
- The example provides a demo of 3 buttons connected on PA0, PA1 and PA2 line.
- For more, read the comments in the code.
