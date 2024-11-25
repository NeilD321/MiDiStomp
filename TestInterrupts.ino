#include <Arduino.h>

// defining the input pin for the external interrupt (GPIO2), and attachInterrupt() to it.
#define Btn1_GPIO   2

// The ISR handler routine is defined as IRAM_ATTR, 
// which is a macro to place the function in the IRAM section of the ESP32 memory.
volatile bool flag = false;

void IRAM_ATTR Ext_INT1_ISR()
{
  flag = true;
}

void setup()
{
  Serial.begin(115200);
  delay(2000);
  pinMode(Btn1_GPIO, INPUT);

  attachInterrupt(digitalPinToInterrupt(Btn1_GPIO), Ext_INT1_ISR, RISING);
  Serial.println("Interrupts attached");
}
 
void loop()
{
  if (flag)
  {
    Serial.println("Interrupt triggered");
    flag = false;
  }
}
