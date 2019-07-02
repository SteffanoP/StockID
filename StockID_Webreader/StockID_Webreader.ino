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
#define SSID "ssid"
#define PASSWORD "password"
#endif

#define IP_ADDRESS 10,0,0,200

const char* ssid = SSID;
const char* password = PASSWORD;

//Portas de feedback da conexão WiFi
#define PIN_LEDCONEXAO D1

//Portas de configuração do MFRC522
#define PIN_SDA D3
#define PIN_RST D8

MFRC522 mfrc522(PIN_SDA, PIN_RST);   // Create MFRC522 instance.

//Porta do servidor
ESP8266WebServer server(80);
 IPAddress ip(IP_ADDRESS);
 IPAddress gateway(10,0,0,254);
 IPAddress subnet(255,255,255,0);

char constantes[2][12] = {{"FF FF FF FF"},{"56 BB 59 90"}}; //Banco de dados das tags
int valor = 0;
short txLeituras = 0;

void leitura_MFRC522() {
  String json;
  // Procura por uma nova tag
  while ( ! mfrc522.PICC_IsNewCardPresent()) {
    if (txLeituras > 2)
    valor = 0;
    txLeituras++;
    
    json = "{\"leitura_mfrc522\":"+String(valor)+"}";
    server.send(200, "application/json", json);
    return;
  }
  // Realiza a leitura da tag por meio do protocolo Serial
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  txLeituras = 0;
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
  String html ="<html lang=\"pt-br\"> <head> <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no' /> <title>StockID Monitor</title> <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'> <link href=\"https://fonts.googleapis.com/css?family=Montserrat|Roboto&display=swap\" rel=\"stylesheet\"> <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js'></script> <script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script> <style type='text/css'> body { padding: 35px; background-color: #222222; font-family: 'Roboto', sans-serif; } h1 { color: #FFFFFF; font-family: 'Montserrat', sans-serif; } p { color: #EEEEEE; font-family: 'Roboto', sans-serif; font-size: 18px; } .flex { display: flex; } .flex-wrap { flex-wrap: wrap; } .center { align-items: center; } .baseline { align-items: baseline; } .box { margin: 10px; width: 200px; height: 150px; background-color: #707070; border-radius: 0.2em; text-align: center; font-size: 1.5em; } .box_name { height: 30px; background-color: #FFFFFF; border-bottom: solid white; border-radius: 0.3em 0.3em 0em 0em } .box_content { height: 90px; text-align: center; } .box_footer { height: 30px; background-color: #FFFFFF; border-radius: 0em 0em 0.3em 0.3em } .item { flex: 1; margin: 5px; padding: 0 10px; background: tomato; text-align: center; font-size: 1.5em; } </style> </head> <body> <div class=\"container-fluid\"> <div class=\"row\"> <div class=\"col-md-12 inserted\"> <nav> <ol class=\"breadcrumb\"> <li class=\"breadcrumb-item\"> <a href=\"/\"> <button type=\"button\" class=\"btn btn-success\" href=\"/\">Inicio</button> </a> </li> <li class=\"breadcrumb-item\"> <button type=\"button\" class=\"btn btn-success active\" href=\"#\">Monitoramento</button> </li> <li class=\"breadcrumb-item\"> <a href=\"#\"><button type=\"button\" class=\"btn btn-success\">Cadastro</button> </a> </li> </ol> </nav> </div> </div> </div> <h1>StockID Monitor</h1> <p id='leitura_mfrc522'>ID da Tag: </p> <div class=\"container flex flex-wrap\"> <div class=\"box\"> <div class=\"box_name\"> Tag P1 </div> <div class=\"box_content\"> </div> <div class=\"box_footer\"> Endereço </div> </div> <div class=\"box\"> <div class=\"box_name\"> Tag P2 </div> <div class=\"box_content\"> </div> <div class=\"box_footer\"> Endereço </div> </div> <div class=\"box\"> <div class=\"box_name\"> Tag P3 </div> <div class=\"box_content\"> </div> <div class=\"box_footer\"> Endereço </div> </div> <div class=\"box\"> <div class=\"box_name\"> Tag P4 </div> <div class=\"box_content\"> </div> <div class=\"box_footer\"> Endereço </div> </div> </div> </body> <script type='text/javascript'> refresh(); setInterval(refresh, 250); function refresh() { refreshLeitura_MFRC522(); } function refreshLeitura_MFRC522() { var xmlhttp = new XMLHttpRequest(); xmlhttp.onreadystatechange = function() { if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200) { document.getElementById('leitura_mfrc522').innerHTML = 'ID da Tag: ' + JSON.parse(xmlhttp.responseText).leitura_mfrc522; } }; xmlhttp.open('GET', 'http://10.0.0.200/leitura_mfrc522', true); xmlhttp.send(); } </script> </html>";


  //Envia o html para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "text/html", html);
}

void telaInicial() {
  String html ="<!DOCTYPE html> <html lang=\"pt-br\"> <head> <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no' /> <title>StockID</title> <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'> <link href=\"https://fonts.googleapis.com/css?family=Montserrat|Roboto&display=swap\" rel=\"stylesheet\"> <style type='text/css'> body { padding: 35px; background-color: #222222; font-family: 'Roboto', sans-serif; } h1 { color: #FFFFFF; font-family: 'Montserrat', sans-serif; } p { color: #EEEEEE; font-family: 'Roboto', sans-serif; font-size: 18px; } </style> </head> <body> <div class=\"container-fluid\"> <div class=\"row\"> <div class=\"col-md-12 inserted\"> <nav> <ol class=\"breadcrumb\"> <ol class=\"breadcrumb\"> <li class=\"breadcrumb-item\"> <button type=\"button\" class=\"btn btn-success active\">Inicio</button> </li> <li class=\"breadcrumb-item\"> <a href=\"/showMonitor\"><button type=\"button\" class=\"btn btn-success\">Monitoramento</button> </a> </li> <li class=\"breadcrumb-item\"> <a href=\"#\"><button type=\"button\" class=\"btn btn-success\">Cadastro</button> </a> </li> </ol> </ol> </nav> </div> </div> </div> <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js'></script> <script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script> </body> </html>";

  server.send(200, "text/html", html);
}

void conectando() {
  digitalWrite(PIN_LEDCONEXAO, HIGH);
  delay(250);
  Serial.print(".");
  digitalWrite(PIN_LEDCONEXAO, LOW);
  delay(250);
}

void conectado() {
  Serial.println();
  Serial.print("Conectado à: ");
  Serial.println(ssid);
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  digitalWrite(PIN_LEDCONEXAO, HIGH);
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
  pinMode(PIN_LEDCONEXAO, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    conectando();
  }
  //Conexão WiFi estabelecida
  conectado();

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
