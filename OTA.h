#ifndef __OTA_H
#define __OTA_H
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebSrv.h>
#include <Update.h>
#include <ArduinoJson.h>
#define SSID_OTA "UPDATE FIRMWARE"
#define PASSWORD_OTA "12345678"
void Ini_OTA();
void Process_OTA();
#endif
