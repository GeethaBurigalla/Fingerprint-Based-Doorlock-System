#include <EEPROM.h>
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A0, A1);
#include <Adafruit_Fingerprint.h>
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
#define enroll 8
#define del 10
#define up A2
#define down A3
#define match 11
#define sw1 13
// #define sw2 4
// #define sw3 3
// #define resultsw 2
#define indFinger A5
#define buzzer 9
#define records 250
int vote1, vote2, vote3, vote;
int flag;
int Lock = 12;
void setup() {
  delay(1000);
  pinMode(enroll, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(del, INPUT_PULLUP);
  pinMode(match, INPUT_PULLUP);
  pinMode(sw1, OUTPUT);
  pinMode(Lock, OUTPUT);
  // pinMode(sw2, INPUT_PULLUP);
  // pinMode(sw3, INPUT_PULLUP);
  // pinMode(resultsw, INPUT_PULLUP);
  digitalWrite(sw1, HIGH);
  pinMode(buzzer, OUTPUT);
  pinMode(indFinger, OUTPUT);
  // if (digitalRead(resultsw) == 0) {
  //   for (int i = 0; i < records; i++)
  //     EEPROM.write(i + 10, 0xff);
  //   EEPROM.write(0, 0);
  //   EEPROM.write(1, 0);
  //   EEPROM.write(2, 0);
  //   lcd.clear();
  //   lcd.print("System Reset");
  //   delay(1000);
  // }
  lcd.clear();
  lcd.begin(16, 2);
  lcd.print("finger print ");
  lcd.setCursor(0, 1);
  lcd.print("based lock system");
  delay(2000);
  if (EEPROM.read(0) == 0xff)
    EEPROM.write(0, 0);
  if (EEPROM.read(1) == 0xff)
    EEPROM.write(1, 0);
  if (EEPROM.read(1) == 0xff)
    EEPROM.write(1, 0);
  finger.begin(57600);
  Serial.begin(9600);

  lcd.clear();
  lcd.print("Finding Module");
  lcd.setCursor(0, 1);
  delay(1000);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print("Found Module ");
    delay(1000);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.print("module not Found");
    lcd.setCursor(0, 1);
    lcd.print("Check Connections");
    while (1)
      ;
  }
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Match Key ");
  lcd.setCursor(0, 1);
  lcd.print("to start system");
  digitalWrite(indFinger, LOW);

  if (digitalRead(match) == 0) {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    digitalWrite(indFinger, HIGH);
    for (int i = 0; i < 3; i++) {
      lcd.clear();
      lcd.print("Place Finger");
      delay(2000);
      int result = getFingerprintIDez();
      if (result >= 0) {
        flag = 0;
        for (int i = 0; i < records; i++) {
          if (result == EEPROM.read(i + 10)) {
            lcd.clear();
            lcd.print("Authorised FP");
            lcd.setCursor(0, 1);
            lcd.print("Please Wait....");
            delay(1000);
            digitalWrite(buzzer, HIGH);
            delay(200);
            digitalWrite(buzzer, LOW);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("lock open");
            digitalWrite(Lock, LOW);  /// lock open
            delay(50000);
            //digitalWrite(sw1, HIGH);  /// lock close
            EEPROM.write(i + 10, 0xff);
            flag = 1;
            return;
          }
        }
      }
    }
    lcd.clear();
  }
  checkKeys();
  delay(1000);
}

void checkKeys() {
  if (digitalRead(enroll) == 0) {
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    while (digitalRead(enroll) == 0)
      ;
    Enroll();
  } else if (digitalRead(del) == 0) {
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    delet();
  }
}

void Enroll() {
  int count = 0;
  lcd.clear();
  lcd.print("Enter Finger ID:");
  while (1) {
    lcd.setCursor(0, 1);
    lcd.print(count);
    if (digitalRead(up) == 0) {
      count++;
      if (count > 25)
        count = 0;
      delay(500);
    }

    else if (digitalRead(down) == 0) {
      count--;
      if (count < 0)
        count = 25;
      delay(500);
    } else if (digitalRead(del) == 0) {
      id = count;
      getFingerprintEnroll();
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i + 10) == 0xff) {
          EEPROM.write(i + 10, id);
          break;
        }
      }
      return;
    }

    else if (digitalRead(enroll) == 0) {
      return;
    }
  }
}

void delet() {
  int count = 0;
  lcd.clear();
  lcd.print("Enter Finger ID");

  while (1) {
    lcd.setCursor(0, 1);
    lcd.print(count);
    if (digitalRead(up) == 0) {
      count++;
      if (count > 25)
        count = 0;
      delay(500);
    }

    else if (digitalRead(down) == 0) {
      count--;
      if (count < 0)
        count = 25;
      delay(500);
    } else if (digitalRead(del) == 0) {
      id = count;
      deleteFingerprint(id);
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i + 10) == id) {
          EEPROM.write(i + 10, 0xff);
          break;
        }
      }
      return;
    } else if (digitalRead(enroll) == 0) {
      return;
    }
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.print("finger ID:");
  lcd.print(id);
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        lcd.clear();
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.println("No Finger");
        lcd.clear();
        lcd.print("No Finger");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        //Serial.println("Communication error");
        lcd.clear();
        lcd.print("Comm Error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        //Serial.println("Imaging error");
        lcd.clear();
        lcd.print("Imaging Error");
        break;
      default:
        //Serial.println("Unknown error");
        lcd.clear();
        lcd.print("Unknown Error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.clear();
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      lcd.clear();
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      lcd.clear();
      lcd.print("Comm Error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    default:
      //Serial.println("Unknown error");
      lcd.clear();
      lcd.print("Unknown Error");
      return p;
  }

  //Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  //Serial.print("ID "); //Serial.println(id);
  p = -1;
  //Serial.println("Place same finger again");
  lcd.clear();
  lcd.print("Place Finger");
  lcd.setCursor(0, 1);
  lcd.print("   Again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        //Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        //Serial.println("Imaging error");
        break;
      default:
        //Serial.println("Unknown error");
        return;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  //Serial.print("Creating model for #");  //Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println("Fingerprints did not match");
    return p;
  } else {
    //Serial.println("Unknown error");
    return p;
  }

  //Serial.print("ID "); //Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Stored!");
    lcd.clear();
    lcd.print("Stored!");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println("Error writing to flash");
    return p;
  } else {
    //Serial.println("Unknown error");
    return p;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Finger Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Try Later");
    delay(2000);
    digitalWrite(buzzer, HIGH);
    delay(3000);
    digitalWrite(buzzer, LOW);
    return -1;
  }
  // found a match!
  //Serial.print("Found ID #");
  //Serial.print(finger.fingerID);
  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  lcd.clear();
  lcd.print("Please wait");
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Deleted!");
    lcd.clear();
    lcd.print("Figer Deleted");
    lcd.setCursor(0, 1);
    lcd.print("Successfully");
    delay(1000);
  }

  else {
    //Serial.print("Something Wrong");
    lcd.clear();
    lcd.print("Something Wrong");
    lcd.setCursor(0, 1);
    lcd.print("Try Again Later");
    delay(2000);
    return p;
  }
}