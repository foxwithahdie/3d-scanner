"""
Understanding the data that we got from the sensors.
"""

import ast
import os
from collections.abc import Callable

import numpy as np
import matplotlib.pyplot as plt

import three_dim_scanner.calibration_data as calibration_data


def convert_dataset(dataset: str) -> np.ndarray:
    """
    Converts a dataset of analog values ranging from 0-1023 to distances,
    using the quadratic function of best fit.

    Parameters
    ----------
    dataset : str
        The filepath to the analog dataset.

    Returns
    -------
    np.ndarray
        A list of the distances, converted from the analog data.
    """
    with open(dataset, "r", encoding="utf-8") as file:
        # Because the data in the file is put in in the structure of a Python list,
        # it can be read like one with ast.literal_eval
        analog_list: list[int] = ast.literal_eval(file.read())
        converted_analog_list: np.ndarray = np.array(analog_list, dtype=float)

    # Function to convert the analog data to distance.
    analog_conversion: Callable[[np.ndarray], np.ndarray] = (
        calibration_data.function_from_calibration()
    )
    return analog_conversion(converted_analog_list)


def gather_servo_data_from_file(dataset: str) -> list[int]:
    """
    Gathers the Servo tilt/pan data from a dataset file.

    Parameters
    ----------
    dataset : str
        The filepath to the dataset.

    Returns
    -------
    list[int]
        The list of angles of the pan/tilt Servos.
    """
    with open(dataset, "r", encoding="utf-8") as file:
        servo_list = ast.literal_eval(file.read())

    return servo_list


def plot_histogram(histo_data: str) -> None:
    """
    Plots analog data in a histogram, representing the frequency of ranges of values.

    Parameters
    ----------
    histo_data : str
        The file name to the analog data.
    """

    distance_data = np.array(convert_dataset(histo_data), dtype=float)

    plt.figure()

    plt.hist(distance_data)

    plt.title("Frequency of Distances")
    plt.xlabel("Distance (in)")
    plt.ylabel("Frequency of Distance")

    plt.show()


def plot_data_in_spherical(
    analog_dataset: str, pan_dataset: str, tilt_dataset: str
) -> None:
    """
    Plots the data in spherical coordinates.

    Takes three different datasets representing the analog data,
    pan data and tilt data and plots it spherically.

    Parameters
    ----------
    analog_dataset : str
        The filepath to the analog data.
    pan_dataset : str
        The filepath to the pan servo data.
    tilt_dataset : str
        The filepath to the tilt data.
    """
    # The optimal distance away from the Servo, to cut out unnecessary data.
    opt_distance: int = 10

    # Gathers data from several different sources,
    # each of them representing the different parts of plotting spherically.
    distance_data = np.array(convert_dataset(analog_dataset), dtype=float)  # radius
    servo_pan_data = np.deg2rad(
        np.array(gather_servo_data_from_file(pan_dataset), dtype=float) - 20
    )  # azimuthal
    servo_tilt_data = np.deg2rad(
        np.array(gather_servo_data_from_file(tilt_dataset), dtype=float) - 20
    )  # polar

    fig = plt.figure()
    axis = fig.add_subplot(projection="3d")  # 3D plot

    # y = (distance_data) * np.cos(servo_tilt_data) * np.cos(servo_pan_data)
    # x = (distance_data) * np.cos(servo_tilt_data) * np.sin(servo_pan_data) * -1
    # z = (distance_data) * np.sin(servo_tilt_data)

    # Cuts the data to only include the data that is within the optimal distance.
    new_servo_pan_data = servo_pan_data[distance_data <= opt_distance]
    new_servo_tilt_data = servo_tilt_data[distance_data <= opt_distance]
    new_distance_data = distance_data[distance_data <= opt_distance]

    # z = (new_distance_data) * np.cos(new_servo_tilt_data) * np.cos(new_servo_pan_data)
    # x = (
    #     (new_distance_data)
    #     * np.cos(new_servo_tilt_data)
    #     * np.sin(new_servo_pan_data)
    #     * -1
    # )
    # y = (new_distance_data) * np.sin(new_servo_tilt_data)
    axis.scatter(
        new_servo_pan_data * -1,  # X coordinates are flipped, flip by multiplying by -1
        new_distance_data,
        new_servo_tilt_data,
        c=new_distance_data,  # color map for the data, based on the distance from the sensor
    )
    # axis.scatter(x, y, z, c=y)
    axis.set_title("Distance Sensor Visualization")
    axis.set_xlabel("pan (x, rad)")
    axis.set_ylabel("distance (y, in)")
    axis.set_zlabel("tilt (z, rad)")

    plt.show()


if __name__ == "__main__":
    # new_data = convert_dataset("data/distance_sensor_data.txt").tolist()
    # # print(new_data)
    # with open("data/distance_data.txt", "w+", encoding="utf-8") as file:
    #     file.write(str(new_data))
    plot_data_in_spherical(
        os.path.join("data", "distance_sensor_data.txt"),
        os.path.join("data", "servo_pan_data.txt"),
        os.path.join("data", "servo_tilt_data.txt"),
    )
    # plot_histogram(os.path.join("data", "distance_sensor_data.txt"))
