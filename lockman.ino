#include <PN532.h>
#include <PN532Interface.h>
#include <PN532_debug.h>
#include <emulatetag.h>
#include <llcp.h>
#include <mac_link.h>
#include <snep.h>

#include <Due.h>
#include <MifareClassic.h>
#include <MifareUltralight.h>
#include <Ndef.h>
#include <NdefMessage.h>
#include <NdefRecord.h>
#include <NfcAdapter.h>
#include <NfcDriver.h>
#include <NfcTag.h>

#include <PN532_I2C.h>
#include <Arduino.h>

#include <HTTPClient.h>
#include <WiFiManager.h>
#include <Preferences.h>


PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);
Preferences preferences;

volatile bool RFID_CONNECTED = false;

bool wm_nonblocking = false;  // change to true to use non blocking

WiFiManager wm;
WiFiManagerParameter custom_field;  // global param ( for non blocking w params )
String URL = "";
String MAC = "";

void resetWIFI() {
  wm.resetSettings();
  bool res = wm.autoConnect("AutoConnectAP", "cherokee");  // password protected ap

  if (!res) {
    Serial.println("Failed to connect.  Restarting...");
    ESP.restart();
  } 
}

void setup() {
  preferences.begin("", false); 
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);

  if (wm_nonblocking) wm.setConfigPortalBlocking(false);

  const char* custom_str = "<br/><label for='customfieldid'>Server URL</label><input type='text' name='serverURL'> ";
  new (&custom_field) WiFiManagerParameter(custom_str);  // custom html input

  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);

  bool res = wm.autoConnect("AutoConnectAP", "cherokee");  // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    resetWIFI();
  }

  Serial.println("connected!)");
  MAC = WiFi.macAddress();
  URL = getParam("serverURL");
  MAC.replace(":", "_");
}

void loop() {
  boolean success;
  // Buffer to store the UID
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  // UID size (4 or 7 bytes depending on card type)
  uint8_t uidLength;

  while (!RFID_CONNECTED) {
    RFID_CONNECTED = connectRFID();
  }

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if (success) {
    String UID = "";
    Serial.println("Card Detected");
    for (uint8_t i = 0; i < uidLength; i++) {
      UID = UID + uid[i];
    }
    Serial.print("UID: ");
    sendToServer(UID);
    delay(1000);
    RFID_CONNECTED = connectRFID();
  }

  delay(5000);
}

bool connectRFID() {
  nfc.begin();
  // Connected, show version
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("PN53x card not found!");
    return false;
  }

  //port
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware version: ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.setPassiveActivationRetries(0xFF);
  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for card (ISO14443A Mifare)...");
  Serial.println("");

  return true;
}

void sendToServer(String UID) {
  HTTPClient http;
  String sendURL = URL + "/" + MAC + "/";
  String body = "UID=" + UID;

  
  http.begin(sendURL);
  int httpCode = http.POST(body);
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

String getParam(String name) {
  String value;
  if ((wm.server) && wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  } else {
    value = preferences.getString("serverURL", "NA");
  }
  return value;
}

void saveParamCallback() {
  URL = getParam("serverURL");
  preferences.putString("serverURL", URL);
  Serial.println("server URL = " + URL);
}