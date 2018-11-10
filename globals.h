#include "./config.h"

struct Beacon 	beacons [MAX_BEACON_CLIENTS];   // Array to hold beacons
struct APClient clients [MAX_AP_CLIENTS];       // Array to hold clients

unsigned int beaconCount = 0;   // Our current discovered beacon count
unsigned int clientCount = 0;   // Our current discovered client count

unsigned int channel = START_CHANNEL; // The current WiFi channel we're scoping out.
unsigned int timeout = 0;             // the time since we last heard something new on this channel.