import machine
import utime
import sys

global uart, i2c
global CMD_RATE, DATA_RATE
global DATA_RATE
global USE_I2C, devices, device_address
global x,y

CMD_RATE=1200
DATA_RATE=9600
USE_I2C=False
devices=None
device_address=None
i2c=None
uart=None
x=0
y=0

def uart_init(rate):
    global uart, i2c
    if USE_I2C:
        i2c = machine.I2C(0, sda=machine.Pin(4), scl=machine.Pin(5), freq=100000)
        devices = i2c.scan()
        print(devices)
        if len(devices) == 0:
            sys.exit(0)
    else:
        uart = machine.UART(1, tx=machine.Pin(4), rx=machine.Pin(5))
        uart.init(rate, bits=8, parity=0, stop=1)

def reverse_bits(b):
    result = 0
    for i in range(0, 8):
        result = result * 2
        if b % 2: result = result + 1
        b = b // 2
    return result
    
def hex(b):
    digits = "0123456789ABCDEF"
    sgn = " "
    c=""
    if b < 0:
        sgn = '-'
        b = -b
    if b > 255:
        sgn = '!' + hex(b//256)
        b = b & 255
    #b = reverse_bits(b)
    #b = b ^ 0x7F
    if b >=32 and b <= 98: c = "-"+chr(b)
    hi = b // 16
    lo = b % 16
    return sgn+digits[hi]+digits[lo]+c

def brk(rate=DATA_RATE):
    global DATA_RATE, i2c, uart
    if USE_I2C:
        pass
    else:
        uart.deinit()  # Deinitialize the UART
        tx_pin = machine.Pin(4, machine.Pin.OUT)
        tx_pin.value(0)  # Pull the TX line low
        utime.sleep_ms(25)  # Keep the line low for 25 ms to simulate a break signal
        uart_init(rate)

def cmd(arr):
    global DATA_RATE, CMD_RATE, i2c
    print("sending", arr)
    if USE_I2C:
        i2c.writeto(device_address, bytes(arr))
    else:
        brk(CMD_RATE)
        uart.write(bytes(arr))
        uart_init(DATA_RATE)        

def read(n=0):
    global uart, i2c
    if USE_I2C:
        b = i2c.readfrom(device_address, 1)
    else:
        if n>0:
            while not uart.any():
                pass
        b = uart.read(1)
    return b if b == None else b[0]

def sign_extend(byte):
    if byte & 0x40:
        return byte - 0x80
    else:
        return byte

uart_init(DATA_RATE)
brk()
cmd([0x25,0x65])

xxx = 0
later = utime.ticks_ms() + 5 * 1000
sx=0
sy=0
ex=0
ey=0
tracking=False
while True:
    now = utime.ticks_ms()
    if later and (now > later):
        cmd([0x25,0x65])
        later = 0
    b = read()
    if b == None: continue
    if b > 127:
        down=not not (b & 0x04)
        if down:
            sx=ex
            sy=ey
            #if not tracking: print("start:", sx,sy)
            tracking = True
        else:
            print("end:", ex,ey)
            tracking = False
        y1=read(1)
        x1=read(1)
        y0=read(1)
        x0=read(1)
        dx=sign_extend(x0)
        dy=sign_extend(y0)
        ex += dx
        ey += dy
        #print(b,down,dx,dy,x,y)
        continue
    # print(xxx, hex(b))
    xxx += 1
    if xxx == 4:
        print("brk")
        brk()
    if xxx == 20:
        print("sending 2")
        uart.write(bytes([0x25,0x65]))

