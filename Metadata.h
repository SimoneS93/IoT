#ifndef _METADATA_H_
#define _METADATA_H_ 1

#include "FS.h"


/**
 * Metadata persistent storage
 */
class Metadata {

    public:

        Metadata(const char* filename) {
            _filename = filename;
        }

        ~Metadata() {
            write();
        }

        /**
         * 
         */
        uint32_t incrementSeqNo() {
            _metadata.seq_no = getSeqNo() + 1;

            return getSeqNo();
        }

        /**
         *
         */
        uint32_t getSeqNo() {
            read();
            return _metadata.seq_no;
        }

    private:
        bool _ready = false;
        const char *_filename;
        struct {
            uint32_t seq_no;
        } _metadata;

        /**
         * Read binary file from disk
         */
        void read() {
            if (_ready)
                return;

            SPIFFS.begin();
            File file = SPIFFS.open(_filename, "r");

            if (file) {
                file.read((uint8_t *) &_metadata, sizeof(_metadata));
                file.close();
            }
            else {
                _metadata.seq_no = 0;
            }

            _ready = true;
        }

        /**
         * Write binary file to disk
         */
        void write() {
            File file = SPIFFS.open(_filename, "w");
            
            file.write((const uint8_t *) &_metadata, sizeof(_metadata));
            file.close();
        }
};

#endif
