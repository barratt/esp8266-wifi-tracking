#include "stdint.h"
#include "../config.h"

struct APClient {
	uint8_t bssid	[ETH_MAC_LEN];
	uint8_t station	[ETH_MAC_LEN];
	uint8_t ap		[ETH_MAC_LEN];

	unsigned int channel;
	signed rssi;
	
	long lastDiscoveredTime;
	bool err;
};

struct APClient parseClient(uint8_t *frame, uint16_t framelen, signed rssi)
{
	struct APClient client;

	client.channel 		= -1;
	client.err 			= false;
	client.rssi 		= rssi;

	struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) frame;

	memset(client.bssid,   0xFF,       ETH_MAC_LEN);
	memcpy(client.station, frame + 10, ETH_MAC_LEN);
	
	return client;
}

struct APClient parseDataIntoClient(uint8_t *frame, uint16_t framelen, signed rssi, unsigned channel) {
	struct APClient client;

	client.channel 	= channel;
	client.rssi 	= rssi;
	client.err  	= false;

	int pos = 36;
	uint8_t *bssid;
	uint8_t *station;
	uint8_t *ap;
	uint8_t ds;

	// See https://dalewifisec.wordpress.com/2014/05/17/the-to-ds-and-from-ds-fields/

	ds = frame[1] & 3;	// Set first 6 bits to 0
	switch (ds) {
		case 0: // XXXX-XX00 = NoDS packet p[4]=DST p[10]=SRC p[16]=BSS
			ap 		= frame + 4;
			bssid 	= frame + 16;
			station = frame + 10;
			break;
		case 1:	// XXXX-XX01 = ToDS packet p[4]=BSS p[10]=SRC p[16]=DST
			ap 		= frame + 16;
			bssid 	= frame + 4;
			station = frame + 10;
			break;
		case 2: // XXXX-XX10 = FromDS packet p[4]=DST p[10]=BSS p[16]=SRC
			ap 		= frame + 16;
			bssid 	= frame + 10;
			station = frame + 4;
			if ( // Compare DST to the common broadcast patterns if they match then ap and station are actually switched.
				   memcmp(frame + 4, broadcast1, 3) 
				|| memcmp(frame + 4, broadcast2, 3) 
				|| memcmp(frame + 4, broadcast3, 3)
			) {
				station = frame + 16;
				ap = frame + 4;
			}
			break;
		case 3: // XXXX XX11 = WDS packet p[4]=RCV p[10]=TRM p[16]=DST p[26]=SRC
			ap 		= frame + 4;
			bssid 	= frame + 10;
			station = frame + 4;
			break;
	}

	memcpy(client.ap, 		ap, 		ETH_MAC_LEN);
	memcpy(client.bssid, 	bssid, 		ETH_MAC_LEN);
	memcpy(client.station,  station, 	ETH_MAC_LEN);

	return client;
}