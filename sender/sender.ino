#include <SPI.h>
#include <Wire.h>
#include <SSD1306.h>
#include <LoRa.h>
#include "SdsDustSensor.h"
//#include "images.h"

//#define LORA_BAND    433
//#define LORA_BAND    868
#define LORA_BAND    915

#define OLED_SDA    4
#define OLED_SCL    15
#define OLED_RST    16

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

SSD1306 display(0x3c, OLED_SDA, OLED_SCL);

// Forward declarations
void displayLoraData(String countStr);
//void showLogo();
  int rxPin = 3;
  int txPin = 1;
  SdsDustSensor sds(rxPin, txPin);

void setup() {
  Serial.begin(9600);
  sds.begin();

  while (!Serial);
  Serial.println();
  Serial.println("LoRa Transmitter");
   Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setContinuousWorkingPeriod().toString()); // ensures sensor has continuous working period - default but not recommended

  // Configure the LED an an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Configure OLED by setting the OLED Reset HIGH, LOW, and then back HIGH
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  delay(100);
  digitalWrite(OLED_RST, LOW);
  delay(100);
  digitalWrite(OLED_RST, HIGH);

  display.init();
  display.flipScreenVertically();


  delay(2000);

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoRa Transmitter");
  display.display();
  delay(2000);

  // Configure the LoRA radio
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(LORA_BAND * 1E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("init ok");
}

void loop() {
  static int counter = 0;

  // send packet
  LoRa.beginPacket();
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    LoRa.print("PM2.5 = ");
    LoRa.print(pm.pm25);
    LoRa.print(", PM10 = ");
    LoRa.println(pm.pm10);

    // if you want to just print the measured values, you can use toString() method as well
    LoRa.println(pm.toString());
  } else {
    // notice that loop delay is set to 0.5s and some reads are not available
    LoRa.print("Could not read values from sensor, reason: ");
    LoRa.println(pm.statusToString());
  }
  LoRa.endPacket();

 
  delay(1500);
}
