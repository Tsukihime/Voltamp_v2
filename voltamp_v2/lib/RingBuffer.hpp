#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <string.h>
#include <stdint.h>

template <size_t size, typename T = uint8_t>
class RingBuffer {
    private:
        T buffer[size];
        uint8_t tail;
        uint8_t head;
        uint8_t count;

    public:
        void clear() {
            tail = 0;
            head = 0;
            count = 0;
        }

        bool push(T item) {
            if (count < size) {
                buffer[tail] = item;
                count++;
                tail++;
                if (tail == size) {
                    tail = 0;
                }
                return true;
            }
            return false;
        }

        bool pop(T &item) {
            if (count > 0) {
                item = buffer[head];
                count--;
                head++;
                if (head == size) {
                    head = 0;
                }
                return true;
            }
            return false;
        }
};

#endif /* RINGBUFFER_H_ */
