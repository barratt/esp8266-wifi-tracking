#include "ESP8266WiFi.h"
#include "string.h"
#include "stdint.h"
#include "stddef.h"

bool pushClient(struct APClient &client) { 
    bool knownClient = false;

	// Update its last seen if its here.
    for (int i = 0; i < clientCount; i++) {
        // Compare the client in the array to the param
        if (!memcmp(clients[i].station, client.station, ETH_MAC_LEN)) {
            clients[i].lastDiscoveredTime   = millis();
            clients[i].rssi                 = client.rssi;

            knownClient = true;
            return knownClient;
        }
    }

    if (knownClient)
        return knownClient;

    // Its a new client, lets copy over the old one.
    client.lastDiscoveredTime = millis();
    memcpy(&clients[clientCount], &client, sizeof(client));

    // Increment our known clients count
    clientCount++;

    if (clientCount >= sizeof(clients)) {
        Serial.printf("WARNING!!! EXCEEDED MAX CLIENTS!!!\n");
        clientCount--; // Always rewrite the last one in the array.
    }

    return knownClient;
}

bool pushBeacon(struct Beacon &beacon) { 
    bool knownBeacon = false;

    for (int i = 0; i < beaconCount; i++) {
        // Compare the beacon in the array to the param
        if (!memcmp(beacons[i].bssid, beacon.bssid, ETH_MAC_LEN)) {
            beacons[i].lastDiscoveredTime   = millis();
            beacons[i].rssi                 = beacon.rssi;

            knownBeacon = true;
            return knownBeacon;
        }
    }

    if (knownBeacon)
        return knownBeacon;

    // Its a new beacon, lets copy over the old one.
    beacon.lastDiscoveredTime = millis();
    memcpy(&beacons[beaconCount], &beacon, sizeof(beacon));

    // Increment our known beacons count
    beaconCount++;

    if (beaconCount >= sizeof(beacons)) {
        Serial.printf("WARNING!!! EXCEEDED MAX BEACONS!!!\n");
        beaconCount--; // Always rewrite the last one in the array.
    }

    return knownBeacon;
}

void promiscuousCallback(uint8_t *buf, uint16_t length) {
    // If the length is 128 chances are its a beacon or probe.
    if (length == 128) {
        // Cast it to a sniffer128 packet.
        struct sniffer_buf128 *sniffer = (struct sniffer_buf128*) buf;

        // Check if its a beacon frame (see https://wiki.wireshark.org/Wi-Fi)
        if (sniffer->buf[0] == 0x80) {
            struct Beacon beacon = parseBeacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
            
            if (beacon.err)
                return;
            
            if (pushBeacon(beacon)) // Add it and print it if its new
                printBeacon(beacon);
        }
        
        // Listen for probe requests (see https://www.lovemytool.com/blog/2010/02/wireshark-wireless-display-and-capture-filters-samples-by-joke-snelders.html)
        if ((sniffer->buf[0] == 0x40)) {
            struct APClient client = parseClient(sniffer->buf, 36, sniffer->rx_ctrl.rssi);
            if (client.err)
                return;
            
            if (pushClient(client)) // Add it and print it if its new
                printClient(client);
        }

        return;
    } 

    // Look for Data and QoS requests (see https://www.lovemytool.com/blog/2010/02/wireshark-wireless-display-and-capture-filters-samples-by-joke-snelders.html)
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
        struct APClient client = parseDataIntoClient(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);

        if (client.err)
            return;
        
        if (pushClient(client)) // Add it and print it if its new
            printClient(client);
    }
}
