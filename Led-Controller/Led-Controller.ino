#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "credentials.h"
#include "SPIFFS.h"
#include "ledBuffer.h"
#include "pages.h"

//shifting to async web server
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

#define FILESYSTEM SPIFFS
// You only need to format the filesystem once
#define FORMAT_FILESYSTEM false
#define DBG_OUTPUT_PORT Serial

#if FILESYSTEM == FFat
#include <FFat.h>
#endif
#if FILESYSTEM == SPIFFS
#include <SPIFFS.h>
#endif

const char* host = "esp32";
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const char * hostName = "esp-async";
const char* http_username = "admin";
const char* http_password = "admin";



String processor(const String& var)
{
  return String();
}
/*
 * setup function
 */
void setup(void) {
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  
  //on file request serve from storage
//  server.serveStatic("/", SPIFFS, "/www/");
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
//  server.on("/index", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/plain", index_html);
//  });
//
//  server.on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/plain", server_index_html);
//  });
  
//  //called when the url is not defined here
//  //use it to load content from FILESYSTEM
//  server.onNotFound([](AsyncWebServerRequest *request){
//    Serial.printf("NOT_FOUND: ");
//    if(request->method() == HTTP_GET)
//      Serial.printf("GET");
//    else if(request->method() == HTTP_POST)
//      Serial.printf("POST");
//    else if(request->method() == HTTP_DELETE)
//      Serial.printf("DELETE");
//    else if(request->method() == HTTP_PUT)
//      Serial.printf("PUT");
//    else if(request->method() == HTTP_PATCH)
//      Serial.printf("PATCH");
//    else if(request->method() == HTTP_HEAD)
//      Serial.printf("HEAD");
//    else if(request->method() == HTTP_OPTIONS)
//      Serial.printf("OPTIONS");
//    else
//      Serial.printf("UNKNOWN");
//    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
//
//    if(request->contentLength()){
//      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
//      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
//    }
//
//    int headers = request->headers();
//    int i;
//    for(i=0;i<headers;i++){
//      AsyncWebHeader* h = request->getHeader(i);
//      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
//    }
//
//    int params = request->params();
//    for(i=0;i<params;i++){
//      AsyncWebParameter* p = request->getParam(i);
//      if(p->isFile()){
//        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
//      } else if(p->isPost()){
//        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
//      } else {
//        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
//      }
//    }
//
//    request->send(404);
//  });
  
  server.onNotFound(notFound);
  server.begin();
}

void loop(void) {
  ws.cleanupClients();
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  }
  
bool exists(String path){
  bool yes = false;
  File file = FILESYSTEM.open(path, "r");
  if(!file.isDirectory()){
    yes = true;
  }
  file.close();
  return yes;
}
