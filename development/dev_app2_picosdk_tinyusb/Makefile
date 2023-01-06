build:
	pio run

flash:
	pio run --target upload

clean:
	pio run --target clean

monitor:
	pio run --target monitor

write_uf2: build
	cmd /c copy .pio\build\generic\firmware.uf2 D:\firmware.uf2

debug: flash monitor

# create library symlink for development
symlink_win_create:
	cmd /c mkdir lib\kermite_core_dev
	cmd /c mklink /D lib\kermite_core_dev\src ..\..\..\..\src

symlink_win_remove:
	cmd /c rmdir lib\kermite_core_dev\src
	cmd /c rmdir lib\kermite_core_dev

