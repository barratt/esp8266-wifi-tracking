#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

#include "lib/json.h"
#include "config.h"

StaticJsonBuffer<JSON_BUFFER_SIZE>  jsonBuffer;

// Connects to our reportback WiFi network
void connectToWiFi() {
    wifi_promiscuous_enable(0);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.print(WiFi.localIP());
    Serial.println();
}

String formatMac(uint8_t mac[ETH_MAC_LEN]) {
    String prettyMac = "";

    for (int i = 0; i < ETH_MAC_LEN; i++) {
        if (mac[i] < 16) 
            prettyMac = prettyMac + "0" + String(mac[i], HEX);
        else 
            prettyMac = prettyMac + String(mac[i], HEX);
        
        if (i < 5) 
            prettyMac = prettyMac + ":";
    }

  return prettyMac;
}

void sendData(Beacon beacons[], APClient clients[], unsigned int beaconCount, unsigned int clientCount) {
    Serial.printf("Sending up %d beaconds and %d clients", beaconCount, clientCount);
    connectToWiFi();
    
    // Lets prep the JSON String
    jsonBuffer.clear();
    JsonObject& root    = jsonBuffer.createObject();
    root["device"]      = DEVICE_NAME;

    JsonArray& jBeacons = root.createNestedArray("beacons");
    JsonArray& jClients = root.createNestedArray("clients");

    // add Beacons
    for (int i = 0; i < beaconCount; i++) {
        JsonObject& jBeacon = jBeacons.createNestedObject();
        jBeacon["bssid"]    = formatMac(beacons[i].bssid);
        jBeacon["ssid"]     = beacons[i].ssid;
        jBeacon["rssi"]     = beacons[i].rssi;
        jBeacon["channel"]  = beacons[i].channel;
    }

    // Add Clients
    for (int i = 0; i < clientCount; i++) {
        JsonObject& jClient = jClients.createNestedObject();
        jClient["bssid"]    = formatMac(clients[i].bssid);
        jClient["station"]  = formatMac(clients[i].station);
        jClient["ap"]       = formatMac(clients[i].ap);

        jClient["rssi"] = clients[i].rssi;
        jClient["channel"] = clients[i].channel;
    }

    String out;
    root.prettyPrintTo(Serial);
    root.printTo(out);

    Serial.println();

    // Post it.
    HTTPClient http;
    http.begin(POST_PATH);
    http.addHeader("Content-Type", "application/json");
    http.POST(out);
    http.writeToStream(&Serial);
    http.end();

    Serial.println("Sent data!");

    // put it back into scan mode.
	wifi_set_opmode(STATION_MODE);
    wifi_promiscuous_enable(1);
}

void printClient(APClient &client) {
	Serial.printf("CLIENT: ");
	Serial.print(formatMac(client.station));  	 //Mac of device
	Serial.printf(" - ");
	Serial.print(formatMac(client.ap));   		 // Mac of connected AP
	Serial.printf("  % 3d",     client.channel); // Channel connected over
	Serial.printf("   % 4d\r\n", client.rssi);   // Signal strength
}

void printBeacon(struct Beacon &beacon) {
    Serial.printf("  - BEACON [%32s]  ", beacon.ssid);
    Serial.print(formatMac(beacon.bssid));
    Serial.printf("   %2d",   beacon.channel);
    Serial.printf("   %4d\n", beacon.rssi);
}