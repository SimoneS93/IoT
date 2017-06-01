#define CLIENT 1
#define SERVER 0
#define TEST 2

#include "defs.h"
#include "Scheduler.h"
#include "Serializer.h"
#include "Logger.h"
#include "Metadata.h"
#include "Network.h"


#ifdef TEST
    #include "TEST.h"
#else
    #ifdef SERVER
        #include "SERVER.h"
    #else
      #ifdef CLIENT
        #include "CLIENT.h"
      #endif
    #endif
#endif
