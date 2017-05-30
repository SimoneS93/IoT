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
        }

        /**
         * Alert other nodes that you joined the network.
         * Then wait for the server to tell you its address.
         */
        void hello() {
            String msg(MSG_HELLO);

            _isWaiting = true;
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
        }

        /**
         * Add node to the known list
         */
        void add(uint32_t node) {
            _known.add(node);
        }

        /**
         * Remove node from the known list
         */
        void remove(uint32_t node) {
            _known.remove(node);
        }

        /**
         * Handle incoming messages
         */
        void onReceive(uint32_t from, String& msg) {
            if (msg.startsWith(MSG_HELLO)) {
                add(from);
            }
            else if (msg.startsWith(MSG_BYE)) {
                remove(from);
            }
            else if (msg.startsWith(MSG_SERVER)) {
                _isWaiting = false;
                setServer(msg);
            }
            else if (msg.startsWith(MSG_DATA_ACK)) {
                _isWaiting = false;
                
                if (_onDataAck)
                    _onDataAck();
            }
        }

        /**
         * Test if network is waiting for a response / ack
         */
        bool isWaiting() {
            return _isWaiting;
        }

        /**
         * Register callback for the receipt of a DATA_ACK message
         */
        void onDataAck(NetworkDataAckCallback callback) {
            _onDataAck = callback;
        }

    private:
        bool _isWaiting;
        uint32_t _server;
        StringPool<uint32_t> _known;
        NetworkDataAckCallback _onDataAck;
};

#endif
