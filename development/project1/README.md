# Testbed project for KermiteCore

This project is intended to be used for development and debugging for KermiteCore.

The application is a simple keyboard with one-key and one-LED. It is used to check the developing core library works correctly.

## Configurations (`platformio.ini`)

There is 2 configuration targets in platformio.ini.
- `[env:generic]` is used for normal development.
- `[env:portable_stack]` is a special setting which utilize portable version of the hardware dependent modules.

## Configurations (`app.cpp`)

In `src/app.cpp`, there is a LED and button configurations part. please change them according to your board.


## Build and debug

To build and write the firmware to the board, execute
```
  pio run --target upload
``` 
  or simply 
```
  make flash
```

## Note

The library code is referenced by symlink. `./lib/kermite_core_dev/src` is a symbolic link to `<RepositoryRoot>/src`.
