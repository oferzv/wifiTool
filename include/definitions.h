//enum WiFiMode can be:WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA
//* these two pseudo modes are experimental:WIFI_SHUTDOWN = 4, WIFI_RESUME = 8
#define DEF_WIFI_MODE       WIFI_AP_STA

#define WAIT_FOR_WIFI_TIME_OUT 6000UL

#define SECRETS_PATH        "/secrets.json"

//Set the accespoint ip
#define DEF_AP_IP           192,168,4,1

//Set the default gateway ip
#define DEF_GATEWAY_IP      192,168,4,1

//Set the subnet mask
#define DEF_SUBNETMASK      255,255,255,0

//SoftAP name
#define DEF_AP_NAME         "esp_AP"

//DNS port
#define DEF_DNS_PORT        53

//DNS IP
#define DEF_DNS_IP          192,168,4,1