#include "LED.h"

void InitializeLEDPortPins(void)
{
	// Initialize the value and port pin direction of the LEDs.
	TURN_OFF_LED1;
	SET_LED1_AS_AN_OUTPUT;
	TURN_OFF_LED2;
	SET_LED2_AS_AN_OUTPUT;
}

