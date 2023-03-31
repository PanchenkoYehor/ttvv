#include <Wire.h>        /*include Wire.h library*/
void setup()
{
  Wire.begin();           /*Wire I2C communication START*/
  Serial.begin(9600);    /*baud rate set for Serial Communication*/
  while (!Serial);       /*Waiting for Serial output on Serial Monitor*/
  Serial.println("\nI2C Scanner");
}
void loop()
{
  byte err, adr;       /*variable error is defined with address of I2C*/
  int number_of_devices;
  Serial.println("Scanning.");
  number_of_devices = 0;
  for (adr = 1; adr < 127; adr++ )
  {
    Wire.beginTransmission(adr);
    err = Wire.endTransmission();

    if (err == 0)
    {
      Serial.print("I2C device at address 0x");
      if (adr < 16)
        Serial.print("0");
      Serial.print(adr, HEX);
      Serial.println("  !");
      number_of_devices++;
    }
    else if (err == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (adr < 16)
        Serial.print("0");
      Serial.println(adr, HEX);
    }
  }
  if (number_of_devices == 0)
    Serial.println("No I2C devices attached\n");
  else
    Serial.println("done\n");
  delay(5000);             /*wait 5 seconds for the next I2C scan*/
}