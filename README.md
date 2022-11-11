# ESP_fan_controller
A simple esp fan controller I created for an HP DL380p G8, it replaces the PWM signals from the motherboard

WARNING! ESP8266 may need a pull-up resistor on the pin used to PWM depending on the model.
Upgrading the heatsink on the RAID controller is advised as it tends to be the hottest running component (also stock heatsink is laughable).

How does it work? Fans are usually controlled by 6 individual PWM signals, in order to take control of them, we can cut all the connectors and jump tach to ground (yellow and black cables in my case), this is necessary to avoid fan errors from iLO. The 3 remaining connectors should be +12V, GND and PWM (blue in the 380p G8).
I connected all fans and PWM wires in parallel and then hooked the signal up to pin 2 (D4) on my wemos d1 mini.
Power for the fans can be taken from the 12V connector on the HDD backplane, directly from the motherboard, or any other high-current capable 12V power plane.
