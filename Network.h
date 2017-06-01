#ifndef _NETWORK_H_
#define _NETWORK_H_ 1


#include <painlessMesh.h>
#include "Pool.h"


#ifndef MESH_SSID
    #define MESH_SSID "AgriMesh"
#endif
#ifndef MESH_PASSWD
    #define MESH_PASSWD "AgriMesh"
#endif


typedef void (*NetworkDataAckCallback)();
typedef void (*NetworkTimeoutCallback)();
typedef void (*NetworkReceiveCallback)(uint32_t from, String& msg);


class Network {

    public:
        painlessMesh _mesh;

        /**
         * Start the network
         */
        void setup(NetworkReceiveCallback callback) {
            _server = 0;
            _mesh.setDebugMsgTypes( ERROR | MESH_STATUS | MSG_TYPES | REMOTE ); // all types on
            _mesh.init(MESH_SSID, MESH_PASSWD);
            _mesh.onReceive(callback);
        }

        /**
         *
         */
        void update() {
            _mesh.update();

            if (millis() - _time > _timeout) {
              timedOut();
            }
        }

        /**
         * Alert other nodes that you joined the network.
         * Then wait for the server to tell you its address.
         */
        void hello() {
            String msg(MSG_HELLO);

            wait();
            _mesh.sendBroadcast(msg);
        }

        /**
         * Alert other nodes that you're done transmitting.
         */
        void bye() {
            String msg(MSG_BYE);

            _mesh.sendBroadcast(msg);
        }

        /**
         * Set server address from message
         */
        void setServer(uint32_t server) {
            PRINT("set server: "); PRINTLN(server);
            _server = server;
        }

        /**
         * Be sure to know the receiver before start sending the messages
         */
        bool knowsServer() {
            return _server != 0;
        }

        /**
         * Test if all known nodes finished transmitting
         */
        bool isEmpty() {
            return _known.isEmpty();
        }

        /**
         * Send data to the server
         */
        void sendData(String data) {
            String msg = MSG_DATA;

            msg += data;
            _mesh.sendSingle(_server, msg);
            wait();
        }

        /**
         * Add node to the known list
         */
        void onHello(uint32_t node) {
            #ifdef SERVER
              String ack = String(MSG_SERVER_ACK) + String(_mesh.getNodeId());
              
              PRINT("send server address to "); PRINTLN(ack);
              
              _mesh.sendSingle(node, ack);
            #else
              _known.add(node);
            #endif
        }

        /**
         * Remove node from the known list
         */
        void onBye(uint32_t node) {
            _known.remove(node);
        }

        /**
         * Handle incoming messages
         */
        void onReceive(uint32_t from, String& msg) {
            PRINT("received: "); PRINTLN(msg);
            
            if (msg.startsWith(MSG_HELLO)) {
                onHello(from);
            }
            else if (msg.startsWith(MSG_BYE)) {
                onBye(from);
            }
            else if (msg.startsWith(MSG_SERVER_ACK)) {
                _isWaiting = false;
                setServer(from);
            }
            else if (msg.startsWith(MSG_DATA)) {
              onData(from, msg);
            }
            else if (msg.startsWith(MSG_DATA_ACK)) {
                _isWaiting = false;
                
                if (_onDataAck)
                    _onDataAck();
            }
        }

        void onData(uint32_t from, String& msg) {
          String ack = String(MSG_DATA_ACK);
          
          PRINT("got data: "); PRINTLN(msg);

          _mesh.sendSingle(from, ack);
        }

        void onTimeout(NetworkTimeoutCallback callback) {
          _onTimeout = callback;
        }

        void timedOut() {
          _isWaiting = false;
          _time = millis();

          if (_onTimeout) {
            _onTimeout();
          }
        }

        /**
         * Test if network is waiting for a response / ack
         */
        bool isWaiting() {
            return _isWaiting;
        }

        void wait() {
            _time = millis();
            _isWaiting = true;
        }

        /**
         * Register callback for the receipt of a DATA_ACK message
         */
        void onDataAck(NetworkDataAckCallback callback) {
            _onDataAck = callback;
        }

    private:
        bool _isWaiting;
        unsigned long _time;
        unsigned long _timeout = 10000;
        uint32_t _server;
        StringPool<uint32_t> _known;
        NetworkDataAckCallback _onDataAck;
        NetworkTimeoutCallback _onTimeout;
};

#endif
