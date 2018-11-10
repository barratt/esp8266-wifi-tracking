#define WIFI_SSID   ""  // WiFi Username
#define WIFI_PASS   ""  // WiFi Password

#define DEVICE_NAME "Unique ESP #0" // The unique device name to report back with.
#define POST_PATH   "http://192.168.0.24:3020/api/report" // The path to submit the data to.

#define START_CHANNEL       1       // Channel to start at
#define STOP_CHANNEL        14      // Channel to stop at
#define TIME_PER_CHANNEL    200     // Time to listen on the channel for clients

#define MAX_BEACON_CLIENTS  60      // Max beacons (AP Points) 
#define MAX_AP_CLIENTS      20      // Max AP clients (Phones, Laptops etc...)
#define JSON_BUFFER_SIZE    8192    // Tha max JSON buffer size. Use: 

// Constants these values aren't likely to change.
#define ETH_MAC_LEN     6   // Defined in ExpressIFs SDK
#define MAX_SSID_LEN    33  // Max length of an SSID