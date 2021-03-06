#include "movementXY.h"
#include "globalVariables.h"
#include "stdio.h"

scanGrid::scanGrid(uint16_t widthX, uint16_t widthY)
: maxX(widthX -1), maxY(widthY -1),currentX(0), currentY(0),currentDirection(0)
{}


scanGrid::scanGrid(uint16_t maxX, uint16_t maxY, uint16_t startX, uint16_t startY)
: maxX(maxX), maxY(maxY),currentX(startX), currentY(startY), currentDirection(0)
{}

bool scanGrid::moveOn(){
    switch(currentDirection){
        case false: 
            if(currentX<maxX){
                currentX++; //rightwise
                currentXDac = gridToDacValue(currentX, this->getMaxX(), DAC_VALUE_MAX, this->getMultiplicatorGridAdc());
                printf("new X-DAC: %d for X: %d",currentXDac, currentX);
            }else{
                if(currentY != maxY){
                    currentY++; //next row
                    currentYDac = gridToDacValue(currentY, this->getMaxY(), DAC_VALUE_MAX, this->getMultiplicatorGridAdc());
                    currentDirection = true; //direction change
                }else{
                    return true; //all points scanned
                }
            }
            //write new DAC values to DACs
            printf("move \n");
            vTaskResume(handleVspiLoop); //will suspend itself
            return false; 
            break;
        case true: 
            if(currentX>0){
                currentX--; //leftwise
                currentXDac = gridToDacValue(currentX, this->getMaxX(), DAC_VALUE_MAX, this->getMultiplicatorGridAdc());
                 printf("new X-DAC: %d for X: %d",currentXDac, currentX);
            }else{
                if(currentY != maxY){   
                    currentY++; //next row
                    currentYDac = gridToDacValue(currentY, this->getMaxY(), DAC_VALUE_MAX, this->getMultiplicatorGridAdc());
                    currentDirection = false; //direction change
                }else{
                    return true; //all points scanned
                }
            }
            //write new DAC values to DACs
            printf("move \n");
            vTaskResume(handleVspiLoop); //will suspend itself
            return false;
            break;
        default:
            printf("error move \n");
            return false;
            
    }
}

uint16_t scanGrid::getCurrentX(){
    return currentX;
}

uint16_t scanGrid::getCurrentY(){
    return currentY;
}

void scanGrid::setMaxX(uint16_t maxX){
    this->maxX = maxX;
}

void scanGrid::setMaxY(uint16_t maxY){
    this->maxY = maxY;
}

void scanGrid::setStartX(uint16_t startX){
    this->currentX = startX;
}

void scanGrid::setStartY(uint16_t startY){
    this->currentY = startY;
}

void scanGrid::setDirection(bool direction){
    this->currentDirection = direction;
}

void scanGrid::setMultiplicatorGridAdc(uint16_t multiplicator){
    this->multiplicatorGridAdc = multiplicator;
}

uint16_t scanGrid::getMaxX(){
    return this->maxX;
}

uint16_t scanGrid::getMaxY(){
    return this->maxY;
}

bool scanGrid::getCurrentDirection(){
    return this->currentDirection;
}

uint16_t scanGrid::getMultiplicatorGridAdc(){
    return this->multiplicatorGridAdc;
}

uint16_t gridToDacValue(uint16_t currentGridValue, uint16_t maxGridValue, uint16_t maxValueDac, uint16_t multiplicator){
    printf("%d %d %d %d",  currentGridValue,  maxGridValue,  maxValueDac,  multiplicator);
    return maxValueDac/2 + (currentGridValue - (maxGridValue/2 + 1))*multiplicator;
}