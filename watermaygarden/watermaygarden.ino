//pins
const int WATERLEVEL_PIN = A3;
const int RELAY_PIN = 3;
const int WATERSENSOR_PIN1 = A0;
const int WATERSENSOR_PIN2 = A1;
//water options
const int WATERAT = 570;
const int WATERSTOPAT = 370;
bool WATEREMPTY = false;

int const TIMEDEAFAULT = 15000; // 15sec
int const TIMEFAST = 1000; // 1sec

int TIME = TIMEDEAFAULT;
int const MINVALUE = 900;
bool PUMPON = false;
bool EMERGENCYSTOP = false;
int const MAXPUMPTIMECOUNTER = 90; // 90 * 1sec = 90
int const MAXWAITTOCONTINUEPUMPCOUNT = 480; // 60sec / 15sec = 4 * 120 = 480 (2std)
int PUMPTIMECOUNTER = 0;
int WAITTOCONTINUEPUMPCOUNT = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
  startNormalThread();
}

//reads value from sensor
int getValue(int pin) {
  int value = analogRead(pin);
  Serial.println(value);
  return value;
}

//function who set pump on or off
void pumpOnOff(bool state) {
  if (WATEREMPTY == true) {
    Serial.println("Pump off WATER EMPTY !");
    digitalWrite(RELAY_PIN, LOW);
    PUMPON = false;
    TIME = TIMEDEAFAULT;
    return;
  }
  if (WATEREMPTY == false) {
    if (state == true) {
      Serial.println("Pump on");
      digitalWrite(RELAY_PIN, HIGH);
      TIME = TIMEFAST;
      PUMPON = true;
    } else if (state == false) {
      Serial.println("Pump off");
      digitalWrite(RELAY_PIN, LOW);
      PUMPON = false;
      TIME = TIMEDEAFAULT;
    }
  }
}

//function who check if pump should be on or off
void checkIfPUMPONOrOFF(int sensorValue1, int sensorValue2) {
  if (EMERGENCYSTOP == false && PUMPON == true) {
    PUMPTIMECOUNTER = PUMPTIMECOUNTER + 1;
    if (PUMPTIMECOUNTER == MAXPUMPTIMECOUNTER) {
      Serial.println("reached max pump time shutting off pump..");
      PUMPTIMECOUNTER = 0;
      EMERGENCYSTOP = true;
      pumpOnOff(false);
      return;
    }
  }
  //set back PUMPTIMECOUNTER value to 0
  if (EMERGENCYSTOP == false && PUMPON == false) {
    if (PUMPTIMECOUNTER != 0) {
      PUMPTIMECOUNTER = 0;
    }
  }
  if (EMERGENCYSTOP == true) {
    WAITTOCONTINUEPUMPCOUNT = WAITTOCONTINUEPUMPCOUNT + 1;
    if (WAITTOCONTINUEPUMPCOUNT == MAXWAITTOCONTINUEPUMPCOUNT) {
      Serial.println("reached deadline");
      EMERGENCYSTOP = false;
      WAITTOCONTINUEPUMPCOUNT = 0;
    }
    return;
  }

  Serial.print("Sensor value: ");
  int value = getValue(WATERLEVEL_PIN);
  if (value > MINVALUE) {
    Serial.println("WATER IS EMPTY !");
    if (WATEREMPTY == false) {
      WATEREMPTY = true;
      pumpOnOff(false);
    }
    return;
  }
  if (value <= MINVALUE) {
    Serial.println("WATER OK");
    WATEREMPTY = false;
    if ((sensorValue1 >= WATERAT || sensorValue2 >= WATERAT) && PUMPON == false) {
      pumpOnOff(true);
    } else if ((sensorValue1 <= WATERSTOPAT && sensorValue2 <= WATERSTOPAT) && PUMPON == true) {
      pumpOnOff(false);
    }
  }
}

void readValues() {
  int oneValue = getValue(WATERSENSOR_PIN1);
  int twoValue = getValue(WATERSENSOR_PIN2);
  checkIfPUMPONOrOFF(oneValue, twoValue);
}

// function who waits given time but not block like delay()
int startNormalThread() {
  static unsigned long int waitSince = 0;
  static bool state = true;

  if (state == true) {
    if (millis() - waitSince >= TIME) {
      readValues();
      state = false;
      waitSince = millis();
    }
  } else if (state == false) {
    if (millis() - waitSince >= TIME) {
      readValues();
      state = true;
      waitSince = millis();
    }
  }
}
