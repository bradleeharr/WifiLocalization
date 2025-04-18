

# Wifi-RSSI-Localization
 ESP32 using WiFi received signal strength indicator, estimating path loss and calculating distance.

## Theory
The received signal strength indicator (RSSI) is a common metric used in wireless communications to establish the reliability of a communication signal. 

The RSSI is important because it tells you how strong the signal is. If you have a stronger signal, you will be able to send and receive data faster and more reliably. 



* RSSI is simply the ratio of the transmitted power to the received power and is typically displayed in a logarithmic scale of decibels relative to a miliWatt (dBm). 
* Decibels convert to actual power quantities like so:
<p align="center">
<img src="https://github.com/user-attachments/assets/5e2f2d3d-223c-4f19-9b8b-18b023a00218" style="width: 400px;">
<img src="https://github.com/user-attachments/assets/4ab82c45-f0ed-41b9-b0e0-70e26f698d8e" style="width: 400px;">
</p>



* In a perfect vaccuum with a direct line of sight, the power received from a radio transmitter is decreased by a factor of 1/d^2, where d is the distance. 
* Although the environment in any given location is not a perfect vacuum, similar principles apply toward understanding the distance based on a given environment. 

* The relationship is generally referred to as wireless shadowing, or [fading](https://en.wikipedia.org/wiki/Fading), because the signal appears to fade when graphed.

### Log-distance path loss model
The [log-distance path loss model](https://en.wikipedia.org/wiki/Log-distance_path_loss_model) is an extension of the 1/d^2 scaling that happens in a perfect vacuum, allowing for other values of 𝛾 (in free-space 𝛾 = 2, because of 1/d^2) and other reference distances d_0.
Received signal strength is:

<p align="center">
 <img src="https://github.com/user-attachments/assets/20237350-4352-4025-bd9f-b7626e1410c8" alt="RSSI(𝑑)= RSSI_𝑑0  − 10𝛾 log_10⁡(𝑑/𝑑_0)">
</p>


# Procedure
With the ESP32 Arduino libraries, it's easy to get the RSSI from the WiFi module, like so:
```cpp
#include <WiFi.h>

const char *ssid = "Galaxy A13"; // Include the ssid for your WiFi network
const char *password = "";       // Include the password for your WiFi network

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  Serial.print(ssid);
  
  int timeout_counter = 0;
  while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(200);
        timeout_counter++;
        if(timeout_counter >= CONNECTION_TIMEOUT*5){
        ESP.restart();
        }
    }
  Serial.println(WiFi.localIP());
}
WiFi.RSSI();
```

After measuring the RSSI values, this code creates a calibration curve to estimate distances in an environment.

The first step is to take several RSSI measurements at known distances. This is indicated to the ESP32 with a serial commmand `D [distance]`

After measuring distance values, the path loss exponent is estimated using a minimum mean-squared error fit at the distances sampled.

From then, the estimated distance will be continuously calculated and relayed over the serial connection.


# Usage

1. Replace your WiFi credentials in the `ssid` and `password` credentials to those of a WiFi hotspot.

2. Build the code using the Arduino editor and place it onto a device.

    * This project was built for ESP 32. Instructions for loading code can be found [here](https://randomnerdtutorials.com/getting-started-with-esp32/)
  
4. In your editor on a laptop or device connect a serial connection (in Arduino IDE, PuTTY, etc.) to the ESP32

5. If successfully connected to a WiFi hotpost, your serial should show "Setup Complete"

6. Move the ESP32 some distance _away_ from your WiFi hotspot. Then, in the terminal, Enter 'd X', and it will use that measurememnt to calibrate at distance X feet 

7. Try to get some varied measurements and clear measurements in different areas. After the fourth distance input, you will start receiving distance estimates baseed on your calibration.
   
       
# Example Calibration Curve

<p align="center"><img src="https://github.com/bradleeharr/Wifi-RSSI-Localization/assets/56418392/1f3f3bc3-36aa-49be-9e50-a50c54f34a5f" style="width:400px"> </p>
This is an example curve of the distribution of distances against the RSSI generated from walking continuously in an outdoor field. The path loss exponent, 2.28, is close to 2, meaning that it is close to the ideal path loss exponent of 2 (falling off as a function of the distance as 1/d^2)


