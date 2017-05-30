#ifndef _LOGGER_H_
#define _LOGGER_H_ 1


#include "FS.h"


/**
 * Logger interface
 */
class Logger {
    public:
        /**
         * Initialize the Logger
         */
        virtual void open() = 0;

        /**
         * Destruct the Logger
         */
        virtual void close() = 0;

        /**
         * Append data
         */
        virtual bool append(uint32_t seqNo, String data) = 0;

        /**
         * Check if there's more data
         */
        virtual bool hasNext() = 0;

        /**
         * Consume data
         */
        virtual String next() = 0;

        /**
         * Delete the last log
         */
        virtual void pop() = 0;
};


/**
 * Contrete implementation of Logger.
 * Store each record as a file (for easy CRUD)
 */
class DirLogger : public Logger {
    public:

        DirLogger(const char* storage) {
            _dirname = storage;
        }

        void open() {
            if (!_ready) {
                SPIFFS.begin();
                _ready = true;
                _dir = SPIFFS.openDir(_dirname);
            }
        }

        void close() {
            _ready = false;
        }

        bool append(uint32_t seqNo, String data) {
            SPIFFS.begin();

            String filename = String(_dirname) + String("/") + String(seqNo) + String(".txt");
            File file = SPIFFS.open(filename, "w");

            if (file) {
                file.print(data);
                file.print("\n");
                file.close();

                return true;
            }

            return false;
        }

        bool hasNext() {
            open();

            return _dir.next();
        }

        String next() {
            String contents;
            File file = _dir.openFile("r");

            _filename = _dir.fileName();
            contents = file.readStringUntil('\n');
            file.close();

            return contents;
        }

        void pop() {
            SPIFFS.remove(_filename);
        }

    private:
        bool _ready = false;
        const char *_dirname;
        String _filename;
        Dir _dir;
};

#endif