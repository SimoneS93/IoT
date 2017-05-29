#ifndef _METADATA_H_
#define _METADATA_H_ 1

#include "FS.h"

#define DIM 200


/**
 * Tieni traccia dei metadati del progetto
 * in maniera persistente
 */
class Metadata {

    public:

        void open(const char* filename) {
            _filename = filename;
            SPIFFS.begin();
            read();
        }

        void close() {
            write();
        }

        uint32_t incrementSeqNo() {
            _metadata.seq_no = getSeqNo() + 1;

            return getSeqNo();
        }

        uint32_t getSeqNo() {
            return _metadata.seq_no;
        }

        uint32_t getServer() {
            return _metadata.server;
        }

    private:
        const char *_filename;
        struct {
            uint32_t seq_no;
            uint32_t server;
        } _metadata;

        /**
         * Leggi metadati dal disco
         */
        void read() {
            File file = SPIFFS.open(_filename, "r");

            if (file) {
                file.read((uint8_t *) &_metadata, sizeof(_metadata));
                file.close();
            }
            else {
                _metadata.seq_no = 0;
                _metadata.server = 0;
            }
        }

        /**
         * Salva metadati sul disco
         */
        void write() {
            File file = SPIFFS.open(_filename, "w");
            
            file.write((const uint8_t *) &_metadata, sizeof(_metadata));
            file.close();
        }
};

#endif
