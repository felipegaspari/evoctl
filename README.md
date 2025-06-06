# evoctl - A graphical control panel for Audient EVO audio interfaces

This is a fork of the original [evoctl project by dsharlet](https://github.com/dsharlet/evoctl), modified to use modern C++, ImGui for a graphical interface, and a Makefile-based build system that works on Linux.

`evoctl` provides a simple GUI to control the input and output volumes of Audient EVO 4 and EVO 8 audio interfaces.

![Screenshot of evoctl](./doc/screenshot.png)

## Features

- Graphical control of all input and output channel volumes.
- Input-based and output-based layout modes.
- Automatic device detection for EVO 4 and EVO 8.
- Configuration is saved according to the XDG Base Directory Specification (`~/.config/evoctl/imgui.ini`).

## Dependencies

Before you build, you need to install the following system libraries:

- `build-essential` (for `g++`, `make`, etc.)
- `pkg-config`
- `libusb-1.0-0-dev`
- `libglfw3-dev`
- `libgl1-mesa-dev`


## How to Build

1.  **Clone the repository:**
    ```sh
    git clone <your-repository-url>
    cd evoctl
    ```

2.  **Initialize the submodules:**
    This project uses submodules for ImGui, GLEW, and nlohmann/json.
    ```sh
    git submodule update --init --recursive
    ```

3.  **Build the application:**
    Simply run `make` from the project root.
    ```sh
    make
    ```
    This will compile the project and create an executable named `evoctl` in the root directory.

## How to Run

After building, you can run the application with:
```sh
./evoctl
```

### udev Rules (for non-root access)

To run `evoctl` without `sudo`, you need to grant your user permission to access the Audient EVO device. You can do this by creating a `udev` rule.

1.  Create a file at `/etc/udev/rules.d/55-audient-evo.rules`:
    ```sh
    sudo nano /etc/udev/rules.d/55-audient-evo.rules
    ```

2.  Add the following line to the file:
    ```
    SUBSYSTEM=="usb", ATTR{idVendor}=="2708", MODE="0666"
    ```

3.  Reload the udev rules for the changes to take effect:
    ```sh
    sudo udevadm control --reload-rules && sudo udevadm trigger
    ```

## Credits & Acknowledgements

- **Original Project**: This work is heavily based on the original [evoctl by dsharlet](https://github.com/dsharlet/evoctl). Thank you for the reverse-engineering work and the solid foundation.
- **Dear ImGui**: For the graphical user interface. ([ocornut/imgui](https://github.com/ocornut/imgui))
- **GLEW**: The OpenGL Extension Wrangler Library. ([nigels-com/glew](https://github.com/nigels-com/glew))
- **GLFW**: For window and input handling. ([glfw/glfw](https://github.com/glfw/glfw))
- **nlohmann/json**: For JSON support. ([nlohmann/json](https://github.com/nlohmann/json))

