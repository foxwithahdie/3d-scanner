"""
Main file.
"""

import socket


def main() -> None:
    """
    Main function.
    """

    # arduino_serial: serial.Serial = serial.Serial(
    #     port="COM13", baudrate=115200, timeout=0.1
    # )
    sock: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5)
    try:
        while True:
            # arduino_serial.write(bytes("baseline_receive", encoding="utf-8"))
            # time.sleep(1)
            # data_serial: Optional[bytes] = arduino_serial.readline()
            # print(data_serial.decode())

            sock.sendto("echo".encode(), ("192.168.16.172", 8182))
            try:
                response, addr = sock.recvfrom(1024 * 20)
                if len(response.decode()) > 1:
                    print(
                        "data",
                        [
                            int(data_point)
                            for data_point in response.decode()[1:-4].split(",")
                        ],
                    )
                    with open("data/test.txt", "a", encoding="utf-8") as file:
                        file.write(
                            str(
                                [
                                    int(data_point)
                                    for data_point in response.decode()[1:-4].split(",")
                                ]
                            )
                        )
                # else:
                # print(response)
            except socket.timeout:
                pass

            # data, addr = sock.recvfrom(255)
            # print("wifi data: ", data)

            # if data != b"":
            #     converted_data: list[str] = (
            #         data.decode(encoding="utf-8").strip("<>")[:-1].split(",")
            #     )

            #     real_data: list[int] = [
            #         int(individual_byte) for individual_byte in converted_data
            #     ]
            #     print(real_data)
    except KeyboardInterrupt:
        pass
    finally:
        sock.close()


if __name__ == "__main__":
    main()
