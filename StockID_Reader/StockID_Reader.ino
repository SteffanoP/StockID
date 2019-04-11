//Programa : RFID - Controle de Acesso leitor RFID
//Autor : FILIPEFLOP

#include <SPI.h>
#include <MFRC522.h>

#define led_vermelho 2
#define led_verde 3

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

char constantes[2][11] = {{"FF FF FF FF"},{"56 BB 59 90"}};
bool estadoAnteriorOcupado = 0;

void setup() {
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  Estado_livre();
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    if(estadoAnteriorOcupado == false) {
      Estado_livre();
    }
    estadoAnteriorOcupado = false;
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    Estado_livre();
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.println(constantes[1]);
  conteudo.toUpperCase();
  if (conteudo.substring(1) == constantes[1])  {
    Estado_ocupado();
    delay(500);
  }
}

void Estado_ocupado() {
  digitalWrite(led_vermelho, HIGH);
  digitalWrite(led_verde, LOW);
  Serial.println("Estado Ocupado");
  estadoAnteriorOcupado = true;
}

void Estado_livre() {
  digitalWrite(led_verde, HIGH);
  digitalWrite(led_vermelho, LOW);
}
