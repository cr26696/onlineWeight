#include "QRScanner.hpp"

QRScanner::QRScanner() {}

void QRScanner::scanQRCode(const String& QR_val) {
    Serial.println(QR_val);
    float weightVal = Get_Weight();
    Serial.print("get SangYe weight: ");
    Serial.println(weightVal);

    char buffer[QR_val.length() + 1];
    strcpy(buffer, QR_val.c_str());

    if (strstr(buffer, batchSymbol)) { // 判断为批次码
        processBatchCode(buffer);
    } else if (strstr(buffer, staffSymbol)) { // 判断为员工码
        processStaffCode(buffer);
    }
}

void QRScanner::processBatchCode(char* buffer) {
    char* productionBase = strtok(buffer, batchSymbol);
    char* batchNumber = strtok(NULL, batchSymbol);
    char* uniqueCode = strtok(NULL, batchSymbol);

    Serial.println(productionBase);
    Serial.println(batchNumber);
    Serial.println(uniqueCode);
}

void QRScanner::processStaffCode(char* buffer) {
    char* userName = strtok(buffer, staffSymbol);
    char* phoneNumber = strtok(NULL, staffSymbol);

    Serial.println(userName);
    Serial.println(phoneNumber);
}