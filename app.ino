#include "ESP8266WiFi.h" // ESP Library.
#include "./structs.h"   // Useful structs used throughout the program.
#include "./config.h"	 // Configuration, modify this.
#include "./globals.h"   // This file holds the array of clients, beacons channel and timeout 
#include "./utils.h"	 // Useful functions for use throughout the program.
#include "./parser.h"	 // The meat of the code. Parses incoming packets.

void setup() {
	Serial.begin(115200);
	Serial.println();
	Serial.println();
	Serial.println("ESP-WiFi based location tracking");
	Serial.printf("SDK version: %s \n", system_get_sdk_version());
	Serial.println();
	Serial.println();

	// Setup promiscuous mode.
	wifi_set_opmode(STATION_MODE);
	wifi_set_channel(channel);
	wifi_promiscuous_enable(0);
	wifi_set_promiscuous_rx_cb(promiscuousCallback);
	wifi_promiscuous_enable(1);
}

void loop() {
	Serial.println("Scan start");

	beaconCount = 0;
	clientCount = 0;
	channel 	= START_CHANNEL;

	while (channel <= STOP_CHANNEL) {
		Serial.printf("\nSetting scan channel to %d\n", channel);

		wifi_set_channel(channel);
		timeout = 0;

		while (timeout < TIME_PER_CHANNEL) {
    		delay(1); // Give ExpressIF NONO SDK time to fire the promiscuous CB
			timeout++;
		}

		channel++;
	}

	Serial.println("Scan end, sending results");
	sendData(beacons, clients, beaconCount, clientCount);
}