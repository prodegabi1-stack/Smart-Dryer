#include <Servo.h> 
#include <DHT.h>
#include <IRremote.h> 

#define SERVO1_PIN 9
#define SERVO2_PIN 11

#define DHT22_PIN 3
#define PLOAIE_PIN_D 4
#define PLOAIE_PIN_A A0
#define IR_RECEIVE_PIN 2

#define IR_1 0xBA45FF00
#define IR_2 0xB946FF00
#define IR_left 0xF708FF00
#define IR_right 0xA55AFF00
#define IR_up 0xE718FF00
#define IR_down 0xAD52FF00
#define IR_stea 0xE916FF00
#define IR_grid 0xF20DFF00
#define IR_0 0xE619FF00
#define IR_OK 0xE31CFF00

const int VITEZA_OPRIT = 90;
const int VITEZA_IN_1    = 160;
const int VITEZA_OUT_1   = 82;
const int VITEZA_IN1    = 0;
const int VITEZA_IN2    = 25;
const int VITEZA_IN3    = 50;
const int VITEZA_OUT1   = 160;
const int VITEZA_OUT2   = 130;
const int VITEZA_IN_SET   = 91;
const int VITEZA_OUT_SET   = 89;

Servo servo1, servo2;
DHT dht(DHT22_PIN, DHT22);

const int MOD_AUTOMAT = 0;
const int MOD_MANUAL  = 1;
const int MOD_SETARE  = 2;

const int POZ_cobor = 0;
const int POZ_normal  = 1;
const int POZ_afara  = 2;

int mod_curent = MOD_MANUAL;
int poz_curent = POZ_normal;
int servo_selectat = 1;

unsigned long timpAnterior = 0;
const unsigned long interval = 5000;

void setup() {
  Serial.begin(9600);
  Serial.println("Sistem de Automatizare Initializat.");

  servo1.attach(SERVO1_PIN);
  servo1.write(VITEZA_OPRIT);

  servo2.attach(SERVO2_PIN);
  servo2.write(VITEZA_OPRIT);

  dht.begin();
  pinMode(PLOAIE_PIN_D, INPUT); 

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Sistem pornit.");
}

void modulManual(unsigned long cod_primit) {

  if (cod_primit == IR_down) {
    if (poz_curent == POZ_normal) {
      servo1.write(VITEZA_OUT_1); 
      poz_curent = POZ_cobor;
      delay(2500);
      servo1.write(VITEZA_OPRIT); 
    }
  }

  if (cod_primit == IR_up) {
    if (poz_curent == POZ_cobor) {
      servo1.write(VITEZA_IN_1);
      poz_curent = POZ_normal;
      delay(2500);
      servo1.write(VITEZA_OPRIT);  
    }
  }

  if (cod_primit == IR_left) {
    if (poz_curent == POZ_normal) {
      servo2.write(VITEZA_OUT1); 
      poz_curent = POZ_afara;
      delay(500);
      servo2.write(VITEZA_OUT2); 
      poz_curent = POZ_afara;
      delay(1800);
      servo2.write(VITEZA_OPRIT); 
    }
  }

  if (cod_primit == IR_right) {
    if (poz_curent == POZ_afara) {
      servo2.write(VITEZA_IN1); 
      poz_curent = POZ_normal;
      delay(1000);
      servo2.write(VITEZA_IN2); 
      poz_curent = POZ_normal;
      delay(2500);
      servo2.write(VITEZA_OPRIT); 
    }
  }
}

void modulAutomat() {
  unsigned long timpCurent = millis();

  if (timpCurent - timpAnterior >= interval) {
    timpAnterior = timpCurent;

    float umiditate = dht.readHumidity();
    float temperatura = dht.readTemperature();
    int ploaieD = digitalRead(PLOAIE_PIN_D);
    int ploaieA = analogRead(PLOAIE_PIN_A);

    if (isnan(umiditate) || isnan(temperatura)) {
      Serial.println("Eroare DHT!");
      return;
    }

    float dryIndex = (temperatura * 3) - umiditate;

    if (dryIndex >= 0 && ploaieA > 400 && ploaieD == HIGH) {
      if (poz_curent == POZ_normal) {
        servo2.write(VITEZA_OUT1); 
        poz_curent = POZ_afara;
        delay(500);
        servo2.write(VITEZA_OUT2); 
        poz_curent = POZ_afara;
        delay(1800);
        servo2.write(VITEZA_OPRIT); 
      }
    } 
    else {
      if (poz_curent == POZ_afara) {
        servo2.write(VITEZA_IN1); 
        poz_curent = POZ_normal;
        delay(1000);
        servo2.write(VITEZA_IN2); 
        poz_curent = POZ_normal;
        delay(2500);
        servo2.write(VITEZA_OPRIT);
      }
    }
  }
}

void modulSet(unsigned long cod_primit) {

  if (cod_primit == IR_OK) {
    servo1.write(VITEZA_OPRIT);
    servo2.write(VITEZA_OPRIT);
  }

  if (cod_primit == IR_1) {
    servo_selectat = 1;
    servo1.write(VITEZA_OPRIT);
    servo2.write(VITEZA_OPRIT);
  }

  if (cod_primit == IR_2) {
    servo_selectat = 2;
    servo1.write(VITEZA_OPRIT);
    servo2.write(VITEZA_OPRIT);
  }

  if (cod_primit == IR_right) {
    if (servo_selectat == 1) servo1.write(VITEZA_IN_SET);
    else servo2.write(VITEZA_IN_SET);
  }

  if (cod_primit == IR_left) {
    if (servo_selectat == 1) servo1.write(VITEZA_OUT_SET);
    else servo2.write(VITEZA_OUT_SET);
  }
}

void loop() {

  if (mod_curent == MOD_AUTOMAT) {
    modulAutomat();
  }

  // 🔹 IR NOU
  if (IrReceiver.decode()) {

    unsigned long cod_primit = IrReceiver.decodedIRData.decodedRawData;

    Serial.print("IR: 0x");
    Serial.println(cod_primit, HEX);

    IrReceiver.resume();

    if (cod_primit == 0xFFFFFFFF) return;

    if (cod_primit == IR_stea) {
      mod_curent = MOD_AUTOMAT;
      if (poz_curent == POZ_cobor) {
        servo1.write(VITEZA_IN_1);
        poz_curent = POZ_normal;
        delay(2500);
        servo1.write(VITEZA_OPRIT);   
      }
      return;
    }

    if (cod_primit == IR_grid) {
      mod_curent = MOD_SETARE;

      if (poz_curent == POZ_afara) {
        servo2.write(VITEZA_IN1); 
        poz_curent = POZ_normal;
        delay(1000);
        servo2.write(VITEZA_IN2); 
        poz_curent = POZ_normal;
        delay(2500);
        servo2.write(VITEZA_OPRIT);
      }

      if (poz_curent == POZ_cobor) {
        servo1.write(VITEZA_IN_1);
        poz_curent = POZ_normal;
        delay(2500);
        servo1.write(VITEZA_OPRIT);  
      }
      return;
    }

    if (cod_primit == IR_0) {
      mod_curent = MOD_MANUAL;

      if (poz_curent == POZ_afara) {
        servo2.write(VITEZA_IN1); 
        poz_curent = POZ_normal;
        delay(3000);
        servo2.write(VITEZA_OPRIT);
      }
      return;
    }

    if (mod_curent != MOD_AUTOMAT) {
      if (mod_curent == MOD_MANUAL) 
        modulManual(cod_primit);
      else 
        modulSet(cod_primit);
    }
  }
}
