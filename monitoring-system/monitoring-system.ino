// Adding the various libraries
#include <dht.h>
#include <Keypad.h>
#include <Servo.h>

// Defining pin numbers
#define water_sensor A0
#define alarm 8
#define pirPin 2
#define LED 3
#define DHT11_PIN 7
// Defining password info
#define Password_Length 8
char Data[Password_Length];
char Master[Password_Length] = "123A456"; // Change the password here
 
 
 // Information for the servo
int pos = 0;
dht DHT;
Servo myservo;


// Information for the Keypad
const byte ROWS = 4; 
const byte COLS = 4; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'}, // You can change these values to change what the keys output
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {31, 33, 35, 37}; 
byte colPins[COLS] = {39, 41, 43, 45}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


// Information for the IR motion detector
int calibrationTime = 30;
long unsigned int lowIn;
long unsigned int pause = 50;
boolean lockLow = true;
boolean takeLowTime;
int PIRValue = 0;


void setup() // The setup function, to run once
{
  myservo.attach(6); // Attaches the servo to pin 6
  Serial.begin(9600); // Begins the serial monitor at 9600 Baud
  pinMode(pirPin, INPUT); // Sets the PIR pin to an input
  pinMode(water_sensor, INPUT); // Sets the water sensor pin to an input
  pinMode(alarm, OUTPUT); // Sets the alarm pin to an output
  pinMode(LED, OUTPUT); // Sets the LED pin to an output
}

void loop() // Function to run continuosly
{
  Measure_Humidity(); // Runs the humidity sensor function
  Enter_Password(); // Runs the keypad function
  Photoresistor(); // Runs the photoresistor function
  PIRSensor(); // Runs the IR motion detector function
  Water_Sensor();
}


void activate_alarm() // Function to activate the alarm
{
  digitalWrite(alarm,HIGH);
  delay(100);
  digitalWrite(alarm,LOW);
  delay(100);
}

void lock_door() // Function to lock the door with the servo
{
  myservo.write(125); 
}

void unlock_door() // Function to unlock the door with the servo
{ 
  myservo.write(0);
}

void Measure_Humidity() // Humidity sensor function
{
  /* Comment out the serial printing lines if you want to diplay the keypad presses */
  int chk = DHT.read11(DHT11_PIN); // Reads the humidity and temperature
  Serial.print("Temperature = "); // Prints the temperature
  Serial.println(DHT.temperature);
  Serial.print("Humidity = "); // Prints the humidity
  Serial.println(DHT.humidity);
  delay(1000);
}

void Enter_Password() // Keypad input function
{
  customKey = customKeypad.getKey();
  if (customKey){
    
    Data[data_count] = customKey;  
    Serial.print(Data[data_count]); 
    data_count++; 
    }

  if(data_count == Password_Length-1){

    if(!strcmp(Data, Master)){ // Unlocks the door and prints to the serial monitor if the password is correct
      Serial.print("\nCorrect\n");
      unlock_door();
      delay(9000);
      }
    else{ // If Locks the door and prints to the serial monitor if the password is incorrect
      Serial.print("\nIncorrect\n");
      lock_door();
      delay(1000);
      }
    
    clearData();  
  }
}

void Photoresistor() // Photoresistor function
{
  int sensorValue = analogRead(A1); // Reads the value of the photoresistor
  if(sensorValue <= 60) { // If there is low light around the photoresistor, it turns the LED's on
    digitalWrite(LED, HIGH);
  }else {
    digitalWrite(LED, LOW);
  }
}  

void PIRSensor() // Function for the IR motion detector
{
   if(digitalRead(pirPin) == HIGH) {
      if(lockLow) {
         PIRValue = 1;
         lockLow = false;
         activate_alarm(); // This changes what it does when it senses motion
         delay(10);
      }
      takeLowTime = true;
   }
   if(digitalRead(pirPin) == LOW) {
      if(takeLowTime){
         lowIn = millis();takeLowTime = false;
      }
      if(!lockLow && millis() - lowIn > pause) {
         PIRValue = 0;
         lockLow = true;
         digitalWrite(alarm,LOW); // This changes what it does when it doesn't sense motion
         delay(10);
      }
   }
}

void Water_Sensor() // Water sensor function
{
  if(analogRead(water_sensor) >= 320) {
    activate_alarm(); // If the water is at or above a certain level the alarm goes off
  }else {
    digitalWrite(alarm, LOW);
  }
}

void clearData() // Clears the entered password
{
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
