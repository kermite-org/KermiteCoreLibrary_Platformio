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
