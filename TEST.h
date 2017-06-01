typedef void (*StateEnterCallback)(int, int);
typedef void (*StateLeaveCallback)(int, int);
typedef void (*StateUpdateCallback)(int);


class State {
    public:
        State(int id) {
            construct(id, NULL, NULL, NULL);
        }

        State(int id, StateEnterCallback enterCallback) {
            construct(id, enterCallback, NULL, NULL);
        }

        State(int id, StateEnterCallback enterCallback, StateUpdateCallback updateCallback) {
            construct(id, enterCallback, updateCallback, NULL);
        }

        State(int id, StateEnterCallback enterCallback, StateUpdateCallback updateCallback, StateLeaveCallback leaveCallback) {
            construct(id, enterCallback, updateCallback, leaveCallback);
        }

        int getId() {
          return id;
        }

        bool operator==(const State& other) {
          return id == other.id;
        }

    private:
        int id;
        StateEnterCallback enterCallback;
        StateLeaveCallback leaveCallback;
        StateUpdateCallback updateCallback;

        void construct(int id, StateEnterCallback enterCallback, StateUpdateCallback updateCallback, StateLeaveCallback leaveCallback) {
            this->id = id;
            this->enterCallback = enterCallback;
            this->updateCallback = updateCallback;
            this->leaveCallback = leaveCallback;
        }
};


#define MAP_SIZE 10

template<typename K, typename V>
class HashTable {

    public:
        void put(K key, V& value) {
            uint8_t index = 0;

            if (has(key)) {
                index = indexOfKey(key) - 1;
            }
            else {
                index = it++;
            }

            keys[index] = key;
            values[index] = value;
        }

        V& get(K key, V& defaults) {
            if (has(key)) {
                uint8_t index = indexOfKey(key) - 1;

                return values[index];
            }

            return defaults;
        }

        bool has(K key) {
            return indexOfKey(key) > 0;
        }

    private:
        uint8_t it = 0;
        K keys[MAP_SIZE];
        V values[MAP_SIZE];

        uint8_t indexOfKey(K key) {
            for (uint8_t i = 0; i < it; i++) {
                if (keys[i] == key)
                    return i+1;
            }

            return 0;
        }
};

typedef int StateEvent;
struct Edge {
  State *from;
  State *to;
  StateEvent event;

  void construct(State *from, State *to, StateEvent event) {
    this->from = from;
    this->to = to;
    this->event = event;
  }
};


#define MAX_EDGES 20
class States {
    public:

        States(State* initial) {
          current = initial;
        }
        
        bool edge(State *from, State *to, StateEvent event) {
            Edge edge;
            
            states.put(from->getId(), from);
            states.put(to->getId(), to);
            edge.construct(from, to, event);
            edges[it++] = edge;
        }

        bool trigger(StateEvent event) {
            Serial.print("it: "); Serial.println(it);
            for (uint8_t i = 0; i < it; i++) {
              Edge edge = edges[i];

              Serial.print("Cmp... "); Serial.print(edge.from->getId()); Serial.print(" vs "); Serial.println(current->getId());
              if (edge.from->getId() == current->getId() && edge.event == event) {
                Serial.print("transition to: "); Serial.println(edge.to->getId());
                current = edge.to;
                return true;
              }
            }

            return false;
        }

  private:
    uint8_t it = 0;
    State *current = 0;
    HashTable<uint8_t, State*> states;
    Edge edges[MAX_EDGES];
};




void test_fsm() {
  const int EV_HELLO = 1;
  const int EV_SENT = 2;
  const int EV_TIMEOUT = 3;
  State sInitial(1);
  State sHello(20);
  State sWait(50);
  States fsm(&sInitial);

  fsm.edge(&sInitial, &sHello, EV_HELLO);
  fsm.edge(&sHello, &sWait, EV_SENT);
  fsm.edge(&sWait, &sHello, EV_TIMEOUT);

  delay(1000);
  fsm.trigger(EV_HELLO);
  delay(1000);
  fsm.trigger(EV_SENT);
  delay(500);
  fsm.trigger(EV_TIMEOUT);
  delay(1000);
  fsm.trigger(EV_SENT);
}


void test_hasmap() {
  HashTable<int, String> map;
  String val("ciao");
  String ret;

  map.put(10, val);
  ret = map.get(10, ret);
  Serial.println(ret);
}




void setup() {
    Serial.begin(115200);
    Serial.println("BEGIN");

    test_fsm();
    
    Serial.println("END");
}

void loop() {

}
