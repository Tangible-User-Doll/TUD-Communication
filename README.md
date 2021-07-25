# Homie- The Tangible User Doll
## Info
This Projekt was created by
Lara Bertram,
Alissa Avtonoshkina and
Michel Schwarz
<br />
<br />
This projekt is based on two ESP8266.
Upload the code from the Send_Values folder to the first ESP to send Data.
Upload the code from the Get_Values folder to the second ESP to get Data.

The send Data Code collects Data from two Gyr-Sensors and sends the data with a get-request to our backend.
The Get Data Code gets the Data from the Backend and visualize the users emotions based on the Gyro movements.

This repository contains two branches.
The master branch contains the code to communicate with thingspeak.
The DockerAPI branch contains the code to communicate with our custom docker backend.