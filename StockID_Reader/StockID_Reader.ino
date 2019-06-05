#include <SPI.h>
#include <MFRC522.h>

#define PIN_SDA 3
#define PIN_RST 4

MFRC522 mfrc522(PIN_SDA, PIN_RST);   // Create MFRC522 instance.

char constantes[2][12] = {{"FF FF FF FF"},{"56 BB 59 90"}};

void setup() {
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);  //Set da antena para ganho máximo
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void loop() {
  // Procura por uma nova tag
  if (!mfrc522.PICC_IsNewCardPresent()) {
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
    Serial.println("Tag Reconhecida");
  }
}
