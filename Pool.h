#ifndef _POOL_H_
#define _POOL_H_ 1


/**
 * Interface for a pool.
 * Pool means a collection of unique values that can only be
 * added or removed by value, with no respect to the order.
 */
template<typename T>
class Pool {
    public:
        /**
         * Add value to the pool
         */
        virtual void add(T value) = 0;

        /**
         * Remove value from the pool.
         */
        virtual bool remove(T value) = 0;

        /**
         * Test if pool has 0 elements
         */
        virtual bool isEmpty() = 0;
};


/**
 * Concrete Pool implementation.
 * Implement pool via String (not very efficient, but easy).
 * T must be strinifiable.
 */

 #ifndef STRINGPOOL_SEPARATOR
    #define STRINGPOOL_SEPARATOR "#"
#endif

template<typename T>
class StringPool : public Pool<T> {
    public:

        StringPool() {
            _len = 0;
            _pool = STRINGPOOL_SEPARATOR;
        }

        void add(T value) {
            remove(value);

            _len++;
            _pool += String(value) + String(STRINGPOOL_SEPARATOR);
        }

        bool remove(T value) {
            uint8_t size = _pool.length();
            String search = String(STRINGPOOL_SEPARATOR) + String(value) + String(STRINGPOOL_SEPARATOR);

            _pool.replace(search, STRINGPOOL_SEPARATOR);

            if (_pool.length() != size) {
                _len--;

                return true;
            }

            return false;
        }

        bool isEmpty() {
            return _len == 0;
        }

    private:
        uint8_t _len;
        String _pool;
};


#endif