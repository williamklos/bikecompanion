#include <Arduino.h>
#include "SparkFunLSM6DSO.h"
#include "MAX30105.h"
#include "heartRate.h"
#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include "spo2_algorithm.h"
#include <ArduinoJson.h>

MAX30105 particleSensor;
LSM6DSO myIMU;

int hallSensorPin = 33;
int hallSensorValue = 0;

int start_time=0;

//bike stats
const int circumfrance=17;
int totalDistance=0;
float speed=6.5;

int counter=0;

float heartRate=0;
float gyroX=0;
float gyroY=0;
float gyroZ=0;
float accelX=0;
float accelY=0;
float accelZ=0;
float temperature=0;

int revolutions=0;


long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;



//Wifi setup
char ssid[] = "";    // your network SSID (name) 
char pass[] = ""; // your network password (use for WPA, or use as key for WEP)

String kHostname= ""; //host url
int kport=; // port of host
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

int flag=0;
int waitTime=0;

void setup()
{
  Serial.begin(9600); // initialize serial communication at 115200 bits per second:
  
  //Accelerometer
  Wire.begin();
  delay(10);

  //accellerometer
  if( myIMU.begin(0x6B) )
    Serial.println("Ready.");
  else { 
    Serial.println("Could not connect to IMU.");
    Serial.println("Freezing");
  }

  if( myIMU.initialize(BASIC_SETTINGS) )
    Serial.println("Loaded Settings.");
  
  //Hall sensor
  pinMode(hallSensorPin,INPUT);


  //Heart 
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  //Wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  
  start_time=millis();
 
}

void loop()
{   
    float temp=millis()/1000.0;
    int temp2 =temp;

    hallSensorValue = digitalRead(hallSensorPin);
    totalDistance=counter*circumfrance;
    
    accelX=myIMU.readFloatAccelX();
    accelY=myIMU.readFloatAccelY();
    accelZ=myIMU.readFloatAccelZ();
    gyroX=myIMU.readFloatGyroX();
    gyroY=myIMU.readFloatGyroY();
    gyroZ=myIMU.readFloatGyroZ();
    temperature=myIMU.readTempF();


    if(millis()-start_time>=1000)
    {
      heartRate=beatsPerMinute;
      speed=revolutions;
      revolutions=0;
      start_time=millis();

      if (WiFi.status() == WL_CONNECTED){
        StaticJsonDocument<300> data;
        data["speed"]=speed;
        data["distance"]=totalDistance;
        data["heartRate"]=heartRate;
        data["temperature"]=temperature;
        JsonObject gyro  = data.createNestedObject("gyro");
        gyro["x"]=gyroX;
        gyro["y"]=gyroY;
        gyro["z"]=gyroZ;
        JsonObject accel  = data.createNestedObject("accellertion");
        accel["x"]=accelX;
        accel["y"]=accelY;
        accel["x"]=accelZ;
        data["counter"]=counter;
        WiFiClient c;
        HTTPClient http;
        
        String uri="/jsonexample";
        http.begin(c,kHostname,kport,uri);      //Specify request destination
        http.addHeader("Content-Type", "application/json");
        String request;
        serializeJson(data,request);
        int httpCode = http.POST(request);   //Send the request
        String payload = http.getString();                                        //Get the response payload
 
        Serial.println(httpCode);   //Print HTTP return code
        Serial.println(payload);    //Print request response payload
 
        http.end();  //Close connection
        
      }
    }
    
    //speed
    if(!hallSensorValue && flag==0){
      
      waitTime=millis()+250;
      flag=1;
      revolutions++;
      counter++;
    }
    if(!hallSensorValue && flag==1){
      /*int time_elapsed=millis()-start_time;
      Serial.println(time_elapsed/1000);
      float elapsed=time_elapsed/1000;
      speed=(float)circumfrance/elapsed;
      start_time=millis();*/
      
    }
    if(hallSensorValue && millis()>=waitTime)
    {
      flag=0;
    }

    //heart rate
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue) == true)
    {
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);
    }

    /*if( temp2 % 4==0)
    {
      Serial.print("\nAccelerometer:\n");
      Serial.print(" X = ");
      
      Serial.println(accelX, 3);
      Serial.print(" Y = ");
      Serial.println(accelY, 3);
      Serial.print(" Z = ");
      Serial.println(accelZ, 3);

      Serial.print("\nGyroscope:\n");
      Serial.print(" X = ");
      Serial.println(gyroX, 3);
      Serial.print(" Y = ");
      Serial.println(gyroY, 3);
      Serial.print(" Z = ");
      Serial.println(gyroZ, 3);

      Serial.print("\nThermometer:\n");
      Serial.print(" Degrees F = ");
      Serial.println(temperature, 3);
      Serial.println();
    }*/
    
  }

    
    
  

  
  
  
