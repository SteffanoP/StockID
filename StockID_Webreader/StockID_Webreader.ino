//Bibliotecas conexão ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//Bibliotecas Web ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//Bibliotecas MFRC522
#include <SPI.h>
#include <MFRC522.h>

//Dados para conexão WiFi
#ifndef SSID
#define SSID "Justino                UniFibra"
#define PASSWORD "SEvERINO586"
#endif

#define IP_ADDRESS 10,0,0,200

const char* ssid = SSID;
const char* password = PASSWORD;

//Portas de configuração do MFRC522
#define PIN_SDA D3
#define PIN_RST D4

MFRC522 mfrc522(PIN_SDA, PIN_RST);   // Create MFRC522 instance.

//Porta do servidor
ESP8266WebServer server(80);
 IPAddress ip(IP_ADDRESS);
 IPAddress gateway(10,0,0,254);
 IPAddress subnet(255,255,255,0);

char constantes[2][12] = {{"FF FF FF FF"},{"56 BB 59 90"}}; //Banco de dados das tags
int valor = 0;

void leitura_MFRC522() {
  String json;
  // Procura por uma nova tag
  while ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(2000);
    valor = 0;
    json = "{\"leitura_mfrc522\":"+String(valor)+"}";
    server.send(200, "application/json", json);
    return;
  }
  // Realiza a leitura da tag por meio do protocolo Serial
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Mostra o ID na Serial
  Serial.print("ID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  conteudo.toUpperCase();

  if (conteudo.substring(1) == constantes[1]) {
    valor = 1;
    Serial.println("Tag Reconhecida");
  } else {
    valor = 2;
  }
  json = "{\"leitura_mfrc522\":"+String(valor)+"}";
  server.send(200, "application/json", json);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void showMonitor() {
  String html ="<html> <head> <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no' /> <title>StockID Monitor</title> <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'> <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js'></script> <script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script> <style type='text/css'> body { padding: 35px; background-color: #222222; } h1 { color: #FFFFFF; font-family: sans-serif; } p { color: #EEEEEE; font-family: sans-serif; font-size: 18px; } </style> </head> <div class=\"container-fluid\"> <div class=\"row\"> <div class=\"col-md-12 inserted\"> <nav> <ol class=\"breadcrumb\"> <li class=\"breadcrumb-item\"> <a href=\"/\"> <button type=\"button\" class=\"btn btn-success\" href=\"/\">Inicio</button> </a> </li> <li class=\"breadcrumb-item\"> <button type=\"button\" class=\"btn btn-success active\" href=\"#\">Monitoramento</button> </li> <li class=\"breadcrumb-item\"> <a href=\"#\"><button type=\"button\" class=\"btn btn-success\">Cadastro</button> </a> </li> </ol> </nav> </div> </div> </div> <body> <h1>StockID Monitor</h1> <p id='leitura_mfrc522'>ID da Tag: </p> </body> <script type='text/javascript'> refresh(); setInterval(refresh, 2000); function refresh() { refreshLeitura_MFRC522(); } function refreshLeitura_MFRC522() { var xmlhttp = new XMLHttpRequest(); xmlhttp.onreadystatechange = function() { if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200) { document.getElementById('leitura_mfrc522').innerHTML = 'ID da Tag: ' + JSON.parse(xmlhttp.responseText).leitura_mfrc522; } }; xmlhttp.open('GET', 'http://10.0.0.200/leitura_mfrc522', true); xmlhttp.send(); } </script> </html>";

  //Envia o html para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "text/html", html);
}

void telaInicial() {
  String html ="<!DOCTYPE html> <html lang=\"br\"> <head> <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no' /> <title>StockID</title> <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'> <style type='text/css'> body { padding: 35px; background-color: #222222; } h1 { color: #FFFFFF; font-family: sans-serif; } p { color: #EEEEEE; font-family: sans-serif; font-size: 18px; } </style> </head> <body> <div class=\"container-fluid\"> <div class=\"row\"> <div class=\"col-md-12 inserted\"> <nav> <ol class=\"breadcrumb\"> <ol class=\"breadcrumb\"> <li class=\"breadcrumb-item\"> <button type=\"button\" class=\"btn btn-success active\">Inicio</button> </li> <li class=\"breadcrumb-item\"> <a href=\"/showMonitor\"><button type=\"button\" class=\"btn btn-success\">Monitoramento</button> </a> </li> <li class=\"breadcrumb-item\"> <a href=\"#\"><button type=\"button\" class=\"btn btn-success\">Cadastro</button> </a> </li> </ol> </ol> </nav> </div> </div> </div> <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js'></script> <script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script> </body> </html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);   // Inicia a serial
  SPI.begin();      // Inicia SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);  //Set da antena para ganho máximo
  //Iniciando configs WiFi
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  //Aguarda conexão WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado à: ");
  Serial.println(ssid);
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", telaInicial);

  server.on("/showMonitor", HTTP_GET, showMonitor);

  server.on("/leitura_mfrc522", HTTP_GET, leitura_MFRC522);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");

  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}