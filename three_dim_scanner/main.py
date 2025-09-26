"""
Main file. This file is designed for collecting data from the Servos
and the distance sensor.
"""

import socket
import os


def servo_data_collect() -> None:
    """
    Collects Servo and Distance Sensor data and puts
    it into a .txt file to be processed later.
    """

    # Scanner data lists, collected from the Arduino
    servo_pan_data = []
    servo_tilt_data = []
    distance_sensor_data = []

    # Sending data via UDP. Data is sent in UDP packets and collected together from a socket
    sock: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5)
    ip: str = "192.168.16.124"
    return_string: str = "echo"
    buffer: int = 1024 * 20
    port: int = 8182
    response: bytes = bytes()

    try:
        # "end" is the end trigger response from the Arduino
        while response.decode() != "end":

            # data being sent to maintain a two-way connection
            sock.sendto(return_string.encode(), (ip, port))

            try:
                # response from Arduino
                response, _ = sock.recvfrom(buffer)

                # If the response isn't empty and isn't the ending trigger
                if len(response.decode()) > 1 and response.decode() != "end":

                    # Data is sent in two parts: the data type and the values from the response
                    response_type, response_values = tuple(response.decode().split(" "))
                    received_data = [  # converts data in string form to integers
                        int(data_point)
                        for data_point in response_values.strip("<>")[:-4].split(",")
                    ]
                    match response_type:
                        case "distance_sensor_data":
                            distance_sensor_data += received_data
                        case "servo_pan_data":
                            servo_pan_data += received_data
                        case "servo_tilt_data":
                            servo_tilt_data += received_data

            except socket.timeout:
                # In the case that the socket does not receive
                # any feedback within 5 seconds, ignore it
                pass
    except KeyboardInterrupt:
        # In the case that the program needs to end,
        # close the socket instead of throwing an error
        pass
    finally:
        sock.close()

    # Save data to files
    with open(
        os.path.join("data", "second_distance_sensor_data.txt"), "w+", encoding="utf-8"
    ) as file_1:
        file_1.write(str(distance_sensor_data))
    # with open(
    #     os.path.join("data", "servo_pan_data.txt"), "w+", encoding="utf-8"
    # ) as file_2:
    #     file_2.write(str(servo_pan_data))
    with open(
        os.path.join("data", "second_servo_tilt_data.txt"), "w+", encoding="utf-8"
    ) as file_3:
        file_3.write(str(servo_tilt_data))

    print("Collected!")


if __name__ == "__main__":
    servo_data_collect()
