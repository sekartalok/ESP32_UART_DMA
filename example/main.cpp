#include <AlfredoCRSF.h>
#include "SerialHardwareDMA.h"
#define PIN_RX 38
#define PIN_TX 41

// Set up a new Serial object
SerialEsp32DMA crsfSerial(1);
AlfredoCRSF crsf;

static void sendFakeBattery(AlfredoCRSF &crsfInstance, float voltage, float current, float capacity, float remaining); 
void printChannels();
void setup()
{
  pinMode(37,OUTPUT);
  pinMode(34,OUTPUT);
  Serial.begin(115200);
  Serial.println("COM Serial initialized");
  
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_5N1, PIN_RX, PIN_TX);
  if (!crsfSerial) while (1) Serial.println("Invalid crsfSerial configuration");

  crsf.begin(crsfSerial);
}

void loop()
{
  static uint32_t lastChange = 0;
  static float batteryVoltage = 4.2f;
  static uint8_t batteryPercent = 100;
  sendFakeBattery(crsf, batteryVoltage, 0.0f, 0.0f, batteryPercent);
    // Must call crsf.update() in loop() to process data
    crsf.update();
    if(crsf.getChannel(11) > 1000){
        digitalWrite(37,HIGH);
    }else{
        digitalWrite(37,LOW);
    }
    if(crsf.getChannel(12) > 1000){
        digitalWrite(34,HIGH);
    }else{
        digitalWrite(34,LOW);
    }
    if (millis() - lastChange > 4000) {  // simulate voltage drop
      lastChange = millis();
      batteryVoltage -= 0.1f;
      if (batteryVoltage < 3.5f) batteryVoltage = 4.2f;
      batteryPercent -= 5;
      if (batteryPercent < 50) batteryPercent = 100;
    }

    printChannels();

    
}
static void sendFakeBattery(AlfredoCRSF &crsfInstance, float voltage, float current, float capacity, float remaining) {
  crsf_sensor_battery_t batt = {0};
  batt.voltage   = htobe16((uint16_t)(voltage * 10.0f));  // decivolts
  batt.current   = htobe16((uint16_t)(current * 10.0f));  // deciamps
  batt.capacity  = htobe16((uint16_t)(capacity)) << 8;    // mAh
  batt.remaining = (uint8_t)remaining;                    // %
  crsfInstance.queuePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_BATTERY_SENSOR, &batt, sizeof(batt));
}

//Use crsf.getChannel(x) to get us channel values (1-16).
void printChannels()
{
  for (int ChannelNum = 1; ChannelNum <= 16; ChannelNum++)
  {
    Serial.print(crsf.getChannel(ChannelNum));
    Serial.print(", ");
  }
  Serial.println(" ");
}