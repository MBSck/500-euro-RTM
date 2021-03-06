#include <stdint.h>

#include "driver/timer.h"

#include "globalVariables.h"
#include "controller.h"
#include "adc.h"
#include "spi.h"
#include "dataStoring.h"
#include "timer.h"

void controllerStart(){

    esp_err_t errTemp = i2cInit(); //Init ADC
    printf("I2C Init returncode: %d\n", errTemp);
    vspiStart(); //Init and loop for DACs
    //xTaskCreatePinnedToCore(sendDatasets, "sendDatasets", 10000, NULL, 3, &handleSendDatasets, 0);
    //timer_tg0_initialise(1000000); //ns -> 
    xTaskCreatePinnedToCore(controllerLoop,"controllerLoop", 10000, NULL, 2, &handleControllerLoop, 1);
    timer_tg0_initialise(1200); //us -> 10^6us/860SPS = 1162 -> 1200 
}

void controllerLoop(void* unused){
    static double e,w,r,y,eOld,yOld = 0;
    uint16_t ySaturate = 0;
    w = destinationTunnelCurrentnA;
    uint16_t unsentDatasets = 0;
    //TickType_t xLastWakeTime = xTaskGetTickCount(); //set current for vTaskDelayUntil
    
    //TickType_t xFrequency = 10 / portTICK_PERIOD_MS; //1ms
    //timer_start(TIMER_GROUP_0, TIMER_0);
    while(1){
        //vTaskDelayUntil( &xLastWakeTime, xFrequency );
        vTaskSuspend(NULL);
        //printf("controllerLoop");
        uint16_t adcValue = readAdc(); //read current voltageoutput of preamplifier and saturate
        //printf("%d\n", adcValue);
        //vTaskDelay(10);
        currentTunnelCurrentnA = (7.8* adcValue * ADC_VOLTAGE_MAX * 1e3) / (ADC_VALUE_MAX * RESISTOR_PREAMP_MOHM); //max value 20.48 with preAmpResistor = 100MOhm and 2048mV max voltage
        r = currentTunnelCurrentnA; //conversion from voltage to equivalent current
        
        
        e = w - r; //regeldifferenz = fuehrungsgroesse - rueckfuehrgroesse
        //printf("e: %f, remaining: %f \n", e, remainingTunnelCurrentDifferencenA);
        if(std::abs(e) <= remainingTunnelCurrentDifferencenA){
            //save to queue
            dataQueue.emplace(dataElement(rtmGrid.getCurrentX(), rtmGrid.getCurrentY(), currentZDac)); //add dateElememt to queue
            unsentDatasets++; //increment
            if(unsentDatasets >= sendDataAfterXDatasets){
                //printf("enough datasets to send \n");
                timer_pause(TIMER_GROUP_0, TIMER_0); //pause timer during dataset sending
                unsentDatasets = 0;
                dataReady = true;
                
                vTaskResume(handleHspiLoop); //sends datasets to raspberry pi, will resume after task for sending suspends itself
                vTaskSuspend(NULL);
                unsentDatasets = 0;
                timer_start(TIMER_GROUP_0, TIMER_0); //resume timer           
            }else
            {
                //printf("Not enough datasets to send %d\n", unsentDatasets);
            }
            
            //move tip
            if(rtmGrid.moveOn()) //moveTip
            {
                 timer_pause(TIMER_GROUP_0, TIMER_0); //pause timer, will be resumed at next new scan 
                //all points scanned, end controllerLoop
                if(!dataQueue.empty()){
                    printf("Last datasets to send \n");
                    vTaskResume(handleHspiLoop); //sends datasets to raspberry pi, will resume after task for sending suspends itself
                }else{
                    printf("All points scanned, already sent all datasets.\n");
                }
                vTaskDelete(NULL);
            }   
        }else{
            y = kP*e + kI*eOld + yOld; //stellgroesse = kP*regeldifferenz + kI* regeldifferenz_alt + stellgroesse_alt
            
            eOld = e;
            ySaturate = saturate16bit((int32_t) y, 0, DAC_VALUE_MAX); //set to boundaries of DAC
            currentZDac = ySaturate; //set new z height
            vTaskResume(handleVspiLoop); //will suspend itself
        }
        yOld = ySaturate;
    }

} 

uint16_t saturate16bit(int32_t input, int32_t min, int32_t max){
    if(input < min){
        //printf("min saturate %d", input);
        return min; 
    }
    if(input > max){
        //printf("max saturate %d", input);
        return max;
    }
    //printf("no saturate: %d", (uint16_t) input);
    return (uint16_t) input; 
}
