#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_ 1


/**
 * Interface for a serializer
 */
class Serializer {
    public:

        /**
         * Append key/value pair to the serialization
         */
        virtual void append(const char* key, float value, uint8_t decimals = 0) = 0;

        /**
         * Return serialization as String
         */
        virtual String toString() = 0;
};


/**
 * Concrete implementation of Serializer.
 * Serialize as JSON
 */
class JSONSerializer : public Serializer {
    public:

        void append(const char* key, float value, uint8_t decimals = 0) {
            _buffer += ",\"";
            _buffer += key;
            _buffer += "\":";
            _buffer += String(value, decimals);
        }

        String toString() {
            // `append` always prepends a `,` in front of the serialization
            // replace the first one to create a valid JSON
            _buffer.setCharAt(0, '{');

            if (!_buffer.endsWith("}"))
                _buffer += "}";

            return _buffer;
        }

    private:
        String _buffer;
};

#endif