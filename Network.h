#ifndef _NETWORK_H_
#define _NETWORK_H_ 1


#include <painlessMesh.h>


#ifndef MESH_SSID
    #define MESH_SSID "AgriMesh"
#endif
#ifndef MESH_PASSWD
    #define MESH_PASSWD "AgriMesh"
#endif


/**
 * onReceive callback prototype.
 * Since we can't pass a method reference, we're forced to
 * declare a "global" network instance and work on that
 */
void onReceive(uint32_t from, String& msg);


class Network {

    public:
        painlessMesh _mesh;

        /**
         *
         */
        void setup() {
            _sentHello = false;
            _server = 0;
            _mesh.setDebugMsgTypes( ERROR | MESH_STATUS | MSG_TYPES | REMOTE ); // all types on
            _mesh.init(MESH_SSID, MESH_PASSWD);
            _mesh.onReceive(&onReceive);
        }

        /**
         *
         */
        void update() {
            _mesh.update();
        }

        /**
         * Segnala alla rete la tua presenza
         * Questo triggera la risposta dal server
         */
        void hello() {
            String msg("HELLO");

            if (!_sentHello) {
                Serial.println("sendind hello...");
                _mesh.sendBroadcast(msg);
                _sentHello = true;
            }
        }

        /**
         * Set server address from message
         */
        void setServer(String msg) {
            char serverStr[32];

            msg.substring(len(MSG_SERVER)).toCharArray(serverStr, 32);
            _server = atoi(serverStr);
        }

        /**
         * Be sure to know the receiver before start sending the messages
         */
        bool knowsServer() {
            return _server != 0;
        }

        /**
         * Verifica se tutti i nodi hanno finito di comunicare
         */
        bool isEmpty() {
            return false;
            //return _known.size() == 0;
        }

        /**
         * Send data to the server
         */
        void sendData(String data) {
            String msg = MSG_DATA;

            msg += data;
            _mesh.sendSingle(_server, msg);
        }

    private:
        bool _sentHello;
        uint32_t _server;

        /**
         * Aggiungi nodo alla lista dei nodi attivi
         */
        void add(uint32_t node) {
            //_known.add(node);
        }

        /**
         * Rimuovi nodo dalla lista dei nodi attivi
         */
        void remove(uint32_t node) {
            //_known.remove(node);
        }
};

// "global" instance
Network network;


/**
 * Parse message and take proper action
 */
void onReceive(uint32_t from, String &msg) {
    Serial.print("rcv: "); Serial.println(msg);

    if (msg.startsWith(MSG_SERVER)) {
        Serial.println("setting server...");
        network.setServer(msg);
    }
}

#endif
