#ifndef _SENSOR_H_
#define _SENSOR_H_ 1

#include "types.h"


/**
 * Interfaccia per i sensori
 */
class Sensor {
    public:
        /**
         * Leggi dato, indipendentemente da quale esso sia
         */
        virtual float read() {
            return UNDEF;
        }
};

/**
 *
 */
class FakeRandomSensor : public Sensor {
    public:
        float read() {
            return analogRead(A0);
        }
};


/**
 * Sistema sensorio, con letture dinamiche
 */
class SensorArray {
    public:

        /**
         *
         */
        void push(const char* name, float reading) {
            append(name, String(reading, 1));
        }

        /**
         * Converti letture in JSON
         */
        String toJSON(uint32_t seqNo, uint32_t timestamp) {
            append("seqNo", String(seqNo));
            append("ts", String(timestamp));

            // append aggiunge sempre la virgola PRIMA
            // rimuovi quella del primo valore
            _buffer.setCharAt(0, '{');
            _buffer += " }";

            return _buffer;
        }

    private:
        String _buffer;

        void append(const char* key, String value) {
            _buffer += " ,\"";
            _buffer += key;
            _buffer += "\":";
            _buffer += value;
        }

};


#endif