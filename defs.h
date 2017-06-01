#ifndef _TYPES_H_
#define _TYPES_H_ 1

#define UNDEF 0.0
#define EPS 0.001
#define SEC 1000000
#define SLEEP ESP.deepSleep(10 * SEC)
#define PRINT Serial.print
#define PRINTLN Serial.println
#define DEBUG(name, value) PRINT(##name); PRINT(": "); PRINTLN(##value); PRINTLN("######");
#define LB PRINTLN("######");
#define ONCE(label) static bool __once__##label = false; if (!__once__##label && (__once__##label = true))
#define ONCE_AFTER(label, timeout) static unsigned long __onceafter__##label = millis(); if (__onceafter__##label != 1 && (millis() - __onceafter__##label > timeout) && (__onceafter__##label = 1))
#define EVERY(label, interval) static unsigned long __every__##label = millis(); if ((millis() - __every__##label > interval) && (__every__##label = millis()))

#define len(s) (sizeof(s) / sizeof(char))
#define MSG_HELLO "HELLO"
#define MSG_BYE "BYE"
#define MSG_SERVER_ACK "SVR:"
#define MSG_DATA "D:"
#define MSG_DATA_ACK "DACK:"


/**
 * Struttura per il sistema sensorio
 */
typedef struct {
    uint32_t seqNo;
    uint16_t timestamp;
    float temperature;
    uint8_t humidity;
    uint8_t moisture;
    uint8_t drop;
} Sensing;


/**
 * Decidi se stampare nella stringa 1 o 0 cifre decimali
 */
byte decimalPlaces(float n) {
    return abs(n - round(n)) > EPS ? 1 : 0;
}

#endif
