#include "stdint.h"
#include "../config.h"

struct Beacon {
	uint8_t bssid[ETH_MAC_LEN];
	uint8_t ssid[MAX_SSID_LEN];
	unsigned int ssidLen;
	unsigned int channel;

	signed rssi;

	uint8_t capa[2]; // Figure out
	long lastDiscoveredTime;

	bool err;
};

struct Beacon parseBeacon(uint8_t *frame, uint16_t framelen, signed rssi) {
    struct Beacon beacon;

    beacon.ssidLen = 0;
    beacon.channel = 0;
    beacon.err = false;
    beacon.rssi = rssi;

    int position = 36;

    // If the first byte isn't 00 we don't regonise it.
    if (frame[position] != 0x00) {
        beacon.err = true;
        return beacon;
    }

    while (position < framelen) {

        if (frame[position] == 0x00) { // SSID
            beacon.ssidLen = (int) frame[position + 1]; // Get the length of the SSID

            if (beacon.ssidLen < 0) {
                beacon.err = true;
                return beacon;
            }
            
            if (beacon.ssidLen > (MAX_SSID_LEN -1)) {
                beacon.err = true;
                return beacon;
            }

            if (beacon.ssidLen == 0) { // If its 0 then fine empty SSID
                memset(beacon.ssid, '\x00', MAX_SSID_LEN);
            } else {
                memset(beacon.ssid, '\x00', MAX_SSID_LEN);
                memcpy(beacon.ssid, frame + position + 2, beacon.ssidLen);
            }
        }

        if (frame[position] == 0x03) { // Channel #
            beacon.channel = (int) frame[position + 2];
            break;
        }

        position += (int) frame[position + 1] + 2;
    }

    beacon.capa[0] = frame[34];
    beacon.capa[1] = frame[35];

    memcpy(beacon.bssid, frame + 10, ETH_MAC_LEN);

    return beacon;
}