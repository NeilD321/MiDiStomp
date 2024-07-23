#include <Arduino.h>

volatile bool pins[40] = { 0 };

void INT1_ISR() { pins[1] = true; }
void INT2_ISR() { pins[2] = true; }
void INT3_ISR() { pins[3] = true; }
void INT4_ISR() { pins[4] = true; }
void INT5_ISR() { pins[5] = true; }
void INT6_ISR() { pins[6] = true; }
void INT7_ISR() { pins[7] = true; }
void INT8_ISR() { pins[8] = true; }
void INT9_ISR() { pins[9] = true; }
void INT10_ISR() { pins[10] = true; }
void INT11_ISR() { pins[11] = true; }
void INT12_ISR() { pins[12] = true; }
void INT13_ISR() { pins[13] = true; }
void INT14_ISR() { pins[14] = true; }
void INT15_ISR() { pins[15] = true; }
void INT16_ISR() { pins[16] = true; }
void INT17_ISR() { pins[17] = true; }
void INT18_ISR() { pins[18] = true; }
void INT19_ISR() { pins[19] = true; }
void INT20_ISR() { pins[20] = true; }
void INT21_ISR() { pins[21] = true; }
void INT22_ISR() { pins[22] = true; }
void INT23_ISR() { pins[23] = true; }
void INT24_ISR() { pins[24] = true; }
void INT25_ISR() { pins[25] = true; }
void INT26_ISR() { pins[26] = true; }
void INT27_ISR() { pins[27] = true; }
void INT28_ISR() { pins[28] = true; }
void INT29_ISR() { pins[29] = true; }
void INT30_ISR() { pins[30] = true; }
void INT31_ISR() { pins[31] = true; }
void INT32_ISR() { pins[32] = true; }
void INT33_ISR() { pins[33] = true; }
void INT34_ISR() { pins[34] = true; }
void INT35_ISR() { pins[35] = true; }
void INT36_ISR() { pins[36] = true; }
void INT37_ISR() { pins[37] = true; }
void INT38_ISR() { pins[38] = true; }
void INT39_ISR() { pins[39] = true; }
void INT40_ISR() { pins[40] = true; }

void setup()
{
  Serial.begin(115200); // Any baud rate should work
  delay(2000);

  for (int i = 0; i < 40; i++)
  {
    pinMode(i, INPUT);
  }

  attachInterrupt(digitalPinToInterrupt(1), INT1_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(2), INT2_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(3), INT3_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(4), INT4_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(5), INT5_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(6), INT6_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(7), INT7_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(8), INT8_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(9), INT9_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(10), INT10_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(11), INT11_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(12), INT12_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(13), INT13_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(14), INT14_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(15), INT15_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(16), INT16_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(17), INT17_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(18), INT18_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(19), INT19_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(20), INT20_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(21), INT21_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(22), INT22_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(23), INT23_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(24), INT24_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(25), INT25_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(26), INT26_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(27), INT27_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(28), INT28_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(29), INT29_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(30), INT30_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(31), INT31_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(32), INT32_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(33), INT33_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(34), INT34_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(35), INT35_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(36), INT36_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(37), INT37_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(38), INT38_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(39), INT39_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(40), INT40_ISR, RISING);

  Serial.println("Interrupts attached");
}

void loop()
{
  for (int i = 0; i < 40; i++)
  {
    if (pins[i])
    {
      Serial.println("Interrupt triggered");
      pins[i] = false;
      Serial.println(i);
    }
  }
}
