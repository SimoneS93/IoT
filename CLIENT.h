// prototype for the RECEIVED network event
void onReceive(uint32_t from, String& msg);

// prototype for a DATA_ACK network event
void onDataAck();
void onTimeout();
void sense(uint32_t seqNo, Scheduler& scheduler, Logger& logger);


DirLogger logger = DirLogger("/data");
Network network;
bool isDone = false;


void setup() { Serial.begin(115200); SPIFFS.begin();
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
  network.onTimeout(&onTimeout);
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
    delay(10);
    return;
  }

  // if we're waiting for the server, just wait
  if (!network.knowsServer()) {
    delay(10);
    return;
  }

  // send data to the server, one at a time
  // protocol is "Stop and Wait" to not overload the medium with much data
  if (!network.isWaiting()) {
    if (logger.hasNext()) {
        PRINT("sending data: "); PRINTLN(logger.next()); LB;
        network.sendData(logger.next());
    }
    // if there's no more data, alert you're done on the next ack
    else {
      isDone = true;

      if (network.isEmpty()) {
        PRINTLN("all done. go sleep...");
        delay(5000);
        SLEEP;
      }
    }
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
  PRINTLN("timeout");

  if (!network.knowsServer()) {
    return;
  }
  
  if (isDone) {
    if (!network.isWaiting()) {
      PRINTLN("done... send BYE");
      network.bye();
    }
  }
  else {
    PRINT("re-sending data: "); PRINTLN(logger.current()); LB;
    network.sendData(logger.current());
  }
}