#include <Wire.h>  //needed because DS3231 uses I2C Bus
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


RTC_DS3231 rtc;  //the object rtc is created from the class RTC_DS3231
char daysOfTheWeek[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
#define deviceAddress 0x68

int ledState = 0;                   // ledState used to set the LED
unsigned long previousMillis = 10;  // will store last time LED was updated
long interval = 10;                 // frequency interval (ms)
long interval_ask = 2000;           // call rtc every {interval_ask} (ms)
long last_asked = 0;                // time when we asked rtc for the last time
long last_notification = 0;
long interval_notification = 20 * 1000;
int led = 0;

enum Prescalers {
  PRESCALER_1 = 1,
  PRESCALER_8 = 2,
  PRESCALER_64 = 3,
  PRESCALER_256 = 4,
  PRESCALER_1024 = 5
};

void setup() {
  lcd.init();  // initialize the lcd
  lcd.backlight();
  // lcd.print("First line");

  // uint8_t prescaler = PRESCALER_1024;
  // uint16_t topValue = 65535;
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  // TCCR1A = 0x40;          // Инвертирование пина 9 по сравнению
  // TCCR1B = 0x08 | prescaler;  // Установить СТС режим и делитель частоты
  // OCR1A = topValue;       // установить TOP равным topValue
  Serial.begin(9600);
  rtc.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
}

float lerp(float a, float b, float x) {
  return a + x * (b - a);
}

void draw0(unsigned long currentMillis, unsigned long freq, int down) {
  unsigned long x = currentMillis % freq;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // interval = (sin(currentMillis*(0.01 + 0.005*sin(currentMillis*0.001)))*0.4 + 0.45)*60;
    double w_min = 5;
    double w_max = 40;
    int div = freq == 250 ? 500 : 1000;
    interval = lerp(down * w_min + (1 - down) * w_max, down * w_max + (1 - down) * w_min, (double)x / div);
    ledState = 1 - ledState;

    if ((interval > (w_max + w_min) / 4 && down == 1) || (interval > 3 * (w_max + w_min) / 4 && down == 0)) {
      led = 1;
    } else {
      led = 0;
    }
  }
}

unsigned long sector = 0;
unsigned long last_changed = 0;
unsigned long to_beat = 0;
unsigned long beat_duration = 3000;
unsigned long beat_pulsation = 5;

void printFirstLine(byte bcdHours, byte bcdMinutes, byte bcdSeconds) {
  lcd.clear();
  lcd.backlight();
  lcd.print(bcdHours >> 4);
  lcd.print(bcdHours & 0x0F);
  lcd.print(':');
  lcd.print(bcdMinutes >> 4);
  lcd.print(bcdMinutes & 0x0F);
  lcd.print(':');
  lcd.print(bcdSeconds >> 4);
  lcd.print(bcdSeconds & 0x0F);
}

void printSecondLine(int sector) {
  lcd.setCursor(0, 1);
  lcd.print("Sector = ");
  lcd.print(sector);
}


void loop() {
  // return;
  unsigned long currentMillis = millis();

  // Serial.println(currentMillis);
  // Serial.print(" ");
  // {
  //   ledState = 1 - ledState;
  //   digitalWrite(8, ledState);
  //   digitalWrite(9, 1 - ledState);
  //   return;
  // }
  Serial.print(digitalRead(6));
  Serial.print(" ");
  Serial.print(digitalRead(6));
  Serial.print(" ");

  if (digitalRead(6) == digitalRead(7)) {
    digitalWrite(10, LOW);
  } else {
    digitalWrite(10, HIGH);
  }
  // lcd.clear();
  // lcd.print("First line");
  // return;
  if (currentMillis - last_asked >= interval_ask) {
    last_asked = currentMillis;
    DateTime nowTime = rtc.now();
    Wire.beginTransmission(deviceAddress);  //START, Roll Cal
    Wire.write(0x00);                       //set SEC Register address
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, 3);
    byte bcdSeconds = Wire.read();
    byte bcdMinutes = Wire.read();
    byte bcdHours = Wire.read();

    Serial.print(bcdSeconds >> 4);
    Serial.print(bcdSeconds & 0x0F);
    Serial.println();

    int current_sec = 10 * (bcdSeconds >> 4) + (bcdSeconds & 0x0F);
    sector = current_sec / 10;
    printFirstLine(bcdHours, bcdMinutes, bcdSeconds);

    if (digitalRead(6) == LOW && digitalRead(7) == HIGH) {
      printFirstLine(bcdHours, bcdMinutes, bcdSeconds);
    } else if (digitalRead(6) == HIGH && digitalRead(7) == LOW) {
      printFirstLine(bcdHours, bcdMinutes, bcdSeconds);
      printSecondLine(sector);
    } else {
      lcd.noBacklight();
      lcd.clear();
    }
  }

  if (currentMillis - last_notification >= interval_notification) {
    last_notification = currentMillis;
    to_beat = currentMillis + random(3 * 1000, interval_notification - 3 * 1000);
  }

  int beating = 0;
  if (currentMillis >= to_beat && currentMillis <= to_beat + beat_duration) {
    unsigned long value = (currentMillis - to_beat) % (2 * beat_pulsation);
    if (value <= beat_pulsation) {
      ledState = 1;
    } else {
      ledState = 0;
    }
    beating = 25;
  } else {
    // if (currentMillis > 6000 * 4) {
    //   currentMillis = 0;
    //   previousMillis = 10;
    // }
    // unsigned long sector = (currentMillis % 6000) / 1000;
    // if (currentMillis  - last_changed >= every) {
    //   last_changed = currentMillis;
    //   unsigned long new_sector;
    //   do {
    //     new_sector = random(0, 6);
    //   } while (new_sector == sector);
    //   // } while (false);
    //   sector = new_sector;
    //   // sector = 5;
    //   Serial.print(sector);
    //   Serial.print(" ");
    // }
    if (sector < 2) {
      draw0(currentMillis, 1000, sector % 2);
    } else if (sector < 4) {
      draw0(currentMillis, 500, sector % 2);
    } else {
      draw0(currentMillis, 333, sector % 2);
    }
  }
  Serial.print(interval);
  Serial.print(" ");
  Serial.print(sector * 10);
  Serial.print(" ");
  Serial.print(beating);
  Serial.print(" ");
  Serial.print("0");
  Serial.print(" ");
  Serial.println(60);

  digitalWrite(8, ledState);
  digitalWrite(9, 1 - ledState);
  digitalWrite(LED_BUILTIN, led);
}
