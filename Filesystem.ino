#include "defs.h"
#include "Scheduler.h"
#include "Serializer.h"
#include "Logger.h"
#include "Metadata.h"
#include "Network.h"

// #define SERVER 1


#ifdef SERVER


void onReceive(uint32_t from, String& msg) {
  PRINT("got message: ");
  PRINTLN(msg);
}

painlessMesh mesh;


void setup() {
  Serial.begin(115200);
  PRINTLN("setup");
  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | MSG_TYPES | REMOTE ); // all types on
  mesh.init(MESH_SSID, MESH_PASSWD);
  mesh.onReceive(&onReceive);
}

void loop() {
  mesh.update();
  delay(10);
}

#else

// prototype for the RECEIVED network event
void onReceive(uint32_t from, String& msg);

// prototype for a DATA_ACK network event
void onDataAck();


DirLogger logger = DirLogger("/data");
Network network;
bool isDone = false;


void setup() { Serial.begin(115200); SPIFFS.begin(); return;
  Metadata metadata("/metadata.bin");
  RTCScheduler scheduler;

  if (scheduler.shouldSense()) {
    sense(metadata.incrementSeqNo(), scheduler, logger);
  }

  if (!scheduler.shouldTX()) {
    SLEEP;
  }

  network.setup(&onReceive);
  network.onDataAck(&onDataAck);
}


void loop() {  
  network.update();


  // no matter what, after 2 minutes of activity go sleep
  ONCE_AFTER(SLEEP_TIMEOUT, 120 * 1000) {
    PRINTLN("Too much work, I gotta sleep!");
    SLEEP;
  }

  // send HELLO message while we don't know the server address
  if (!network.knowsServer() && !network.isWaiting()) {
    PRINTLN("sending HELLO");
    network.hello();
    delay(100);
    return;
  }

  // if we're waiting for the server, just wait
  if (!network.knowsServer()) {
    delay(10);
    return;
  }

  // send data to the server, one at a time
  // protocol is "Stop and Wait" to not overload the medium with much data
  if (logger.hasNext()) {
    if (!network.isWaiting()) {
      PRINT("sending data: "); PRINTLN(logger.next()); LB;
      network.sendData(logger.next());
    }
  }
  // if there's no more data, alert you're done on the next ack
  else {
    isDone = true;
  }

  delay(10);
}


/**
 * Read connected sensors
 * @todo
 */
void sense(uint32_t seqNo, Scheduler& scheduler, Logger& logger) {
  JSONSerializer serializer;

  serializer.append("seqNo", seqNo);
  serializer.append("ts", scheduler.now());
  serializer.append("temp", 22.0, 1);
  serializer.append("hum", 80);
  logger.append(seqNo, serializer.toString());

  PRINT("sensing serialization: ");
  PRINTLN(serializer.toString());
  LB;
}


/**
 * We just need to pass the params to the network.
 * It will handle the message
 */
void onReceive(uint32_t from, String& msg) {
  network.onReceive(from, msg);
}

/**
 * When we got an ack for a data packet, remove it from the logger.
 * It works with pop because is a Stop-and-Wait protocol!
 */
void onDataAck() {
  logger.pop();

  if (isDone) {
    network.bye();
  }
}

/**
 * Be sure to send the BYE message, wether on ACK or on TIMEOUT
 */
void onTimeout() {
  if (isDone) {
    network.bye();
  }
}


#endif
