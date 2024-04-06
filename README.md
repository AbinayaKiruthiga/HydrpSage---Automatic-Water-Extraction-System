Components used are 
  1.ultra sonic sensor
  2.micro controler (ESP8266)
  3.OLED display
  4.Relay
  5.motor

Cloud plate form is thinkspeak
Connections are noted in the program

Working:
the code is automated in sunch a way that the sensor will detect the water level and send the data to micro controler, that will undergo calculation to compare the triggering water level percentage.
Once the water level reaches the water level reaches the limit of 80%(say) micro controler triggers the relay that in sense trigger the motor to On.
The motor will pump the water out from the region and send that to another region via connectivity. When the water level drops below the triggering point
the micro controler will send the signal to turn of the motor.

