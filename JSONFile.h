#ifndef _JSONFILE_H_
#define _JSONFILE_H_ 1


#include "FS.h"


class JSONFile {
    public:

        /**
         *
         */
        void open(const char *filename) {
            SPIFFS.begin();

            _filename = filename;
        }

        /**
         * Aggiungi riga
         */
        void append(String json) {
            File file;

            track();
            file = SPIFFS.open(_filename, "a");
            file.println(json);
            file.close();
            untrack();
        }

        /**
         * Controlla se ci sono ancora righe da scorrere
         */
        bool hasNext() {
            if (!_file)
                _file = SPIFFS.open(_filename, "r");

            return _file.available();
        }

        /**
         * Leggi prossima riga
         */
        String next() {
            return _file.readStringUntil('\n');
        }

    private:
        const char *_filename;
        long _position;
        File _file;

        /**
         * Salva posizione nel file e chiudilo
         */
        void track() {
            _position = _file ? _file.position() : -1;
            _file.close();
        }

        /**
         * Ripristina posizione nel file
         */
        void untrack() {
            if (_position > 0) {
                _file = SPIFFS.open(_filename, "r");
                _file.seek(_position, SeekSet);
            }
        }
};


#endif