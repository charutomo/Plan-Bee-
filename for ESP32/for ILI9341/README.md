# ESP32 Pinout for Plan Bee
<br/>


## Pinout
<h3 align="center">Plan Bee</h3>
<div align="center">
  <a href="https://github.com/charutomo/Plan-Bee-">
    <img src="image/pinout.jpg" alt="pinout" width="786" height="333">
  </a>
</div>
Above shows the pinout of ESP32-WROOM-32D.
<br/>

## List of Pins
Listed below are the current pins occupied for `Plan Bee`: <br/>
<br/>
<details>
	<summary>ILI9341 Display</summary>
|ILI9341 Display	|Type of Pin on ESP32 |
| :-------------:	|:--------:		    |
|MISO			|12	 	          |
|LED			|3v3		          |
|SCK			|32	     		    |
|MOSI			|33			    |
|DC			|26	     		    |
|RESET		|25			    |
|CS			|27			    |
|GND			|GND       		    |
|VCC			|3v3	       	    |
<br/>
This is for the 2.4 inch ILI9341 Display.
</details>

<details>
<summary>SSH1106</summary>
|SSH1106		|Type of Pin on ESP32 	|
|:---------------:|:--------:	 	|
|GND		|GND       		|
|VCC		|3v3	 	      |
|SCL		|22	     		|
|SDA		|21			|
<br/>
This is for the 1.3 inch ILI9341 Display.
</details>

<details>
<summary>SSD1306</summary>
|SSD1306		|Type of Pin on ESP32	    |
|:---------------:	|:--------:	 	    |
|GND		|GND       		    |
|VCC		|3v3	 	          |
|D0		|32	     		    |
|D1		|33			    |
|RESET	|25		     	    |
|DC		|26			    |
|CS		|27			    |
<br/>
This is for the 0.96 inch ILI9341 Display.
</details>

<details>
	<summary>Temperature Sensor DS18B20</summary>
|DS18B20		|Type of Pin on ESP32 |
| :------------:	|:--------:	 |
|VCC			|5V	 	 |
|DQ			|0	       |
|GND			|GND       	 |
<br/>
</details>

<details>
<summary>INA3221 Current and Voltage Sensor</summary>
|INA3221		|Type of Pin on ESP32 |
| :--------------:|:--------:		    |
|VCC			|3v3	 	          |
|GND			|GND       		    |
|SCL			|22	     		    |
|SDA			|21			    |
<br/>
</details>

<details>
<summary>INA226 Current and Voltage Sensor</summary>
|INA226		|Type of Pin on ESP32 |
| :------------:	|:--------:		    |
|VCC			|3v3	 	          |
|GND			|GND       		    |
|SDA			|21			    |
|SCL			|22	     		    |
|ALERT		|25 *intercept	    |
<br/>
</details>


<br/>


