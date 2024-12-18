#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Alamat I2C perangkat
#define BMP280_ADDR 0x76   // Alamat I2C untuk BMP280
#define LCD_ADDR 0x27      // Alamat I2C untuk LCD

// Definisi pin
#define MQ135_PIN 34       // Pin analog untuk MQ-135 (GPIO34 pada ESP32)

// Inisialisasi objek BMP280 dan LCD
Adafruit_BMP280 bmp;  
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// Variabel untuk kalibrasi MQ-135
const float calibrationFactor = 0.1; // Faktor kalibrasi amonia (ppm)

// Threshold suhu (°C)
const float tempAmanMin = 20.0;
const float tempAmanMax = 32.0;
const float tempNormalMin = 32.0;
const float tempNormalMax = 33.0;
const float tempTinggiMin = 33.0;
const float tempTinggiMax = 35.0;

// Threshold tekanan (hPa)
const float tekananAmanMin = 1000.0;
const float tekananNormalMin = 999.0;
const float tekananNormalMax = 995.0;
const float tekananTinggiMin = 995.0;
const float tekananTinggiMax = 990.0;

// Threshold gas amonia (ppm)
const float ammoniaAmanMax = 25.0;
const float ammoniaNormalMax = 50.0;
const float ammoniaTinggiMax = 75.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();

  if (!bmp.begin(BMP280_ADDR)) {
    Serial.println("BMP280 tidak ditemukan!");
    lcd.setCursor(0, 0);
    lcd.print("BMP280 Error!");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("Inisialisasi...");
  delay(2000);
  lcd.clear();
}

// Fungsi evaluasi status berdasarkan suhu, tekanan, dan gas amonia
String evaluateStatus(float suhu, float tekanan, float ammonia) {
  if (suhu > 35.0 || suhu < 10.0 || tekanan < 990.0 || ammonia > ammoniaTinggiMax) {
    return "KRITIS";
  } else if ((suhu >= 32.0 && suhu < 33.0) || (tekanan >= 999.0 && tekanan <= 995.0) || (ammonia >= 25.0 && ammonia <= 50.0)) {
    return "NORMAL";
  } else if ((suhu > 33.0) || (tekanan >= 995.0) || (ammonia > 50.0)) {
    return "TINGGI";
  } else {
    return "AMAN";
  }
}

void loop() {
  float suhu = bmp.readTemperature();

  int gasLevelRaw = analogRead(MQ135_PIN);
  float ammoniaPpm = gasLevelRaw * calibrationFactor;

  float tekanan = bmp.readPressure() / 100.0;  // Konversi ke hPa

  String status = evaluateStatus(suhu, tekanan, ammoniaPpm);

  lcd.setCursor(0, 0);
  lcd.print("S: ");
  lcd.print(suhu, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Amonia: ");
  lcd.print(ammoniaPpm, 1);
  lcd.print(" ppm");

  delay(2000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Status: ");
  lcd.print(status);

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.println(" °C");

  Serial.print("Tekanan: ");
  Serial.print(tekanan);
  Serial.println(" hPa");

  Serial.print("Gas Amonia: ");
  Serial.print(ammoniaPpm);
  Serial.println(" ppm");

  Serial.print("Status: ");
  Serial.println(status);
  Serial.println();

  delay(1000);  // Delay untuk pembacaan berikutnya
}
