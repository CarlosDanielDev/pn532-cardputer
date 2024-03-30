#include <M5Stack.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

// Inicializa a interface I2C para o PN532
PN532_I2C pn532_i2c(Wire);
PN532 nfc(pn532_i2c);

void setup() {
    M5.begin(true, true, true); // Inicializa o M5Stack com LCD, SD e Serial habilitados
    Serial.println("Hello Maker on M5Stack!");

    nfc.begin(); // Inicia a comunicação com o PN532
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.print("Didn't find PN53x module");
        while (1); // Halt
    }

    // Exibe informações do firmware
    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    // Configura o PN532 para leitura de tags RFID
    nfc.SAMConfig();
    Serial.println("Waiting for an ISO14443A card...");
}

void loop() {
    readNFC();
    M5.update(); // Necessário para manter o funcionamento dos botões e sistema do M5Stack
}

void readNFC() {
    boolean success;
    uint8_t uid[7] = {0}; // Buffer para armazenar o UID da tag
    uint8_t uidLength; // Tamanho do UID

    // Tenta ler o ID da tag RFID
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (success) {
        Serial.print("UID Length: ");
        Serial.print(uidLength, DEC);
        Serial.println(" bytes");

        Serial.print("UID Value: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(uid[i] < 0x10 ? " 0" : " ");
            Serial.print(uid[i], HEX);
        }
        Serial.println();

        String tagId = tagToString(uid, uidLength);
        Serial.print("tagId is: ");
        Serial.println(tagId);

        delay(1000); // Pausa de 1 segundo
    }
}

String tagToString(byte *uid, byte uidLength) {
    String tagId = "";
    for (byte i = 0; i < uidLength; i++) {
        if (i > 0) {
            tagId += ".";
        }
        tagId += String(uid[i], HEX);
    }
    return tagId;
}
