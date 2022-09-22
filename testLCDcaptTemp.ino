
#include <driver/adc.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int analogInPin = 39;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_DB_0);
  
}

void loop() {
  int value = adc1_get_raw(ADC1_CHANNEL_3);
  Serial.println("Value: " + String(value));
  float voltage = (float)value*1100./4095.;
  float temperature = voltage / 10;
  Serial.println("Millivolts: " + String(temperature));
  //LCD screen
  lcd.setCursor(0,0);
  lcd.print("Temp = ");
  lcd.print(temperature);
  lcd.println(" C  ");
  delay(1000);
}
