#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h" 
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "soc/rtc_periph.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include "globalVariables.h"
#include "spi.h"
#include "adc.h"
#include "controller.h"
#include "dataStoring.h"
#include "timer.h"


//Main application
extern "C" void app_main(void)
{
    //init and start HSPI connection to Raspberry Pi
    printf("bootup");
    hspiStart();
    //xTaskCreate(&null_task,"Printing VOID Logs ",2048 , NULL , 3, &handleTask);
    //timer_tg0_initialise(1000000);

    //esp_err_t i2cInitErr = i2cInit();
    //printf("%s \n", (char*) &i2cInitErr);

    remainingTunnelCurrentDifferencenA = 0.1;
    destinationTunnelCurrentnA = 20.0;
    kP = 100;
    kI = 50;

    
    //controllerStart();

    //long count = 1; 
    //vTaskSuspend(NULL); 
    while(1) {
        //printf("main \n");
        //i2cInit();
        //uint16_t temp = readAdc();
        //count++;
    }

}


