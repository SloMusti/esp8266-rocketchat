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

const char* ssid = "open.wlan-si.net";
const char* password = "";

const char* host = "chat.wlan-si.net";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "4F 6F 19 74 49 CD 0A 19 AC 21 39 2A C2 38 93 31 47 A2 1B 7C";

void setup() {
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
  WiFiClientSecure client;
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

  String url = "/api/login";
  //String url = "/14s7wad1";
  Serial.print("requesting URL: ");
  Serial.println(url);

 /* client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  */             
  String postdata ="password=chatbot&user=musti_bot";
 client.print(String("POST ") + url + " HTTP/1.1\r\n" + "Host: " + host + 
    "\r\n" + "User-Agent: Arduino/1.0\r\n" + "Content-type: application/x-www-form-urlencoded\r\n" + 
    "Content-Length: " + postdata.length() + "\r\n" + "Connection: close\r\n\r\n");
client.println(postdata);

while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("#headers received");
      Serial.println(line);
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("#parse1");
  Serial.println(line); // gives 8c
  line = client.readStringUntil('\n');
  Serial.println("#parse2");
  Serial.println(line); // gives {
  line = client.readStringUntil('\n');
  Serial.println("#parse3");
  Serial.println(line);
  line = client.readStringUntil('\n');
  Serial.println("#parse4");
  Serial.println(line);
  line = client.readStringUntil('\n');
  Serial.println("#parse5");
  Serial.println(line); //auth token
  //remove text
  int commaIndex = line.indexOf('"');
  String authToken=line.substring(commaIndex+14, commaIndex+57);
  Serial.print("#authToken:");
  Serial.println(authToken);
  line = client.readStringUntil('\n');
  Serial.println("#parse6");
  Serial.println(line); //user token
  commaIndex = line.indexOf('"');
  String userId=line.substring(commaIndex+11, commaIndex+28);
  Serial.print("#userId:");
  Serial.println(userId);
      

  String postdata2 ="X-Auth-Token: ";
  postdata2 += authToken;
  postdata2 += "\r\n";
  postdata2 +="X-User-Id: ";
  postdata2 += userId;
  postdata2 += "\r\n";
  String url2 = "/api/rooms/bot_test/send";
  String postdata3 =" \"msg\" : \"OK\" ";
 client.print(String("POST ") + url2 + " HTTP/1.1\r\n" + "Host: " + host + 
    "\r\n" + "User-Agent: Arduino/1.0\r\n" + postdata2 + "Content-Length: " + postdata3.length() + "Connection: close\r\n\r\n");
 client.print(postdata3);
 
  Serial.println("request2 sent");
  while (client.connected()) {
    //String line = client.readStringUntil('\n');
    String line = client.readString();
    Serial.println(line);
  }
Serial.println("finished");
}

void loop() {
}


