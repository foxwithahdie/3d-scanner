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

    # Cuts the data to only include the data that is within the optimal distance.
    new_servo_pan_data = servo_pan_data[distance_data <= opt_distance]
    new_servo_tilt_data = servo_tilt_data[distance_data <= opt_distance]
    new_distance_data = distance_data[distance_data <= opt_distance]

    axis.scatter(
        new_servo_pan_data * -1,  # X coordinates are flipped, flip by multiplying by -1
        new_distance_data,
        new_servo_tilt_data,
        c=new_distance_data,  # color map for the data, based on the distance from the sensor
    )
    axis.set_title("Distance Sensor Visualization")
    axis.set_xlabel("pan (x, rad)")
    axis.set_ylabel("distance (y, in)")
    axis.set_zlabel("tilt (z, rad)")

    plt.show()


def isolate_shape_plot(
    analog_dataset: str, pan_dataset: str, tilt_dataset: str
) -> None:
    # The optimal closest distance from the Servo, to cut out data that is too close.
    opt_min_distance: int = 8
    # The optimal distance away from the Servo, to cut out unnecessary data.
    opt_max_distance: int = 11

    # Gathers data from several different sources,
    # each of them representing the different parts of plotting spherically.
    distance_data = np.array(convert_dataset(analog_dataset), dtype=float)  # radius
    servo_pan_data = (
        np.array(gather_servo_data_from_file(pan_dataset), dtype=float) - 20
    )  # azimuthal
    servo_tilt_data = (
        np.array(gather_servo_data_from_file(tilt_dataset), dtype=float) - 20
    )  # polar

    plt.figure()

    distance_range = (distance_data > opt_min_distance) & (
        distance_data < opt_max_distance
    )

    new_servo_pan_data = servo_pan_data[distance_range]
    new_servo_tilt_data = servo_tilt_data[distance_range]

    plt.gca().invert_xaxis()

    plt.scatter(new_servo_pan_data, new_servo_tilt_data)
    plt.title("Plot of Relevant Points to K, Projected onto a Flat Plane")
    plt.xlabel("Pan Angle (degrees)")
    plt.ylabel("Tilt Angle (degrees)")

    plt.show()
