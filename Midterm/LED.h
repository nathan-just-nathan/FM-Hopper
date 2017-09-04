#ifndef LED_H
#define LED_H

#include <msp430.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                 LED #1
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Color     :  Red
 *   Polarity  :  Active High
 *   GPIO      :  P1.0
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define LED1 1
#define LED1_BIT				BIT0
#define LED1_PORT				P1OUT
#define LED1_DDR				P1DIR
#define SET_LED1_AS_AN_OUTPUT	LED1_DDR |= LED1_BIT
#define TURN_ON_LED1			LED1_PORT |= LED1_BIT
#define TURN_OFF_LED1			LED1_PORT &= ~LED1_BIT
#define TOGGLE_LED1				LED1_PORT ^= LED1_BIT

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                 LED #2
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Color     :  Green
 *   Polarity  :  Active High
 *   GPIO      :  P1.6
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define LED2 2
#define LED2_BIT				BIT6
#define LED2_PORT				P1OUT
#define LED2_DDR				P1DIR
#define SET_LED2_AS_AN_OUTPUT	LED2_DDR |= LED2_BIT
#define TURN_ON_LED2			LED2_PORT |= LED2_BIT
#define TURN_OFF_LED2			LED2_PORT &= ~LED2_BIT
#define TOGGLE_LED2				LED2_PORT ^= LED2_BIT

// Prototypes
void InitializeLEDPortPins(void);

#endif
