import os

import matplotlib.pyplot as plt
from collections.abc import Callable
import pandas as pd
import numpy as np


def calibration_data() -> dict[int, float]:
    """
    Gathers the calibration data from the Excel spreadsheet.

    Returns
    -------
    dict[int, float]
        A dictionary of the distance measurement and their voltages.
    """
    data: pd.DataFrame = pd.read_excel(os.path.join("data", "calibration_data.xlsx"))
    inches = data["distance"]
    voltage = data["voltage"]
    converted_data: dict[int, float] = {
        inches[i]: voltage[i] for i in range(len(inches))
    }

    return converted_data


def calibration_plot() -> None:
    """
    Plots the calibration data.
    """
    calibration = calibration_data()
    distances = np.array(list(calibration.keys()))[::-1]
    voltages = np.array(list(calibration.values()))[::-1]

    plt.plot(distances, voltages)
    linear = function_from_calibration()

    plt.plot(linear(voltages), voltages)
    plt.show()


def function_from_calibration() -> Callable[[list[float]], float]:
    calibration = calibration_data()
    distances = np.array(list(calibration.keys()))[::-1]
    voltages = np.array(list(calibration.values()))[::-1]

    poly = np.polyfit(voltages, distances, 1)

    linear_fit = lambda x: poly[0] * x + poly[1]

    return linear_fit


if __name__ == "__main__":
    calibration_plot()
