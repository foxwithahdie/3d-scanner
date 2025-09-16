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
        arduino.write(bytes("baseline_receive", encoding="utf-8"))
        data: Optional[bytes] = arduino.readline()
        if data != b"":
            converted_data: list[str] = (
                data.decode(encoding="utf-8").strip("<>")[:-1].split(",")
            )

            real_data: list[int] = [
                int(individual_byte) for individual_byte in converted_data
            ]

            print(real_data)


if __name__ == "__main__":
    main()
