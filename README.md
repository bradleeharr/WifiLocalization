# Wifi-RSSI-Localization
 ESP32 using WiFi received signal strength indicator, estimating path loss and calculating distance.



# Procedure
Given RSSI values, this code creates a calibration curve to estimate distances in an environment.
The first step is to take several RSSI measurements at known distances. This is indicated to the ESP32 with a serial commmand `D [distance]`

After measuring distance values, the path loss exponent is estimated using a minimum mean-squared error fit at the distances sampled.

From then, the estimated distance will be continuously calculated and relayed over the serial connection.



# Example Calibration Curve
![image](https://github.com/bradleeharr/Wifi-RSSI-Localization/assets/56418392/1f3f3bc3-36aa-49be-9e50-a50c54f34a5f)


