#include "driver/i2c.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char *wifi_ssid = "PODA_8669";
const char *wifi_password = "318545318545";
#define MQTT_SEND_DATA_TOPIC "/lightsense-xproko47/data"
#define MQTT_GET_threshold_TOPIC "/lightsense-xproko47/set_threshold"
esp_mqtt_client_handle_t client;
int ledc_threshold;
int has_ip_from_wifi = 0; // flag for checking if we got ip from wifi and can send mqtt messages

#define STORAGE_NAMESPACE "storage"

static const char *TAG = "IMP light detection";

// LED related defs
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (2) // Define the output GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_MAX (8192)            // Set duty to 100%. (2 ** 13) * 100% = 8192
#define LEDC_FREQUENCY (4000)           // Frequency in Hertz. Set frequency at 4 kHz
#define LEDC_DEFAULT_THRESHOLD (100)

// light detector defs
#define DATA_LENGTH 512                 /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 128              /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_CHECKING_SENSOR 500 // delay between taking data from sensor. 120ms is the minimum

#define I2C_MASTER_SCL_IO 22        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM 0            /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */

#define BH1750_SENSOR_ADDR 0x23    // “0100011“
#define BH1750_CMD_START 0x10      // Continuously H-Resolution Mode
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

/**
 * @brief Configure the LEDC peripheral
 */
static void ledc_init() {
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_MODE,
                                      .duty_resolution = LEDC_DUTY_RES,
                                      .timer_num = LEDC_TIMER,
                                      .freq_hz = LEDC_FREQUENCY,
                                      .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {.speed_mode = LEDC_MODE,
                                          .channel = LEDC_CHANNEL,
                                          .timer_sel = LEDC_TIMER,
                                          .intr_type = LEDC_INTR_DISABLE,
                                          .gpio_num = LEDC_OUTPUT_IO,
                                          .duty = 0,
                                          .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

int retry_num = 0;
/**
 * @brief Event handler for catching Wi-Fi events.
 *
 */
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        printf("WIFI CONNECTING....\n");
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("WiFi CONNECTED\n");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("WiFi lost connection\n");
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        printf("Wifi got IP...\n\n");
        has_ip_from_wifi = 1;
    }
}

/**
 * @brief Connect to Wi-Fi
 */
void wifi_connection() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); //
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {.sta =
                                            {
                                                .ssid = "",
                                                .password = "",

                                            }

    };
    strcpy((char *)wifi_configuration.sta.ssid, wifi_ssid);
    strcpy((char *)wifi_configuration.sta.password, wifi_password);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_connect();
}

/**
 * @brief Set the LEDC brightness
 *
 * @param lux - the lux value from the sensor
 */
void set_led_brightess(double lux) {
    // lux == 0 -> max light
    // lux >= threshold -> no light
    if (lux > ledc_threshold) {
        lux = ledc_threshold;
    }
    static int last_duty_num = 0;

    // how much of the 'max light' should be used
    double lux_percent = 1 - (lux / ledc_threshold);
    int duty_num = LEDC_DUTY_MAX * lux_percent;

    if (last_duty_num > duty_num) {
        for (int i = last_duty_num; i > duty_num; i--) {
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            vTaskDelay(9 / portTICK_PERIOD_MS);
        }
    } else if (last_duty_num < duty_num) {
        for (int i = last_duty_num; i < duty_num; i++) {
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            vTaskDelay(9 / portTICK_PERIOD_MS);
        }
    }
    last_duty_num = duty_num;
}

/**
 * @brief Get the lux value from the sensor
 */
static esp_err_t i2c_master_sensor(i2c_port_t i2c_num, uint8_t *data_h, uint8_t *data_l) {
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BH1750_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, BH1750_CMD_START, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }
    vTaskDelay(30 / portTICK_PERIOD_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, BH1750_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data_h, ACK_VAL);
    i2c_master_read_byte(cmd, data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init() {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief Get the data from the sensor
 */
int get_data_from_sensor() {
    int ret;
    uint8_t sensor_data_h, sensor_data_l;
    ret = i2c_master_sensor(I2C_MASTER_NUM, &sensor_data_h, &sensor_data_l);
    if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "I2C Timeout");
        return -1;
    } else if (ret == ESP_OK) {
        return (sensor_data_h << 8 | sensor_data_l) / 1.2;
    }
    return -1;
}

/**
 * @brief Initialize the NVS
 */
void initialize_nvs() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

/**
 * @brief Read the value from the NVS or return the default value if not found
 *
 * @return int - the value(threshold) from the NVS
 */
int read_threshold_from_nvs() {
    nvs_handle_t my_handle;
    int32_t value = LEDC_DEFAULT_THRESHOLD;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        // Read the value or use the default
        err = nvs_get_i32(my_handle, "user_value", &value);
        switch (err) {
            case ESP_OK:
                printf("User's custom threshold = %ld\n", value);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The user hasn't set any custom threshold.\n");
                break;
            default:
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }

    return value;
}

/**
 * @brief Write the value to the NVS
 *
 * @param value - the value(threshold) to be written to the NVS
 */
void write_threshold_from_nvs(int value) {
    nvs_handle_t my_handle;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        // Write the value
        err = nvs_set_i32(my_handle, "user_value", value);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit the changes
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Commit Failed!\n" : "Commit Done\n");

        nvs_close(my_handle);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = 0;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, MQTT_SEND_DATA_TOPIC, "data_3", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, MQTT_GET_threshold_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            // got data on a subscribed topic
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            char data[20];
            sprintf(data, "%.*s", event->data_len, event->data);
            int tmp = atoi(data);
            if(tmp > 0){
                ledc_threshold = tmp;
                write_threshold_from_nvs(ledc_threshold);
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                // log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                // log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                // log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

/**
 * @brief Start the MQTT client
 */
static void mqtt_app_start() {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.emqx.io",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main() {
    ledc_init();
    initialize_nvs();
    wifi_connection();
    while (!has_ip_from_wifi) {
        vTaskDelay(100 / portTICK_PERIOD_MS); // wait some time to get ip from wifi
    }

    ledc_threshold = read_threshold_from_nvs();

    ESP_ERROR_CHECK(i2c_master_init());
    mqtt_app_start();

    char msg[10];
    while (1) {
        double lux = get_data_from_sensor();
        if (lux == -1) {
            vTaskDelay((DELAY_TIME_BETWEEN_CHECKING_SENSOR) / portTICK_PERIOD_MS);
            continue;
        }
        sprintf(msg, "%lf", lux);
        esp_mqtt_client_publish(client, MQTT_SEND_DATA_TOPIC, msg, 0, 1, 0);
        set_led_brightess(lux);

        vTaskDelay((DELAY_TIME_BETWEEN_CHECKING_SENSOR) / portTICK_PERIOD_MS);
    }
}
