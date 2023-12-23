import board
import busio
import digitalio
import usb_hid
from adafruit_hid.mouse import Mouse

import time
import sys

global uart
global DATA_RATE
global mouse_dx, mouse_dy
global mouse

DATA_RATE=9600
uart=None

MOUSE_WHEEL_DY = -1
MOUSE_CLICK = 200
MOUSE_LONG_PRESS = 300
MOUSE_CLICK_PRESS = 300

mouse_clicked = 0
mouse_down = 0
mouse_wheel = 0
mouse_drag = 0
mouse_move = 0
mouse_dx = 1
mouse_dy = 1

def mouse_init():
    global mouse
    mouse = Mouse(usb_hid.devices)

def sleep_ms(n):
    secs = n * 0.001
    time.sleep(secs)

def ticks_ms():
    ms = int(time.monotonic() * 1000.0 +0.5)
    return ms

def log(a="",b="",c=""):
    print(a,b,c)
    pass

def uart_init(rate):
    global uart
    uart = busio.UART(
        board.GP4,  # TX
        board.GP5,  # RX
        baudrate=rate,
        bits=8,  # Number of bits
        parity="none",  # Parity
        stop=1,  # Number of stop bits
        timeout=0
    )

def read(n=0):
    global uart
    if n==0: ## no block
        b = uart.read(1) 
    else: ## blocking
        b = None
        while b == None:
            b = uart.read(1) 
    result = b if b == None else b[0]
    if result != None: log(b)
    return result

def sign_extend(byte):
    if byte & 0x40:
        return byte - 0x80
    else:
        return byte

uart_init(DATA_RATE)
mouse_init()
while True:
    now = ticks_ms()
    if mouse_down and not mouse_drag:
        if (now - mouse_down) > MOUSE_LONG_PRESS:
            mouse_wheel = now
            mouse_down = 0
            log("start wheel");
    b = read()
    if b == None:
        continue
    if b > 127:
        down=b & 0x04
        y1=read(1)
        x1=read(1)
        y0=read(1)
        x0=read(1)
        dx=sign_extend(x0)
        dy=sign_extend(y0)
        if down:
            now = ticks_ms()
            if dx==0 and dy==0:
                if mouse_clicked and (now - mouse_clicked) < MOUSE_CLICK_PRESS:
                    mouse.press(Mouse.LEFT_BUTTON)
                    mouse_clicked = 0
                    mouse_wheel = 0
                    mouse_down = 0
                    mouse_drag = now
                    log("pressed")
                else:
                    mouse_dx = 1
                    mouse_dy = 1
                    mouse_down = now
                    log("down", mouse_down)
            else:
                if mouse_wheel:
                    mouse.move(wheel = dy * MOUSE_WHEEL_DY)
                else:
                    mouse.move(dx * mouse_dx, dy * mouse_dy)
                    if mouse_move:
                        delta = now - mouse_move
                        mult = 1
                        if False:
                            if delta < 20: mult = 10
                            elif delta < 40: mult = 8
                            elif delta < 60: mult = 6
                            elif delta < 80: mult = 4
                            elif delta < 100: mult = 3
                            elif delta < 120: mult = 2
                            else: mult = 2
                        # log("mult", mult)
                        mouse_dx = mult
                        mouse_dy = mult
                    mouse_move = now
                mouse_down = 0
        else:
            now = ticks_ms()
            if mouse_down and (now - mouse_down) < MOUSE_CLICK:
                mouse.click(Mouse.LEFT_BUTTON)
                mouse_clicked = now
                log("click")
            else:
                mouse.release(Mouse.LEFT_BUTTON)
                log("release")
            mouse_down = 0
            mouse_wheel = 0
            mouse_drag = 0
            mouse_move = 0
        continue
