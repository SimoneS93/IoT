#ifndef _TYPES_H_
#define _TYPES_H_ 1

#define UNDEF 0.0
#define EPS 0.001

#define len(s) (sizeof(s) / sizeof(char))
#define MSG_SERVER "Server:"
#define MSG_DATA "Data:"


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