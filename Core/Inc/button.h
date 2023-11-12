/**
 * @file button.h
 *
 * @brief Button handling library for STM32.
 *
 * @details This library provides functions to initialize and handle button events
 * on STM32 microcontrollers.
 *
 * @author deligent4
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f3xx_hal.h"


#define DEBOUNCE_DURATION			(uint16_t)200
#define LONG_PRESS_DURATION 		(uint16_t)1000
#define DOUBLE_PRESS_WINDOW 		(uint16_t)500


/**
 * @struct Button
 *
 * @brief Represents a button with its properties and states.
 */
typedef struct {
    GPIO_TypeDef* GPIO_Port; 		/**< GPIO port of the button. */
uint16_t _pin; 						/**< Pin number of the button. */
    uint16_t _delay; 				/**< Debounce delay for the button. */
    GPIO_PinState _state; 			/**< Current state of the button. */
    uint8_t _has_changed; 			/**< Flag indicating if the button state has changed. */
    uint32_t _press_start_time; 	/**< Timestamp when the button press started. */
    uint8_t _long_press_event; 		/**< Flag indicating a long press event. */
    uint8_t _double_press_event; 	/**< Flag indicating a double press event. */
} Button;

/**
 * @brief Initializes a button with the specified GPIO port, pin, and debounce time.
 *
 * @param button Pointer to the Button structure to be initialized.
 * @param GPIO_Port GPIO port of the button.
 * @param pin Pin number of the button.
 * @param debounce_ms Debounce time for the button in milliseconds.
 */
void Button_Init(Button* button, GPIO_TypeDef* GPIO_Port, uint16_t pin);

/**
 * @brief Handles button interrupts, debounces the button, and updates its state.
 *
 * @param button Pointer to the Button structure.
 */
void Button_IRQ_Handler(Button* button);

/**
 * @brief Checks if the button is currently pressed.
 *
 * @param button Pointer to the Button structure.
 * @return 1 if the button is pressed, 0 otherwise.
 */
uint8_t Button_Pressed(Button* button);

/**
 * @brief Checks if the button has been pressed for a long duration.
 *
 * @param button Pointer to the Button structure.
 * @return 1 if a long press event occurred, 0 otherwise.
 */
uint8_t Button_Long_Pressed(Button* button);

/**
 * @brief Checks if the button has been double-pressed.
 *
 * @param button Pointer to the Button structure.
 * @return 1 if a double press event occurred, 0 otherwise.
 */
uint8_t Button_Double_Pressed(Button* button);

#endif /* BUTTON_H */
