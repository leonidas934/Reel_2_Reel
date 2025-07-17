#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

class Encoder {
private:
    const int A_pin;
    const int B_pin;
    const int pulses_per_rev = 2048;

    static const int BUFFER_SIZE = 10;
    volatile unsigned long intervals[BUFFER_SIZE];
    volatile int buffer_index = 0;
    volatile int interval_count = 0;

    volatile unsigned long last_pulse_time = 0;

public:
    Encoder(int A_pin, int B_pin);
    void begin();
    double get_speed();
    void reset();
    void handlePulse();
};

Encoder* global_encoder_pointer = nullptr;

Encoder::Encoder(int A_pin, int B_pin) : A_pin(A_pin), B_pin(B_pin) {}

void Encoder::begin() {
    pinMode(A_pin, INPUT);
    pinMode(B_pin, INPUT);
    global_encoder_pointer = this;
    attachInterrupt(digitalPinToInterrupt(A_pin), []() {
        global_encoder_pointer->handlePulse();
    }, RISING);
}

void Encoder::handlePulse() {
    unsigned long current_time = micros();
    if (last_pulse_time != 0) {
        unsigned long interval = current_time - last_pulse_time;
        intervals[buffer_index] = interval;
        buffer_index = (buffer_index + 1) % BUFFER_SIZE;
        if (interval_count < BUFFER_SIZE) interval_count++;
    }
    last_pulse_time = current_time;
}

double Encoder::get_speed() {
    noInterrupts();
    unsigned long total_interval = 0;
    for (int i = 0; i < interval_count; i++) {
        total_interval += intervals[i];
    }
    int count_copy = interval_count;
    interrupts();

    if (count_copy == 0 || total_interval == 0) return 0.0;
    
    double average_interval = (double)total_interval / count_copy;
    double speed = (60.0 * 1000000.0) / (pulses_per_rev * average_interval);
    return speed;
}

void Encoder::reset() {
    noInterrupts();
    buffer_index = 0;
    interval_count = 0;
    last_pulse_time = 0;
    interrupts();
}

#endif