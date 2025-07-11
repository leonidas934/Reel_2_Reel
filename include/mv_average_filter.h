#ifndef MV_AVERAGE_FILTER_H
#define MV_AVERAGE_FILTER_H
#include <math.h>
class mv_average_filter {
private:
    int count;
    int MAX_SIZE; // Define a fixed size
    double* data;

public:
    mv_average_filter(int _max_size) : count(0), MAX_SIZE(_max_size) {
        data = new double[MAX_SIZE];
    }

    void push(double value) {
        if (isnan(value) || isinf(value)) return;
        if (count == MAX_SIZE) {
            // Shift the elements to make space for the new value
            for (int i = MAX_SIZE - 1; i > 0; --i) {
                data[i] = data[i - 1];
            }
            data[0] = value; // Add the new value at the start
        } else {
            for (int i = count; i > 0; --i) {
                data[i] = data[i - 1];
            }
            data[0] = value;
            count++;
        }
    }

    int get_size() const {
        return count;
    }

    double get_average() const {
        if (count == 0) return 0.0; // Return 0.0 if the filter is empty
        double sum = 0.0;
        for (int i = 0; i < count; ++i) {
            sum += data[i];
        }
        double avg = sum / count;
        //if (isnan(avg) || isinf(avg)) return 0.0; // Return safe value
        return avg;
    }

    void init() {
        count = 0; // Reset count to zero
        for (int i = 0; i < MAX_SIZE; ++i) {
            data[i] = 0.0; // Clear the buffer
        }
    }
    ~mv_average_filter() {
    delete[] data; // You must add this to avoid memory leak
    }
};

#endif
