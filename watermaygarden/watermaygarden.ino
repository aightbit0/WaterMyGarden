//pins
const int WATERLEVEL_PIN = A3;
const int RELAY_PIN = 3;
const int WATERSENSOR_PIN1 = A0;
const int WATERSENSOR_PIN2 = A1;
// water options
const int WATERAT = 800;
const int WATERSTOPAT = 500;
bool WATEREMPTY = false;
int TIME = 6000;
int MINVALUE = 900;
bool pumpOn = false;

void setup() 
{
  Serial.begin(9600); 
  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
  startNormalThread(&TIME);
}

//reads value from sensor
int getValue(int pin){
  int value = analogRead(pin);
  Serial.println(value);
  return value;
}

//function who set pump on or off
void pumpOnOff(bool state, int*Time){
  if(WATEREMPTY == true){
    Serial.println("Pump off WATER EMPTY !");
    digitalWrite(RELAY_PIN, LOW);
    pumpOn = false;
    *Time = 6000;
    return;
  }
  if(WATEREMPTY == false){
    if(state == true){
      Serial.println("Pump on");
      digitalWrite(RELAY_PIN, HIGH);
      *Time = 1000;
      pumpOn = true;
    }else if(state == false){
      Serial.println("Pump off");
      digitalWrite(RELAY_PIN, LOW);
      pumpOn = false;
      *Time = 6000;
    }
  }
}

//function who check if pump should be on or off
void checkIfPumpOnOrOFF(int sensorValue1, int sensorValue2){
 int value = getValue(WATERLEVEL_PIN);
 Serial.print("Sensor value: ");
 Serial.println(value);
 if(value > MINVALUE){
  Serial.println("WATER IS EMPTY !");
  if(WATEREMPTY == false){
    WATEREMPTY = true;
    pumpOnOff(false,&TIME);
  }
  return;
 }
 if(value <= MINVALUE){
    Serial.println("WATER OK");
    WATEREMPTY = false;
    if((sensorValue1 >= WATERAT || sensorValue2 >= WATERAT) && pumpOn == false){
      pumpOnOff(true, &TIME);
    }else if((sensorValue1 <= WATERSTOPAT && sensorValue2 <= WATERSTOPAT) && pumpOn == true){
      pumpOnOff(false, &TIME);
    }
  }
}

void readValues(){
   int oneValue = getValue(WATERSENSOR_PIN1);
   int twoValue = getValue(WATERSENSOR_PIN2);
   checkIfPumpOnOrOFF(oneValue, twoValue);
}

// function who waits given time but not block like delay()
int startNormalThread(int * timeValue){
  static unsigned long int waitSince = 0;
  static bool state = true;
  
  if(state == true){
    if (millis() - waitSince >= *timeValue) {
      readValues();
      state = false;
      waitSince = millis();
    }
   }else if(state == false){
      if (millis() - waitSince >= *timeValue) {
        readValues();
        state = true;
        waitSince = millis();
      }
    }
    
}
