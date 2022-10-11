# Plan-Bee-
**Hand crank Emergency Power Bank**


**Welcome!** :wave:


## Introduction
<h3 align="center">Plan Bee</h3>
<div align="center">
  <a href="https://github.com/charutomo/Plan-Bee-">
    <img src="image/bee.jpg" alt="bee" width="100" height="80">
  </a>
</div>

[Andy](https://github.com/Andypek921), Jia Woei, [Jinghui](https://github.com/jhlow5), 
[Chermaine](https://github.com/chermainegoh) and [Charissa](https://github.com/charutomo) 
are part of Plan Bee and we are making a prototype of an emergency hand crank power bank 
in fulfilment of the course PS5888: Making and Tinkering. In this particular repository, 
it would consists the C & C++ codes of our functions in our power bank
for Arduino Uno and ESP32-WROOM-32D respectively. More Information can be found in our [blog](https://blogs.ntu.edu.sg/ps5888-2022-g11/).

<br>

## Content Of Repository 
Listed below are the current codes for `Plan Bee `: 

### Hyperlinks to the respective folders
Click to view the table of content
<br>
<details>
	<summary>Table of Content</summary>

* [README.MD](https://github.com/charutomo/Plan-Bee-/blob/main/README.md)

* [License](https://github.com/charutomo/Plan-Bee-/blob/main/LICENSE)

* [Arduino Uno](https://github.com/charutomo/Plan-Bee-/blob/main/for%20Arduino)
	* [LED](https://github.com/charutomo/Plan-Bee-/blob/main/for%20Arduino/LED/LED.ino)

	* [Current, Voltage and Power Monitor](https://github.com/charutomo/Plan-Bee-/blob/main/for%20Arduino/INA3221_for_arduino_testing/INA3221_for_arduino_testing.ino)

* [ESP32-WROOM-32D](https://github.com/charutomo/Plan-Bee-/blob/main/for%20ESP32)
	* [for ILI9341](https://github.com/charutomo/Plan-Bee-/blob/main/for%20ESP32/for%20ILI9341)

	* [for SSH1106](https://github.com/charutomo/Plan-Bee-/blob/main/for%20ESP32/for%20SH1106/SH1106)
	
	* [for SSD1306](https://github.com/charutomo/Plan-Bee-/blob/main/for%20ESP32/for%20SSD1306/INA3221_works)

* [Modified Libraries](https://github.com/charutomo/Plan-Bee-/tree/main/modified%20libraries)
	* [Adafruit_ILI9341](https://github.com/charutomo/Plan-Bee-/tree/main/modified%20libraries/Adafruit_ILI9341)

	* [TFT_eSPI](https://github.com/charutomo/Plan-Bee-/tree/main/modified%20libraries/TFT_eSPI)


* [image](https://github.com/charutomo/Plan-Bee-/blob/main/image)


</details>

### Structure of repository

The structure of `Plan Bee ` repository is as such:

```
Plan Bee 
│	README.md
│	LICENSE
│
├───	PCB Design
│	├─── PCB Main V1
│	├─── PCB Main V2
│	└─── PCB Main V3
│
├───	For Arduino
│	├─── INA226 for arduino uno
│	├─── INA3221 for arduino uno
│	├─── LED
│	└─── Voltage and Current sensor using ACS712 and potentiometer
│
├─── For ESP32
│	├─── for ILI9341
│	│    ├─── ILI9341_INA226
│	│    ├─── ILI9341_INA3221
│	│    ├─── graphictestILI9341
│	│    ├─── printextILI9341
│	│    └─── README.md
│	│
│	├─── for SH1106
│	│    ├─── SH1106 (using INA3221 and general use)
│	│    ├─── SH1106_forUsers (using INA3221 and for users)
│	│    └─── README.md
│	│
│	├─── for SSD1306
│	│    └───  INA3221_works
│	│
│	└─── intermediate
│	     ├─── Temperature
│	     └─── switch
│
├─── image
│	├─── Plan Bee Logo (ALIVE).png
│	├─── Plan Bee Logo (DEAD).png
│	├─── bee.jpg
│	└─── pinout.jpg
│
├─── modified libraries
│	├─── Adafruit_ILI9341
│	└─── TFT_eSPI
│

```


<br>

## Libraries
These are libraries imported:

### Current and Voltage Sensor
* [BeastDevices INA3221](https://github.com/beast-devices/Arduino-INA3221)
* [INA226_WE](https://github.com/wollewald/INA226_WE)

### Display Screen
* [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
* [Adafruit ILI9341](https://github.com/adafruit/Adafruit_ILI9341)
* [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
* [ESP32-SH1106-OLED](https://github.com/nhatuan84/esp32-sh1106-oled)
* [Modified Libraries](https://github.com/charutomo/Plan-Bee-/tree/main/modified%20libraries/)

### Datetime
* [NTPClient](https://github.com/barsrb/NTPClient)

### Temperature Sensor
* [DallasTemperature](https://github.com/vlast3k/Arduino-libraries/tree/master/DallasTemperature)

### Encoder
* [Ai Esp32 Rotary Encoder](https://github.com/igorantolic/ai-esp32-rotary-encoder)


<br>

## Components
Currently, we use the following items for the functions: 
|Items			| Model No. 1	| Model No. 2 | Model No. 3|
| :----------------:	|:--------:	 	|:--------:   |:--------:  |
|Current, Voltage Sensor| INA3221	 	| INA226	  | -		   |
|Display Screen		| SSD1306	 	| SSH1106	  | ILI9341	   |
|Temperature Sensor	| DS18B20	 	|	-	  |	-	   |
|Encoder			| HW040	 	|	-	  |	-	   |
|Relay			|SRD-05VDC-SL-C	| 	-	  |	-	   |
|Button			|Tactile Push Button|	-	  |	-	   |

<br>

## Getting started

1. Clone this repository into your working station, key in the following command in CMD(Windows) or Terminal(Mac or Linux). 


For Windows/Mac, to save the repository content into a folder in Desktop: 

```
cd Desktop
```

Followed by 

```
git clone https://github.com/charutomo/Plan-Bee-
```

For Linux, to save the repository content into a folder in Desktop: 

```
$ ls Desktop
```

Followed by 

```
$ git clone https://github.com/charutomo/Plan-Bee-
```

2. Install Arduino IDE(if you haven't) and download the [libraries](#libraries) to have the full features.


3. You can start upload code from the repository to test out.

<br>


## Acknowledgement
Special thanks to Tony and Qi Jie for their guidance throughout the whole project of `Plan Bee`.

<br>

## Further enquiries
Please direct your enquiries to planbee9555@gmail.com or under the issues section.

<br>

Thank you for your support! :blush: