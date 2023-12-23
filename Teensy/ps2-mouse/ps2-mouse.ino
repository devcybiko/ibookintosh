#include "PS2Mouse.h"
#define DATA_PIN 23
#define CLOCK_PIN 22
#define LED_PIN 13

int blink_count = 0;

PS2Mouse mouse = PS2Mouse(CLOCK_PIN, DATA_PIN);


void blink(int mask)
{
  digitalWrite(LED_PIN, (blink_count++) & mask);
}

void send_key(int key)
{
  Keyboard.set_key1(key);
  // Keyboard.set_key2(slot2);
  // Keyboard.set_key3(slot3);
  // Keyboard.set_key4(slot4);
  // Keyboard.set_key5(slot5);
  // Keyboard.set_key6(slot6);
  Keyboard.send_now();
  // delay(10);
  Keyboard.set_key1(0);
  Keyboard.send_now();
}

void send_byte(int b)
{
  for (int i = 0x80; i > 0; i = i / 2)
  {
    if (i == 0x80) send_key(KEY_SPACE);
    if (i == 0x08) send_key(KEY_SPACE);
    if (b & i)
      send_key(KEY_1);
    else
      send_key(KEY_0);
  }
  send_key(KEY_COMMA);
}

void send_word(int b)
{
  for (int i = 0x8000; i > 0; i = i / 2)
  {
    if (i == 0x8000) send_key(KEY_SPACE);
    if (i == 0x800) send_key(KEY_SPACE);
    if (i == 0x80) send_key(KEY_SPACE);
    if (i == 0x08) send_key(KEY_SPACE);
    if (b & i)
      send_key(KEY_1);
    else
      send_key(KEY_0);
  }
  send_key(KEY_COMMA);
}

void send_bit(int b)
{
  if (b)
    send_key(KEY_1);
  else
    send_key(KEY_0);
}

void report_mouse() {
    int buffer[32][100];
  // send_key(KEY_X);
  // send_key(KEY_RETURN);
  for (int j = 0; j < 32; j++)
  {
    for (int i = 0; i < 100; i++)
    {
      buffer[j][i] = digitalRead(CLOCK_PIN);
      delayMicroseconds(10);
    }
  }
  for (int j = 0; j < 32; j++)
  {
  int zeroes = 0;
  int ones = 0;
    for (int i = 0; i < 100; i++)
    {
      if(buffer[j][i]) ones++;
      else zeroes++;
    }
    send_key(KEY_0);
    send_key(KEY_SPACE);
    send_byte(zeroes);
    send_key(KEY_SPACE);
    send_key(KEY_1);
    send_key(KEY_SPACE);
    send_byte(ones);
    send_key(KEY_RETURN);
  }
  send_key(KEY_RETURN);
}

void setup()
{
  delay(4000); // give me a couple seconds to get out
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);
  send_key(KEY_Z);

  mouse.writeByte(0xFF);
  int a = mouse.readByte(); if (a != 0xFA) error(50, KEY_A, a);
  int b = mouse.readByte(); if (b != 0xAA) error(250, KEY_B, b);
  int c = mouse.readByte(); if (c != 0x00) error(1000, KEY_C, c);
  mouse.writeByte(0xEA);
  // int d = mouse.readByte(); if (d != 0xFE) error(50, KEY_D, d);
  digitalWrite(LED_PIN, 0);
  // mouse.initialize();
  send_key(KEY_Y);
}

void error(int d, int key, int value) {
    send_key(KEY_SPACE);
    send_key(key);
    send_key(KEY_SPACE);
  send_word(value);
  send_key(KEY_RETURN);
  while(true) {
  delay(d);
  blink(0x01);
  }

}
void loop()
{
  int byte = mouse.readByte();
  send_word(byte);
  // if (byte < 0 || byte > 0xff) blink(0x80);
  // if ((byte & 0xFF) > 1) send_byte(byte);
  // MouseData data = mouse.readData();
  // send_byte(data.position.x);
  // Serial.print(data.status, BIN);
  // Serial.print("\tx=");
  // Serial.print(data.position.x);
  // Serial.print("\ty=");
  // Serial.print(data.position.y);
  // Serial.print("\twheel=");
  // Serial.print(data.wheel);
  // Serial.println();
}
