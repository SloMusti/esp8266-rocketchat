/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "RestClient.h"


#define HTTP_DEBUG



const char* ssid = "open.wlan-si.net";
const char* password = "";

const char* host = "chat.wlan-si.net";
//const char* host = "requestb.in";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "4F 6F 19 74 49 CD 0A 19 AC 21 39 2A C2 38 93 31 47 A2 1B 7C";
//const char* fingerprint = "E1 17 1D 8B 13 58 02 C9 C2 6A 11 98 73 45 74 90 16 C1 C7 00";


RestClient client = RestClient(host,443,fingerprint);

//RestClient client = RestClient(host);

void setup() {
  StaticJsonBuffer<512> jsonBuffer;
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  /*WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  

   if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  */
  //authenticate
  String  response = "";
  int statusCode = client.post("/api/login", "user=musti_bot&password=chatbot", &response);
  Serial.print(" LOGON Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Response body from server: ");
  Serial.println(response);
  
  char index_token = response.indexOf("\"authToken\": \"") + 14;
  char index_userId = response.indexOf("\"userId\": \"") + 11;

  String authToken=response.substring(index_token, index_token+43);
  String userId=response.substring(index_userId, index_userId+17);

// list rooms
  String header = "X-Auth-Token: ";
  header += authToken;
  header += "\r\nX-User-Id: ";
  header += userId;
  header += "\r\n";

  const char content_type[] = "application/x-www-form-urlencoded";
  const char content_type2[] = "application/json";
  
  char header_buffer[header.length()];
  header.toCharArray(header_buffer, header.length());

  response = "";
  client.setHeader(header_buffer);
  client.setContentType("");
  statusCode = client.get("/api/publicRooms", &response);
  Serial.print("ROOMS Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Response body from server: ");
  Serial.println(response);

  //join room message

  response = "";
  client.setHeader(header_buffer);
  client.setContentType(content_type2);
  statusCode = client.post("/api/rooms/Kb2ZtwkpxvYLLSj9q/join", "{}", &response);
  Serial.print("JOIN status code from server: ");
  Serial.println(statusCode);
  Serial.print("Response body from server: ");
  Serial.println(response);
  

  IPAddress ip = WiFi.localIP();
  char localip[20];
  sprintf_P(localip, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]); // ? not sure what it does

 //send message
  String message = "{ \"msg\" : \"RSSI:";
  message+= String(WiFi.RSSI());
  message+=" dBm IP: ";
  message+= localip;
  message+= "\" }";

  char message_buffer[message.length()+2];
  message.toCharArray(message_buffer, message.length()+2);

  response = "";
  client.setHeader(header_buffer);
  client.setContentType(content_type2);
  statusCode = client.post("/api/rooms/Kb2ZtwkpxvYLLSj9q/send", message_buffer, &response);
  Serial.print("SEND MESSAGE status code from server: ");
  Serial.println(statusCode);
  Serial.print("Response body from server: ");
  Serial.println(response);
 

}

void loop() {
}


