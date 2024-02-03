# Wifi-RSSI-Localization
 ESP32 using WiFi received signal strength indicator, estimating path loss and calculating distance.


## Theory
The received signal strength indicator (RSSI) is a common metric used in wireless communications to establish the reliability of a communication signal. RSSI is simply the ratio of the transmitted power to the received power and is typically displayed in a logarithmic scale of decibels (dB). 

In a perfect vaccuum with a direct line of sight, the power received from a radio transmitter is decreased by a factor of 1/d^2, where d is the distance.


Although the environment in any given location is not a perfect vacuum, similar principles apply toward understanding the distance based on a given environment.

### Log-distance path loss model
The log-distance path loss model is an extension of the 1/d^2 model, allowing for other values of 𝛾 and other reference distances d_0.
Received signal strength is:

𝑅𝑆𝑆𝐼(𝑑)= 𝑅𝑆𝑆𝐼_𝑑0  − 10𝛾 log_10⁡(𝑑/𝑑_0 )



# Procedure
Given RSSI values, this code creates a calibration curve to estimate distances in an environment.
The first step is to take several RSSI measurements at known distances. This is indicated to the ESP32 with a serial commmand `D [distance]`

After measuring distance values, the path loss exponent is estimated using a minimum mean-squared error fit at the distances sampled.

From then, the estimated distance will be continuously calculated and relayed over the serial connection.



# Example Calibration Curve
![image](https://github.com/bradleeharr/Wifi-RSSI-Localization/assets/56418392/1f3f3bc3-36aa-49be-9e50-a50c54f34a5f)


