#include <dummy.h>

#include <fabgl.h>

#include "PS2Mouse.h"
#include "Arduino.h"

#define INTELLI_MOUSE 3
#define SCALING_1_TO_1 0xE6
#define RESOLUTION_8_COUNTS_PER_MM 3

#define BIT_SET bitWrite
#define BIT_GET bitRead

#define MAX_MILLIS 700

extern void send_key(int);
extern void send_byte(int);
extern void send_bit(int);
extern void blink();

enum Commands
{
    SET_RESOLUTION = 0xE8,
    REQUEST_DATA = 0xEB,
    SET_REMOTE_MODE = 0xF0,
    GET_DEVICE_ID = 0xF2,
    SET_SAMPLE_RATE = 0xF3,
    RESET = 0xFF,
};

PS2Mouse::PS2Mouse(int clockPin, int dataPin)
{
    _clockPin = clockPin;
    _dataPin = dataPin;
    _supportsIntelliMouseExtensions = false;
}

void PS2Mouse::high(int pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
}

void PS2Mouse::low(int pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void PS2Mouse::initialize()
{
    high(_clockPin);
    high(_dataPin);
    reset();
    checkIntelliMouseExtensions();
    setResolution(RESOLUTION_8_COUNTS_PER_MM);
    setScaling(SCALING_1_TO_1);
    setSampleRate(40);
    setRemoteMode();
    delayMicroseconds(100);
    send_key(KEY_Z);
}

void PS2Mouse::start_send()
{
    high(_clockPin);
    high(_dataPin);
    delayMicroseconds(100);
    low(_clockPin);
    delayMicroseconds(100);
    low(_dataPin);
    delayMicroseconds(100);
    wait_for_clock_state(HIGH);
}

int PS2Mouse::digital_read(int pin)
{
    pinMode(pin, INPUT_PULLDOWN);
    return digitalRead(pin);
}

void PS2Mouse::start_read()
{
    high(_clockPin);
    high(_dataPin);
    delayMicroseconds(50);
    wait_for_clock_state(LOW);
    delayMicroseconds(5);
}

void PS2Mouse::write_byte(char data)
{
    int parityBit = 1;
    // send_key(KEY_W);
    start_send();
    // data
    for (int i = 0; i < 8; i++)
    {
        int dataBit = BIT_GET(data, i);
        // send_bit(dataBit);
        write_bit(dataBit);
        parityBit = parityBit ^ dataBit;
    }

    // parity bit
    write_bit(parityBit);

    // stop bit
    high(_dataPin);
    delayMicroseconds(50);
    wait_for_clock_state(LOW);

    // wait for mouse to switch modes
    while ((digital_read(_clockPin) == LOW) || (digital_read(_dataPin) == LOW))
        ;

    // put a hold on the incoming data
    // low(_clockPin);
    // send_key(KEY_COMMA);
}

void PS2Mouse::write_bit(int bit)
{
    wait_for_clock_state(HIGH);
    wait_for_clock_state(LOW);

    if (bit == HIGH)
    {
        high(_dataPin);
    }
    else
    {
        low(_dataPin);
    }
}

char PS2Mouse::read_byte()
{
    char data = 0;
    // send_key(KEY_R);
    start_read();
    // consume the start bit
    wait_for_clock_state(HIGH);

    // consume 8 bits of data
    for (int i = 0; i < 8; i++)
    {
        int b = read_bit();
        send_bit(b);
        BIT_SET(data, i, b);
    }

    // consume parity bit (ignored)
    read_bit();

    // consume stop bit
    read_bit();

    // put a hold on the incoming data
    low(_clockPin);
    send_key(KEY_RETURN);
    delayMicroseconds(100);
    return data;
}

void PS2Mouse::setSampleRate(int rate)
{
    send_key(KEY_E);
    writeAndReadAck(SET_SAMPLE_RATE);
    writeAndReadAck(rate);
}

void PS2Mouse::writeAndReadAck(int data)
{
    write_byte((char)data);
    read_byte();
}

void PS2Mouse::reset()
{
    // send_key(KEY_A);
    writeAndReadAck(RESET);
    read_byte(); // self-test status
    read_byte(); // mouse ID
}

void PS2Mouse::checkIntelliMouseExtensions()
{
    send_key(KEY_B);
    // IntelliMouse detection sequence
    setSampleRate(200);
    setSampleRate(100);
    setSampleRate(80);

    char deviceId = getDeviceId();
    _supportsIntelliMouseExtensions = (deviceId == INTELLI_MOUSE);
}

char PS2Mouse::getDeviceId()
{
    writeAndReadAck(GET_DEVICE_ID);
    return read_byte();
}

void PS2Mouse::setScaling(int scaling)
{
    send_key(KEY_D);
    writeAndReadAck(scaling);
}

void PS2Mouse::setRemoteMode()
{
    send_key(KEY_F);
    writeAndReadAck(SET_REMOTE_MODE);
}

void PS2Mouse::setResolution(int resolution)
{
    send_key(KEY_C);
    writeAndReadAck(SET_RESOLUTION);
    writeAndReadAck(resolution);
}

void PS2Mouse::wait_for_clock_state(int expectedState)
{
    while (digital_read(_clockPin) != expectedState)
        ;
}

MouseData PS2Mouse::readData()
{
    MouseData data;
    request_data();
    data.status = read_byte();
    data.position.x = read_byte();
    data.position.y = read_byte();

    if (_supportsIntelliMouseExtensions)
    {
        data.wheel = read_byte();
    }

    return data;
};

void PS2Mouse::request_data()
{
    writeAndReadAck(REQUEST_DATA);
}

void PS2Mouse::writeByte(int byte)
{
    int parity = 1; // odd parity
    pinMode(_clockPin, INPUT);
    pinMode(_dataPin, INPUT);
    delayMicroseconds(100);
    pinMode(_clockPin, OUTPUT);
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(100);
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_dataPin, LOW);
    delayMicroseconds(10);
    pinMode(_clockPin, INPUT);

    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x01);
    if (byte & 0x01) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x02);
    if (byte & 0x02) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x04);
        if (byte & 0x04) parity++;

    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x08);
        if (byte & 0x08) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x20);
    if (byte & 0x10) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x20);
    if (byte & 0x20) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, byte & 0x40);
    if (byte & 0x40) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);
    
    digitalWrite(_dataPin, byte & 0x80);
    if (byte & 0x80) parity++;
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, parity & 0x01); // parity
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    digitalWrite(_dataPin, 1); // stop
    while (digitalRead(_clockPin) != HIGH);
    while (digitalRead(_clockPin) != LOW);

    pinMode(_dataPin, INPUT);
    while (digitalRead(_dataPin) != LOW);
    while (digitalRead(_clockPin) != LOW);
    delayMicroseconds(50);
}

