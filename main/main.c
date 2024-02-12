#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_system.h>
#include "driver/gpio.h"
#include <nvs_flash.h>
#include "dht.h"
#include "rgb_led.h"
#include "ultrasonic.h"




#define TRG_PIN 7
#define ECH_PIN 6
#define DHT_PIN 0  // Pin connected to DHT11 sensor module
#define DHT_TYPE DHT_TYPE_DHT11  // Assuming DHT11 sensor type



static const char *TAG = "DHT11";


void dht_task(void *pvParameter) {
    float humidity, temperature;
    while (1) {

        esp_err_t ret = dht_read_float_data(DHT_TYPE, DHT_PIN, &humidity, &temperature);
        if (ret == ESP_OK) {
            printf("Temperature: %.1fC, Humidity: %.1f%%\n", temperature, humidity);
            rgb_led_wifi_connected();


        } else {
            printf("Error reading data from DHT sensor: %d\n", ret);
            rgb_led_http_server_started();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void ultrasonic_task (void *pvParameter){
	esp_rom_gpio_pad_select_gpio(TRG_PIN);
	esp_rom_gpio_pad_select_gpio(ECH_PIN);
	ultrasonic_sensor_t ultrasonic = {
    	.trigger_pin = TRG_PIN,
    	.echo_pin = ECH_PIN
    };
	ultrasonic_init(&ultrasonic);

	uint32_t distance;
	while (1){

		esp_err_t err = ultrasonic_measure_cm(&ultrasonic, 400, &distance);
		if (err == ESP_OK){
			ultrasonic_measure_cm(&ultrasonic, 400, &distance);
			printf("Distance to target is: %lu cm\n", distance);
		}
		else {
			printf("Error reading data from Ultrasonic sensor: %d\n", err);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}




void app_main() {




    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "DHT11 Reading Task starting...");
	rgb_led_wifi_app_started();
	vTaskDelay(1000 / portTICK_PERIOD_MS);


    // Initialize DHT11 sensor module
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT_OD;
    io_conf.pin_bit_mask = (1ULL << DHT_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
	rgb_led_http_server_started();
	vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreate(&dht_task, "dht_task", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(&ultrasonic_task, "ULTRASONIC", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);



}
