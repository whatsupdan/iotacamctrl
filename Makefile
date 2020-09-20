

clean:
	rm -rf build/* iotacamctrl.egg-info

install: clean build

build:
	pip install -e .