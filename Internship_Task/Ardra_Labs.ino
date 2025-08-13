/*
 * LED Sequence Controller with Hardware Interrupt 
 * Based on working code by Meet Jain
 * 3 LEDs connected to pins 4, 5, 6 (Port D)
 * Push button connected to pin 2 (INT0) with hardware interrupt
 * Register level programming for LED control
 * Hardware debouncing for button
 */

#define LED1_PIN 4   // PD4 (Port D, bit 4)
#define LED2_PIN 5   // PD5 (Port D, bit 5) 
#define LED3_PIN 6   // PD6 (Port D, bit 6)
  
// Button pin
#define BUTTON_PIN 2  // PD2 (INT0)

// Variables for button debouncing and state
volatile unsigned long lastInterruptTime = 0;
volatile bool buttonPressed = false;
const unsigned long debounceDelay = 50; // 50ms debounce delay

// LED sequence state
volatile int currentState = 0; // 0=all off, 1=LED1, 2=LED2, 3=LED3, 4=all on

void setup() {
  Serial.begin(9600);
  // Configure LED pins as outputs 
  // Set bits 4, 5, 6 of DDRD  as outputs
  DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD6);
  // Initialize all LEDs to OFF
  PORTD &= ~((1 << PD4) | (1 << PD5) | (1 << PD6));
  // Configure button pin as input with internal pull-up
  // Set PD2 as input 
  DDRD &= ~(1 << PD2);
  // internal pull-up for PD2
  PORTD |= (1 << PD2);
  // Configure external interrupt INT0 (pin 2)
  // Set interrupt to trigger on button press
  EICRA |= (1 << ISC01);    // Set ISC01 bit
  EICRA &= ~(1 << ISC00);   // Clear ISC00 bit (falling edge)
  // Enable INT0 interrupt
  EIMSK |= (1 << INT0);
  // Enable global interrupts
  sei();
  Serial.println("LED Sequence Controller Started");
  Serial.println("Press button to cycle through LED patterns");
  // Debug: Print initial register values
  Serial.print("DDRD: 0b");
  Serial.println(DDRD, BIN);
  Serial.print("PORTD: 0b");
  Serial.println(PORTD, BIN);
}

void loop() {
  // Check if button was pressed
  if (buttonPressed) {
    buttonPressed = false;
    
    // Move to next state
    currentState++;
    if (currentState > 4) {
      currentState = 1; // Reset to state 1 after state 4
    }
    
    // Update LEDs based on current state
    updateLEDs();
    
    // Print current state 
    Serial.print("State: ");
    Serial.println(currentState);
  }
  
  // Small delay 
  delay(10);
}

// External interrupt service routine for INT0 (pin 2)
ISR(INT0_vect) {
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > debounceDelay) {
    buttonPressed = true;
    lastInterruptTime = interruptTime;
  }
}

void updateLEDs() {
  // print state before changes
  Serial.print("Before - PORTD: 0b");
  Serial.println(PORTD, BIN);
  
  switch (currentState) {
    case 0: // All LEDs OFF
      PORTD &= ~((1 << PD4) | (1 << PD5) | (1 << PD6));
      Serial.println("All LEDs OFF");
      break;
      
    case 1: // Only LED1 ON (Pin 4)
      PORTD = (PORTD & ~((1 << PD5) | (1 << PD6))) | (1 << PD4);
      Serial.println("LED1 ON (Pin 4)");
      break;
      
    case 2: // Only LED2 ON (Pin 5)
      PORTD = (PORTD & ~((1 << PD4) | (1 << PD6))) | (1 << PD5);
      Serial.println("LED2 ON (Pin 5)");
      break;
      
    case 3: // Only LED3 ON (Pin 6)
      PORTD = (PORTD & ~((1 << PD4) | (1 << PD5))) | (1 << PD6);
      Serial.println("LED3 ON (Pin 6)");
      break;
      
    case 4: // All LEDs ON
      PORTD |= (1 << PD4) | (1 << PD5) | (1 << PD6);
      Serial.println("All LEDs ON");
      break;
  }
  
  // Print state after changes
  Serial.print("After - PORTD: 0b");
  Serial.println(PORTD, BIN);
  Serial.println("---");
}
