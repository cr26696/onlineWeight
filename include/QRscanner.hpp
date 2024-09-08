#ifndef __QRSCANNER_HPP__
#define __QRSCANNER_HPP__

#include <Arduino.h>

class QRScanner {
private:
    const char *batchSymbol = "#";
    const char *staffSymbol = "?";

    void processBatchCode(char* buffer);
    void processStaffCode(char* buffer);

public:
    QRScanner();

    void scanQRCode(const String& QR_val);
};

#endif // QRSCANNER_HPP