inline int PS2Mouse::wait_for_pin(int pin, int state) {
    unsigned long int timeout = millis() + MAX_MILLIS;
    while (digitalRead(pin) != state) {
      if (millis() > timeout) return 0xF300;
    }
    return 0;
}

inline int PS2Mouse::wait_for_transition(int pin, int start_state, int end_state) {
    if (wait_for_pin(_clockPin, start_state)) return 0xF100;
    if (wait_for_pin(_clockPin, end_state)) return 0xF200;
    return 0;
}

inline int PS2Mouse::read_bit()
{
    int error = wait_for_transition(_clockPin, HIGH, LOW);
    if (error) return error;
    return digitalRead(_dataPin);
}

int PS2Mouse::readByte() {
  int byte = 0;
  int parity = 0;
  int bit = 0;

  pinMode(_clockPin, INPUT);
  int start_bit = read_bit();
  if (start_bit & 0x8000) return 0x80FF;
  if (start_bit != 0x00) return 0x8002;
  for(int i=0x01; i<= 0x80; i = i << 1) {
    bit = read_bit();
  if (bit & 0x8000) return 0x80FF;
    parity += bit;
    byte |= bit ? i : 0;
  }
  bit = read_bit();
  if (bit & 0x8000) return 0x80FF;
  parity += bit;
  if ((parity & 0x01) != 1) return 0x8005;
  int stop_bit = read_bit();
  if (stop_bit < 0) return 0x80FF;
  if (stop_bit != 1) return 0x8007;

  pinMode(_clockPin, OUTPUT);
  digitalWrite(_clockPin, 0);
  delayMicroseconds(50);
  return byte;

}
int PS2Mouse::reportClock()
{
    return digitalRead(_clockPin);
}