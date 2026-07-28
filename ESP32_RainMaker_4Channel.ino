#include <EEPROM.h>
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <AceButton.h>

using namespace ace_button;

#define ENABLE_EEPROM true
#define USE_LATCHED_SWITCH true
#define EEPROM_SIZE 10
#define NUM_CHANNELS 4

const char *service_name = "PROV_12345";
const char *pop = "1234567";

const uint8_t wifiLed = 2;
const uint8_t gpio_reset = 0;

const uint8_t relayPins[NUM_CHANNELS]   = {23, 19, 18, 5};
const uint8_t switchPins[NUM_CHANNELS]  = {13, 12, 14, 27};
const char* deviceNames[NUM_CHANNELS]   = {"Switch1", "Switch2", "Switch3", "Switch4"};

bool toggleStates[NUM_CHANNELS] = {false};

ButtonConfig buttonConfig;
AceButton buttons[NUM_CHANNELS];
Switch* rmakerSwitches[NUM_CHANNELS];

void writeEEPROM(int addr, bool state) {
  if (ENABLE_EEPROM) {
    EEPROM.write(addr, state);
    EEPROM.commit();
  }
}

bool readEEPROM(int addr) {
  return ENABLE_EEPROM ? EEPROM.read(addr) : false;
}

void setRelay(int idx, bool state) {
  toggleStates[idx] = state;
  digitalWrite(relayPins[idx], !state);
  writeEEPROM(idx, state);
}

void buttonHandler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  uint8_t id = button->getId();
  if (id >= NUM_CHANNELS) return;

  bool newState = false;
  if (USE_LATCHED_SWITCH) {
    newState = (eventType == AceButton::kEventPressed);
  } else {
    if (eventType != AceButton::kEventReleased) return;
    newState = !toggleStates[id];
  }

  setRelay(id, newState);
  rmakerSwitches[id]->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, newState);
  Serial.printf("Relay %d toggled manually -> %d\n", id + 1, newState);
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  const char *dev_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(param_name, "Power") == 0) {
    bool newState = val.val.b;
    for (int i = 0; i < NUM_CHANNELS; i++) {
      if (strcmp(dev_name, deviceNames[i]) == 0) {
        setRelay(i, newState);
        rmakerSwitches[i]->updateAndReportParam(param_name, newState);
        Serial.printf("RainMaker set %s -> %d\n", dev_name, newState);
        break;
      }
    }
  }
}

void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("Provisioning started: %s\n", service_name);
      printQR(service_name, pop, "ble");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Wi-Fi connected");
      digitalWrite(wifiLed, HIGH);
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      digitalWrite(wifiLed, LOW);
      break;
  }
}

void checkResetButton() {
  if (digitalRead(gpio_reset) == LOW) {
    delay(100);
    uint32_t start = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    uint32_t duration = millis() - start;

    if (duration > 10000) {
      Serial.println("Factory reset...");
      RMakerFactoryReset(2);
    } else if (duration > 3000) {
      Serial.println("WiFi reset...");
      RMakerWiFiReset(2);
    }
  }
}

void setup() {
  Serial.begin(115200);

  if (ENABLE_EEPROM) EEPROM.begin(EEPROM_SIZE);

  pinMode(wifiLed, OUTPUT);
  pinMode(gpio_reset, INPUT_PULLUP);
  digitalWrite(wifiLed, LOW);

  buttonConfig.setEventHandler(buttonHandler);

  Node my_node = RMaker.initNode("ESP32_Relay_4");

  for (int i = 0; i < NUM_CHANNELS; i++) {
    pinMode(relayPins[i], OUTPUT);
    pinMode(switchPins[i], INPUT_PULLUP);

    toggleStates[i] = readEEPROM(i);
    digitalWrite(relayPins[i], !toggleStates[i]);

    buttons[i].init(&buttonConfig, switchPins[i], HIGH, i);

    rmakerSwitches[i] = new Switch(deviceNames[i], (void*)&relayPins[i]);
    rmakerSwitches[i]->addCb(write_callback);
    my_node.addDevice(*rmakerSwitches[i]);
  }

  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.enableTZService();
  RMaker.enableSchedule();
  RMaker.start();

  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(
    WIFI_PROV_SCHEME_BLE, 
    WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, 
    WIFI_PROV_SECURITY_1, 
    pop, 
    service_name
  );

  for (int i = 0; i < NUM_CHANNELS; i++) {
    rmakerSwitches[i]->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleStates[i]);
  }

  Serial.println("Setup completed.");
}

void loop() {
  checkResetButton();

  digitalWrite(wifiLed, WiFi.status() == WL_CONNECTED);

  for (int i = 0; i < NUM_CHANNELS; i++) {
    buttons[i].check();
  }
}
