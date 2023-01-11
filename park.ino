#include <SPI.h>
#include <MFRC522.h>
#include <AccelStepper.h>

#define SDA_DIO 45
#define RESET_DIO 47
#define buzzerPin 49
MFRC522 mfrc522(SDA_DIO, RESET_DIO);

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN          -1 //PIN 2 is used

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          -1 //PIN 15 is used

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          -1 //PIN 19 is used

//extruder 1
#define E0_STEP_PIN        26
#define E0_DIR_PIN         28
#define E0_ENABLE_PIN      24

//extruder 2
#define E1_STEP_PIN        36
#define E1_DIR_PIN         34
#define E1_ENABLE_PIN      30

const long katlarArasiMesafe = 20000;
const long aracDondurmeMesafesi = 500;
const long dondurmeMesafesi = 500;
const long yurutmeMesafesi = 5000;

AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper stepperZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
AccelStepper stepperE0(AccelStepper::DRIVER, E0_STEP_PIN, E0_DIR_PIN);
AccelStepper stepperE1(AccelStepper::DRIVER, E1_STEP_PIN, E1_DIR_PIN);

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  stepperX.setEnablePin(X_ENABLE_PIN);
  stepperX.setPinsInverted(false,false,true);
  stepperX.setMinPulseWidth(100);
  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(1000);
  stepperX.setSpeed(1000);
  
  stepperY.setEnablePin(Y_ENABLE_PIN);
  stepperY.setPinsInverted(false,false,true);
  stepperY.setMinPulseWidth(100);
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(1000);
  stepperY.setSpeed(1000);

  stepperZ.setEnablePin(Z_ENABLE_PIN);
  stepperZ.setPinsInverted(false,false,true);
  stepperZ.setMinPulseWidth(100);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(1000);
  stepperZ.setSpeed(1000);

  stepperE0.setEnablePin(E0_ENABLE_PIN);
  stepperE0.setPinsInverted(false,false,true);
  stepperE0.setMinPulseWidth(100);
  stepperE0.setMaxSpeed(1000);
  stepperE0.setAcceleration(1000);
  stepperE0.setSpeed(1000);

  stepperE1.setEnablePin(E1_ENABLE_PIN);
  stepperE1.setPinsInverted(false,false,true);
  stepperE1.setMinPulseWidth(100);
  stepperE1.setMaxSpeed(1000);
  stepperE1.setAcceleration(1000);
  stepperE1.setSpeed(1000);

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Approximatethe card to the reader...");
  Serial.println();
  stepperX.enableOutputs();
  stepperY.enableOutputs();
  stepperZ.enableOutputs();
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String content= "";
  Serial.print("UID tag :");

  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "91 2F 40") {
    stepperX.moveTo(katlarArasiMesafe);
    stepperY.moveTo(aracDondurmeMesafesi);
    stepperZ.moveTo(dondurmeMesafesi);
    stepperX.run();
    stepperY.run();
    stepperZ.run();
    while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0 || stepperZ.distanceToGo() != 0) {
      stepperX.run();
      stepperY.run();
      stepperZ.run();
    }

    stepperX.moveTo(-yurutmeMesafesi);
    stepperX.run();
    while (stepperX.distanceToGo() != 0) {
      stepperX.run();
    }
  } else {
    Serial.println("Access denied");
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
