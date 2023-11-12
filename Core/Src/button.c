/**
 * @file button.c
 *
 * @brief Button handling library for STM32.
 *
 * @details This library provides functions to initialize and handle button events
 * on STM32 microcontrollers.
 *
 * @author deligent4
 */


#include "button.h"
#include "main.h"


uint8_t is_button_pressed;
uint16_t debug_count, debounce_counter;
static uint8_t is_state_changed;
uint8_t button_has_changed;
uint32_t double_window_counter;

/**
 * @brief Initializes the button structure with GPIO port, pin, debounce time, initial state, and state change flag.
 *
 * This function sets up the initial configuration for the button structure.
 *
 * @param button Pointer to the Button structure(Button_label).
 * @param GPIO_Port GPIO port of the button.
 * @param pin GPIO pin of the button.
 */
void Button_Init(Button* button, GPIO_TypeDef* GPIO_Port, uint16_t pin) {
    button->GPIO_Port = GPIO_Port;
    button->_pin = pin;
    button->_state = GPIO_PIN_SET;
    button->_has_changed = 0;
    button->_press_start_time = 0;
    button->_long_press_event = 0;
    button->_double_press_event = 0;
}


/**
 * @brief Handles button interrupts, including debouncing and detecting press events.
 *
 * This function is designed to be called in response to button interrupts.
 * Handles Single Press, Double Press and Long Press events.
 *
 * @param button Pointer to the Button structure.
 */
void Button_IRQ_Handler(Button* button) {
    static uint32_t ignore_until = 0;
    static uint32_t press_start_time = 0;
    static uint32_t last_press_time = 0;

    if (ignore_until > HAL_GetTick()) {
        // Ignore any changes during this period
    } else {
        uint8_t current_state = HAL_GPIO_ReadPin(button->GPIO_Port, button->_pin);

        if (current_state != button->_state) {
            // Update the debounce period and handle the state change
            button->_state = current_state;
            button->_has_changed = 1;  // Set the state change flag
            button_has_changed = button->_has_changed;  // Added for debugging
            debug_count++;  // Added for debugging
            ignore_until = HAL_GetTick() + DEBOUNCE_DURATION;

            if (current_state == GPIO_PIN_SET) {
                // Button released
                uint32_t press_duration = HAL_GetTick() - press_start_time;
                if (press_duration >= LONG_PRESS_DURATION) {
                    // Handle long press event
                	button->_long_press_event = 1;
                    // Reset the press start time
                    press_start_time = 0;
                }
                if(press_duration < DOUBLE_PRESS_WINDOW){
                	//Check for double press
                	uint32_t time_since_last_press = press_start_time - last_press_time;
                	if(time_since_last_press < DOUBLE_PRESS_WINDOW){
                		// Handle double press event
                		button->_double_press_event = 1;
                	}
                	// Record the last press time
					last_press_time = press_start_time;
                }


            } else {
                // Button pressed
                press_start_time = HAL_GetTick();
            }
        }
    }
}


/**
 * @brief Reads the current state of the button.
 *
 * This function reads the state of the button's GPIO pin.
 *
 * @param button Pointer to the Button structure.
 * @return GPIO_PIN_RESET if the button is pressed, GPIO_PIN_SET if the button is released.
 */
static uint8_t Button_Read(Button* button) {
    return HAL_GPIO_ReadPin(button->GPIO_Port, button->_pin);
}


/**
 * @brief Checks if the button state has changed since the last check.
 *
 *	This Function is also used to get the Single Button Press events.
 * @param button Pointer to the Button structure.
 * @return 1 if the button state has changed(or button is pressed), 0 otherwise.
 */
static uint8_t Button_Has_Changed(Button* button) {
    if (button->_has_changed) {
        // Clear the state change flag
    	button->_has_changed = 0;
        return 1;
    }
    return 0;
}


/**
 * @brief Calls Button_IRQ_Handler, checks if there is a state change, and if the button is currently pressed.
 *
 * @param button Pointer to the Button structure.
 * @return 1 if the button state has changed and is currently pressed, 0 otherwise.
 */
uint8_t Button_Pressed(Button* button) {
    Button_IRQ_Handler(button);

    // Check if there is a state change AND the button is currently pressed
    is_state_changed = Button_Has_Changed(button);
    is_button_pressed = (Button_Read(button) == GPIO_PIN_RESET);

    return is_state_changed && is_button_pressed;
}


/**
 * @brief Checks if there is a long press event.
 *
 * @param button Pointer to the Button structure.
 * @return 1 if there is a long press event, 0 otherwise.
 */
uint8_t Button_Long_Pressed(Button* button) {
    Button_IRQ_Handler(button);

    // Check if there is a long press event
    if(button->_long_press_event){
    	//CLear the flag and return
    	button->_long_press_event = 0;
    	return 1;
    }
    return 0;
}


/**
 * @brief Checks if there is a double press event.
 *
 * @param button Pointer to the Button structure.
 * @return 1 if there is a double press event, 0 otherwise.
 */
uint8_t Button_Double_Pressed(Button* button) {
    Button_IRQ_Handler(button);

    // Check if there is a long press event
    if(button->_double_press_event){
    	//CLear the flag and return
    	button->_double_press_event = 0;
    	return 1;
    }
    return 0;
}


/*
Flow of the program when the user presses the SWA button.

1. **Button Initialization:**
   - The `Button_Init` function initializes the `Button` structure for SWA
   with the specified GPIO port (GPIOA), pin (GPIO_PIN_0), and debounce time (50ms).

2. **Main Loop:**
   - The program enters the main loop.

4. **Button Press (User Presses SWA):**
   - The user presses the SWA button.
   - The corresponding EXTI interrupt is triggered
   (either falling or rising edge, depending on your button connection).
   - The `HAL_GPIO_EXTI_Callback` function is called due to the interrupt.
   - The `Button_IRQ_Handler` function is invoked for SWA,
   detecting the button press, updating `ignore_until`, and setting `_has_changed` to 1.

5. **Button_Pressed Check:**
   - The main loop checks for a pressed state using `Button_Pressed(&swa)`.
   - `Button_IRQ_Handler` is called again (to update debounce and `_has_changed`).
   - The function returns `1` because the button is pressed and has changed state.

6. **Handle Press Event (SWA Pressed):**
   - The program enters the block of code inside the `if (Button_Pressed(&swa))` condition.
   - You can now handle the SWA button press event. For example,
   toggle an LED or perform any other desired action.

7. **Button Release (User Releases SWA):**
   - The user releases the SWA button.
   - The corresponding EXTI interrupt is triggered again (opposite edge).
   - The `HAL_GPIO_EXTI_Callback` function is called due to the interrupt.
   - The `Button_IRQ_Handler` function is invoked for SWA,
   detecting the button release, updating `_ignore_until`, and setting `_has_changed` to 1.

8. **Button_Released Check:**
   - The main loop checks for a released state using `Button_Released(&swa)`.
   - `Button_IRQ_Handler` is called again (to update debounce and `_has_changed`).
   - The function returns `1` because the button is released and has changed state.

9. **Handle Release Event (SWA Released):**
   - The program enters the block of code inside the `if (Button_Released(&swa))` condition.
   - You can now handle the SWA button release event. For example,
   toggle an LED or perform any other desired action.

This flow provides a high-level overview of how the program responds
to the user pressing and releasing the SWA button. The `_ignore_until`
and debounce mechanisms help ensure that stable button states are considered and noise is filtered out.
*/
