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
const char content_type_url[] = "application/x-www-form-urlencoded";
const char content_type_json[] = "application/json";
char header_auth_buffer[100]; // header length is standard

//RestClient client = RestClient(host);

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

  rocket_chat_login("musti_bot","chatbot");
  rocket_chat_publicRooms();
  rocket_chat_join("Kb2ZtwkpxvYLLSj9q");

  IPAddress ip = WiFi.localIP();
  char localip[20];
  sprintf_P(localip, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]); // ? not sure what it does

 //send message
  String message = "{ \"msg\" : \"RSSI:";
  message+= String(WiFi.RSSI());
  message+=" dBm IP: ";
  message+= localip;
  message+= "\" }";

  rocket_chat_send(message,"Kb2ZtwkpxvYLLSj9q");
}


void loop() {
}

void rocket_chat_login(String username, String password){
  String login = "user=";
  login += username;
  login += "&password=";
  login += password;
  
  char message_buffer[login.length()+2];
  login.toCharArray(message_buffer, login.length()+2);
  
  //authenticate
  String  response = "";
  int statusCode = client.post("/api/login", message_buffer, &response);

  //check if successful
  if(response.indexOf("success")!=-1){
    Serial.println("Success: Login");
  }

  //parse token and userId
  
  char index_token = response.indexOf("\"authToken\": \"") + 14;
  char index_userId = response.indexOf("\"userId\": \"") + 11;
  String authToken=response.substring(index_token, index_token+43);
  String userId=response.substring(index_userId, index_userId+17);

  String header_auth= "X-Auth-Token: ";
  header_auth += authToken;
  header_auth += "\r\nX-User-Id: ";
  header_auth += userId;
  header_auth += "\r\n";
  header_auth.toCharArray(header_auth_buffer, header_auth.length());
}

void rocket_chat_publicRooms(){
  String response = "";
  client.setHeader(header_auth_buffer);
  client.setContentType("");
  int statusCode = client.get("/api/publicRooms", &response);
    //check if successful
  if(response.indexOf("success")!=-1){
    Serial.println("Success: publicRooms");
  }
}

void rocket_chat_join(String rid){
  String url = "/api/rooms/";
  url += rid;
  url += "/join";
  
  char url_buffer[url.length()+2];
  url.toCharArray(url_buffer, url.length()+2);
  
  //join room message
  
  String response = "";
  client.setHeader(header_auth_buffer);
  client.setContentType(content_type_json);
  int statusCode = client.post(url_buffer, "{}", &response);
    //check if successful
  if(response.indexOf("success")!=-1){
    Serial.println("Success: Join");
  }
}  

void rocket_chat_send(String message, String rid){

  String url = "/api/rooms/";
  url += rid;
  url += "/send";
  
  char url_buffer[url.length()+2];
  url.toCharArray(url_buffer, url.length()+2);
  
  char message_buffer[message.length()+2];
  message.toCharArray(message_buffer, message.length()+2);

  String response = "";
  client.setHeader(header_auth_buffer);
  client.setContentType(content_type_json);
  int statusCode = client.post(url_buffer, message_buffer, &response);
    //check if successful
  if(response.indexOf("success")!=-1){
    Serial.println("Success: Post");
  }
}


