# Bikecompanion
Companion device to collect user info and stats on a bike ride
 
As of now uses a esp32 to collect data and send it to a flask webserver on an aws instance
 
Sensors Used:
  -Hall Sensor 
    -Detects revolutions
    -Used to determine speed and disatnce traveled
  -Max30102
    -Heart rate and blood spo2 monitor
  -LSM6DSO Accelerometer
    -Acceleration and gyro data
    -Temperature
