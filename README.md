

# Wifi-RSSI-Localization
 ESP32 using WiFi received signal strength indicator, estimating path loss and calculating distance.

## Theory
The received signal strength indicator (RSSI) is a common metric used in wireless communications to establish the reliability of a communication signal. 

The RSSI is important because it tells you how strong the signal is. If you have a stronger signal, you will be able to send and receive data faster and more reliably. 




RSSI is simply the ratio of the transmitted power to the received power and is typically displayed in a logarithmic scale of decibels (dB). 
In a perfect vaccuum with a direct line of sight, the power received from a radio transmitter is decreased by a factor of 1/d^2, where d is the distance. Although the environment in any given location is not a perfect vacuum, similar principles apply toward understanding the distance based on a given environment.

### Log-distance path loss model
The log-distance path loss model is an extension of the 1/d^2 model, allowing for other values of 𝛾 (in free-space 𝛾 = 2, because of 1/d^2) and other reference distances d_0.
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



# Example Calibration Curve

<p align="center"><img src="https://github.com/bradleeharr/Wifi-RSSI-Localization/assets/56418392/1f3f3bc3-36aa-49be-9e50-a50c54f34a5f" style="width:400px"> </p>
This is an example curve of the distribution of distances against the RSSI generated from walking continuously in an outdoor field. The path loss exponent, 2.28, is close to 2, meaning that it is close to the ideal path loss exponent of 2 (falling off as a function of the distance as 1/d^2)


