#ifndef MOUSE_H_

#define MOUSE_H_

typedef struct
{
    int x, y;
} Position;

typedef struct
{
    int status;
    Position position;
    int wheel;
} MouseData;

class PS2Mouse
{
private:
    int _clockPin;
    int _dataPin;
    bool _supportsIntelliMouseExtensions;

    void high(int pin);

    void low(int pin);

    void writeAndReadAck(int data);

    void start_send();

    void reset();

    void setSampleRate(int rate);

    void checkIntelliMouseExtensions();

    void setResolution(int resolution);

    char getDeviceId();

    void setScaling(int scaling);

    void setRemoteMode();

    void wait_for_clock_state(int expectedState);

    int digital_read(int pin);

    void request_data();

    char read_byte();

    int read_bit();

    int digital_read();

    void start_read();

    void write_byte(char data);

    void write_bit(int bit);

    int wait_for_pin(int pin, int state);
    int wait_for_transition(int pin, int start_state, int end_state);


public:
    PS2Mouse(int clockPin, int dataPin);

    void initialize();

    MouseData readData();

    int reportClock();
    void writeByte(int byte);
    int readByte();
};

#endif // MOUSE_H_