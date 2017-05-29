#ifndef _LOG_H_
#define _LOG_H_ 1

#include "FS.h"

#define eps 0.0001
#define floor(x) round(-0.5 + eps + (x))
#define ne(x, y) (abs((x) - (y)) > eps)
#define dec(x) (abs((x) - floor(x)))
#define UNDEF -99
#define VERSION "v1"


class Loggr {

    public:

        void open(const char* filename) {
            _filename = filename;
            empty();
        }

        void close() {
            _file.close();
        }

        void empty() {
            _sensing.timestamp = 0;
            _sensing.temperature = UNDEF;
            _sensing.humidity = UNDEF;
            _sensing.moisture = UNDEF;
            _sensing.leaf_drop = UNDEF;
        }

        void setTimestamp(uint16_t timestamp) {
            _sensing.timestamp = timestamp;
        }

        void setTemperature(float temperature) {
            _sensing.temperature = temperature;
        }

        void setHumidity(int8_t humidity) {
            _sensing.humidity = humidity;
        }

        float getTemperature() {
            return _current.temperature;
        }

        int8_t getHumidity() {
            return _current.humidity;
        }

        /**
         * Salva record su disco
         */
        void append() {
            File file = SPIFFS.open(_filename, "a");

            file.write((const uint8_t *) &_sensing, sizeof(_sensing));
            file.close();
        }

        /**
         * Leggi prossimo record
         */
        bool next() {
            if (!_file) {
                _file = SPIFFS.open(_filename, "r");
            }

            if (!_file.available()) {
                _file.close();
                return false;
            }

            _file.read((uint8_t *) &_current, sizeof(_current));

            return true;
        }



    private:
        const char *_filename;
        File _file;
        struct {
            uint16_t timestamp;
            uint16_t seq_no;
            float temperature;
            int8_t humidity;
            int8_t moisture;
            int8_t leaf_drop;
        } _sensing, _current;
};

#endif
