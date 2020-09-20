

clean:
	rm -rf build/* iotacamctrl.egg-info

install: clean build-python

build-python:
	pip install -e .