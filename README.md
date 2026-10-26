# IoT Device Communication System (C++ / Qt6 / Yocto / QEMU)

Client (C++, runs inside a Yocto-built QEMU image) talks to a Qt6 GUI server on the laptop
over TCP or UDP. The server checks the temperature threshold every second and sends either
"set threshold" or "get Temp".

## Structure
- `common/`  Socket, TCPSocket, UDPSocket, Channel, ServerChannel, ClientChannel, ServerLogic
- `client/`  terminal client
- `server/gui/`  Qt6 (QML) GUI server: 4 tabs (Real Time Monitor, Historical Analysis, Configuration, Quick Access)
- `server/console/`  console test server (optional)
- `yocto/meta-iotclient/`  Yocto layer: `iotclient` recipe, `iot-prompt` recipe, `iot-image`
- `video/`  demo video

## Build (host)
Requirements: g++ (C++17), make, cmake, Qt 6.8 with Quick, QuickControls2 and Graphs.
    make client console        # client_app, server_console
    make gui                   # Qt6 server -> ./server_gui (QT_PREFIX=... if Qt is not in ~/Qt/6.8.3/gcc_64)

## Run on the host
    ./server_gui tcp 5000      # or: udp 5000. Without arguments, choose the protocol in the window
    ./client_app tcp 127.0.0.1 5000

## Yocto / QEMU (Kirkstone, qemuarm64)
1. Copy `common/`, `client/` and `Makefile` into
   `yocto/meta-iotclient/recipes-iot/iotclient/files/iotclient-src/`.
2. Add the layer: `bitbake-layers add-layer <path>/yocto/meta-iotclient`
3. `bitbake iot-image`
4. Start the server GUI, then:
       runqemu qemuarm64 iot-image slirp nographic
   Log in as root and run:
       iot_client tcp 10.0.2.2 5000     # or udp
   (10.0.2.2 is the host as seen from QEMU user-mode networking.)

## Protocol
Newline-framed text messages. Server -> client: `set threshold` followed by the value, or `get Temp`.
Client -> server: the temperature value. UDP uses a HELLO/ACK handshake.
LED is ON when temperature >= threshold.

## Notes
[any assumptions or limitations]
