#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_ 1


/**
 * Interfaccia per lo scheduling delle operazioni
 */
class Scheduler {
    public:

        /**
         * Decidi se è il momento per effettuare le rilevazioni
         */
        virtual bool shouldSense() = 0;

        /**
         * Decidi se è il momento per trasmettere i dati
         */
        virtual bool shouldTX() = 0;

        /**
         * Ottieni UNIX timestamp
         */
        virtual uint32_t now() = 0;
};


/**
 * Classe concreta che schedula sulla base di un RTC
 */
class RTCScheduler : public Scheduler {

    public:

        /**
         * Effettua le rilevazioni secondo le seguenti regole:
         *   - dalle 4 alle 12, ogni 5 minuti
         *   - dalle 12 alle 4, ogni 20 minuti
         */
        bool shouldSense() {
            // @todo
            return true;
        }

        /**
         * Trasmetti ogni ora
         */
        bool shouldTX() {
            // @todo
            return true;
        }

        /**
         *
         */
        uint32_t now() {
            // @todo
            return 0;
        }
};

#endif