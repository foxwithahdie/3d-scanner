"""
Main file.
"""

from typing import Optional
import time
import serial


def main() -> None:
    """
    Main function.
    """
    arduino: serial.Serial = serial.Serial(port="COM13", baudrate=115200, timeout=0.1)
    while True:
        arduino.write(bytes("received", encoding="utf-8"))
        time.sleep(5)
        data: Optional[bytes] = arduino.readline()
        print(data)


if __name__ == "__main__":
    main()
