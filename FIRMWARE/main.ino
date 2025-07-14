#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <LoRa.h>

// ---------------------------
// OLED Display Configuration
// ---------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 22
#define OLED_SCL 21
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------------------
// BME280 Configuration (SPI)
// ---------------------------
#define BME_CS   17
#define BME_SCK  18
#define BME_MOSI 23
#define BME_MISO 19
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

// ---------------------------
// LoRa SX1262 Configuration
// ---------------------------
#define LORA_CS   5
#define LORA_RST  14
#define LORA_BUSY 15
#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_FREQ 868E6  // Adjust based on your region

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Smart Environment Monitoring Node");

  // ---------------------------
  // Initialize OLED
  // ---------------------------
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED init failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // ---------------------------
  // Initialize BME280
  // ---------------------------
  if (!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    while (1);
  }
  Serial.println("BME280 Sensor Ready");

  // ---------------------------
  // Initialize LoRa (SX1262)
  // ---------------------------
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_BUSY);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa SX1262 Ready");
}

void loop() {
  // Read sensor data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Smart Env Node");
  display.print("Temp: "); display.print(temperature); display.println(" C");
  display.print("Hum: "); display.print(humidity); display.println(" %");
  display.print("Pres: "); display.print(pressure); display.println(" hPa");
  display.display();

  // Send via LoRa
  LoRa.beginPacket();
  LoRa.print("T:"); LoRa.print(temperature);
  LoRa.print(" H:"); LoRa.print(humidity);
  LoRa.print(" P:"); LoRa.print(pressure);
  LoRa.endPacket();

  Serial.println("Data Sent via LoRa");
  delay(5000);  
}
