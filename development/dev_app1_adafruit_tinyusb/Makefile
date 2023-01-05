build:
	pio run

flash:
	pio run --target upload

clean:
	pio run --target clean

monitor:
	pio run --target monitor

write_uf2:
	cmd /c copy .pio\build\generic\firmware.uf2 D:\firmware.uf2

debug: flash monitor

# create library symlink for development
symlink_win_create:
	cmd /c mklink /D lib\kermite_core_dev\src ..\..\..\..\src

symlink_win_remove:
	cmd /c rmdir lib\kermite_core_dev\src

symlink_mac_create:
	mkdir -p lib/kermite_core_dev
	ln -snF ../../../../src lib/kermite_core_dev/src

symlink_mac_remove:
	unlink lib/kermite_core_dev/src