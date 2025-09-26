"""
Reads the collected calibration data, manipulates it and gives a strong calibration
for understanding the distance sensor data.
"""

import os
from collections.abc import Callable

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np


def calibration_data() -> dict[int, int]:
    """
    Collects the calibration data.

    Returns
    -------
    dict[int, int]
        A dictionary of the calibration data, comparing inches to its analog data measurement.
    """
    data: pd.DataFrame = pd.read_excel(os.path.join("data", "calibration_data.xlsx"))
    inches: pd.Series = data["distance"]
    analog_data: pd.Series = data["analog"]

    converted_data: dict[int, int] = {
        inches[i]: analog_data[i] for i in range(len(inches))
    }

    return converted_data


def calibration_plot() -> None:
    """
    Plots the calibration data.
    """
    calibration: dict[int, int] = calibration_data()
    distances: np.ndarray = np.array(list(calibration.keys()))[::-1][4:]
    analog_data: np.ndarray = np.array(list(calibration.values()))[::-1][4:]

    # Plots the calibration data
    plt.plot(distances, analog_data)

    quad: Callable[[np.ndarray], np.ndarray] = function_from_calibration()

    # Plots the calibration curve
    plt.plot(quad(analog_data), analog_data)

    plt.title("Calibration Curve")
    plt.xlabel("Distance (in)")
    plt.ylabel("Analog Data (0-1023, 0-5V)")
    plt.legend(["Calibration Curve", "Fitted Quadratic Curve"])

    plt.show()


def function_from_calibration() -> Callable[[np.ndarray], np.ndarray]:
    """
    Generates a function by fitting the data to a quadratic curve.

    Returns
    -------
    Callable[[np.ndarray], np.ndarray]
        The function, which takes in analog data and returns the distance
        from the sensor, in inches.
    """
    calibration: dict[int, int] = calibration_data()
    distances: np.ndarray = np.array(list(calibration.keys()))[::-1][4:]
    analog_data: np.ndarray = np.array(list(calibration.values()))[::-1][4:]

    # Fits the data to a quadratic curve
    poly: np.ndarray = np.polyfit(analog_data, distances, 2)

    quad_fit: Callable[[np.ndarray], np.ndarray] = (
        lambda x: poly[0] * x**2 + poly[1] * x + poly[2]
    )

    return quad_fit


if __name__ == "__main__":
    calibration_plot()
