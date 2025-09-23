import calibration_data
import ast
from collections.abc import Callable
import numpy as np
import matplotlib.pyplot as plt


def convert_dataset(dataset: str) -> list[float]:
    with open(dataset, "r", encoding="utf-8") as file:
        voltage_list = ast.literal_eval(file.read())
        voltage_list = np.array(voltage_list)
        voltage_list = voltage_list * (5.0 / 1024)

    voltage_conversion: Callable[[list[float]], float] = (
        calibration_data.function_from_calibration()
    )
    return voltage_conversion(voltage_list)
