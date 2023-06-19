void setup() {
  Serial.begin(9600);
  pinMode(6,INPUT); //input pin coming from soil sensor
  pinMode(8,INPUT); //input pin coming from tank sensor
  pinMode(7, OUTPUT); //pin for the relais
}

long int scanTime = 0;
long int maxPumpTime = 0;
bool pumpActive = false;

const int TANK_PIN = 8;
const int SENSOR_PIN = 6;
const int SECOND_SENSOR_PIN = 9;
const int RELAIS_PIN = 7;

void loop() {
   //emergency stop
    if (millis() >= scanTime) {
      if(checkPinState(TANK_PIN)){
         digitalWrite(RELAIS_PIN, LOW);
         pumpActive = false;
      }else if(checkPinState(SENSOR_PIN) || checkPinState(SECOND_SENSOR_PIN)){
         digitalWrite(RELAIS_PIN, HIGH);
         if(!pumpActive){
          pumpActive = true;
          //maxPumpTime = millis()+ 30000;
         }
      }else{
        pumpActive = false;
        digitalWrite(RELAIS_PIN, LOW);
      }
      if(pumpActive){
        scanTime = millis()+ 2500;
      }else{
        scanTime = millis()+ 25000;
      }
    }
}

bool checkPinState(int pin){
  if(digitalRead(pin) == 1){
    return true;
  }
  return false;
}
