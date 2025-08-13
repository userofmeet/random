/*
  Title: LED Control with Push Button Interrupt and State Machine (Internship Task at Ardra Lab)

  Description:
  This Arduino sketch uses an external push button (on pin 2) to cycle through 
  different LED states using an interrupt service routine (ISR) for immediate 
  response. The LEDs are connected to digital pins 4, 5, and 6, and are controlled 
  via direct port manipulation for faster performance compared to digitalWrite().

  Functionality:
  - Pressing the button cycles through the following states:
      0: All LEDs OFF
      1: Only LED on pin 4 ON
      2: Only LED on pin 5 ON
      3: Only LED on pin 6 ON
      4: All LEDs ON
  - After state 4, the sequence loops back to state 1.
  - Includes a 200 ms software debounce to prevent false triggering.
  - Uses `attachInterrupt()` on the rising edge of the button signal.

  Hardware:
  - Push button connected to digital pin 2 with proper pull-down or pull-up resistor.
  - LEDs connected to pins 4, 5, and 6 (through current-limiting resistors).

  Author: Meet Jain
  Date: 13/08/2025
*/

volatile uint8_t state = 0;           
volatile unsigned long last_press = 0; 

void setup() {
  // LED on digital pin 4,5,6
  DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD6); // Configures 4,5,6 pin as output pin
  PORTD &= ~((1 << PD4) | (1 << PD5) | (1 << PD6)); // All OFF initially 

  attachInterrupt(digitalPinToInterrupt(2), interrupt_button, RISING); // Push Button on digital pin 2 and LOW to HIGH edge is considered as an interrupt
}

void loop() {
  switch (state) {
    case 0: // All OFF
      PORTD &= ~((1 << PD4) | (1 << PD5) | (1 << PD6));
      break;

    case 1: // LED1 on
      PORTD = (PORTD & ~((1 << PD5) | (1 << PD6))) | (1 << PD4);
      break;

    case 2: // LED2 om
      PORTD = (PORTD & ~((1 << PD4) | (1 << PD6))) | (1 << PD5);
      break;

    case 3: // LED3 on
      PORTD = (PORTD & ~((1 << PD4) | (1 << PD5))) | (1 << PD6);
      break;

    case 4: // All on
      PORTD |= (1 << PD4) | (1 << PD5) | (1 << PD6);
      break;
  }
}

void interruptbutton() {
  unsigned long now = millis();
  // Debounce delay
  if (now - last_press > 200) { // 200 ms debounce delay
    last_press = now;
    state++;
    if (state > 4) {
      state = 1; // Restart cycle
    }
  }
}
