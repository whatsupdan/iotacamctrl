#!/bin/bash


build {
  if [ "$1" != "-g" ] && [ -z "$VIRTUAL_ENV" ]; then
		echo "You are not in a virtual env. To install"
		echo "globally call build with -g flag."
		echo "Example: build -g"
		exit 1
	fi
}

build

