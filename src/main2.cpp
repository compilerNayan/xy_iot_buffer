#include <cstring>
#include <string>
#include <optional>
#include <StandardDefines.h>   // your typedefs and macros
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"

class WiFiClient {
Private Bool hotspotActive;
Private Int hotspotClients;
Private EventGroupHandle_t wifiEventGroup;
Private StdString ssid;
Private StdString password;
//Private ILogger* logger = nullptr;

Private Static Const Int WIFI_CONNECTED_BIT = BIT0;

Private Static Void EventHandler(VoidPtr arg, esp_event_base_t event_base,
                                 int32_t event_id, VoidPtr event_data) {
    WiFiClient* client = static_cast<WiFiClient*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(client->wifiEventGroup, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START) {
        client->hotspotActive = true;
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        client->hotspotClients++;
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        client->hotspotClients--;
    }
}

Private Void InitNVS() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

Public Explicit WiFiClient() {
    wifiEventGroup = xEventGroupCreate();
    hotspotActive = false;
    hotspotClients = 0;
    InitNVS();
}

Public Bool ConnectWifi(CStdString& ssid, CStdString& password) {
    this->ssid = ssid;
    this->password = password;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &EventHandler,
                                                        this,
                                                        nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &EventHandler,
                                                        this,
                                                        nullptr));

    wifi_config_t wifi_config = {};
    strncpy((Char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid));
    strncpy((Char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    return true;
}

Public Bool IsWiFiConnected() {
    EventBits_t bits = xEventGroupGetBits(wifiEventGroup);
    return (bits & WIFI_CONNECTED_BIT);
}

Public Bool StartHotspot(CStdString& ssid) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &EventHandler,
                                                        this,
                                                        nullptr));

    wifi_config_t wifi_config = {};
    strncpy((Char*)wifi_config.ap.ssid, ssid.c_str(), sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = ssid.length();
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    return true;
}

Public Bool StartSecureHotspot(CStdString& ssid, CStdString& password) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &EventHandler,
                                                        this,
                                                        nullptr));

    wifi_config_t wifi_config = {};
    strncpy((Char*)wifi_config.ap.ssid, ssid.c_str(), sizeof(wifi_config.ap.ssid));
    strncpy((Char*)wifi_config.ap.password, password.c_str(), sizeof(wifi_config.ap.password));
    wifi_config.ap.ssid_len = ssid.length();
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    return true;
}

Public Bool IsHotspotActive() {
    return hotspotActive;
}

Public Bool IsDeviceConnectedToHotspot() {
    return hotspotClients > 0;
}

Public Int GetNoOfDevicesConnectedToHotspot() {
    return hotspotClients;
}

Public optional<StdString> GetWiFiIPAddress() {
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        Char buf[16];
        sprintf(buf, IPSTR, IP2STR(&ip_info.ip));
        return StdString(buf);
    }
    return {};
}

Public optional<StdString> GetHotspotIPAddress() {
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        Char buf[16];
        sprintf(buf, IPSTR, IP2STR(&ip_info.ip));
        return StdString(buf);
    }
    return {};
}
};

#include "esp_log.h"

extern "C" void app_main(void) {
    ESP_LOGI("WiFiClient", "Starting WiFiClient");
   /* WiFiClient client;
    bool connected = client.ConnectWifi("Garfield", "123Madhu$$S");
    if (connected) {
        printf("Connected to WiFi\n");
    } else {
        printf("Failed to connect to WiFi\n");
    } */
}
