# 3d-scanner

A 3D Scanner, run by an Arduino.

To install the required packages for this project, we are using Poetry. To install Poetry, follow the steps in this link:

https://python-poetry.org/docs/

Once poetry is installed, in the root of the project directory, type:

```sh
poetry install
```

This should install all of the required packages and create a virtual environment for you to work with the code.

If any of the files need to be ran, include them in a

```py
if __name__ == "__main__":
    # function calls
```

block and run them in their corresponding file.