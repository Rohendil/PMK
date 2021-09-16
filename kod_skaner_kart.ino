#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10 
#define RST_PIN 9
const int buttonPin = 2;
int buttonState = 0;
LiquidCrystal lcd(2,3,4,5,6,7);

MFRC522 mfrc522(SS_PIN, RST_PIN);   
MFRC522::MIFARE_Key key;


void setup() {
        Serial.begin(9600);   
        SPI.begin();               
        mfrc522.PCD_Init();       
        Serial.println("Scan a MIFARE Classic card");
        pinMode(buttonPin, INPUT);
        lcd.begin(16, 2);
        lcd.print("TRYB ODCZYTU");
        for (byte i = 0; i < 6; i++) {
                key.keyByte[i] = 0xFF;
        }

}


void loop()
{

        /// KONTAKT Z KARTĄ/BRELOCZKIEM
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
         buttonState = digitalRead(buttonPin);
         Serial.println("card selected");

         if (buttonState == HIGH) {
                /// WYWOŁANIE NAPISU ŚWIADCZĄCEGO O FUNKCJI ZAPISU
                lcd.setCursor(0,0);
                lcd.print("TRYB ZAPISU");
                /// WYWOŁANIE FUNKCI PISZĄCEJ NA KARCIE
                for (int i = 1; i <= 63; i++) {
                        int block=i;
                        byte blockcontent[16] = {1,2,1,3,1,4,1,5,1,6,1,7,1,8,1,9};
                        pisanieKarta(block, blockcontent);
                }
         }
         else {
                /// WYWOŁANIE NAPISU ŚWIADCZĄCEGO O FUNKCJI ODCZYTU
                lcd.setCursor(0,0);
                lcd.print("TRYB ODCZYTU");
                /// WYWOŁANIE DANYCH Z KARTY
                czytanieKarta();
         }
         
}

int czytanieKarta()
{
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}


//// FUNKCJA PISZĄCA NA KARCIE
int pisanieKarta(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number = blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3; 
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}\
  Serial.print(blockNumber);
  Serial.println(" is a data block:");
  
  /// sprawdzanie czy blok na karcie jest edytowalny czy zablokowany
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed: ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;
  }

  /// Jeśli edytowalny to zapisz go
        
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);
  if (status != MFRC522::STATUS_OK) {
           Serial.print("MIFARE_Write() failed: ");
           Serial.println(mfrc522.GetStatusCodeName(status));
           return 4;
  }
  Serial.println("block was written");
}
