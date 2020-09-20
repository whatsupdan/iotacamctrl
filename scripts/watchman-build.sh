#!/usr/bin/env sh

watchman_build() {

  if [ -z "$VIRTUAL_ENV" ]; then
    if [ -d "venv" ]; then
      . venv/bin/activate
    else
      printf "Error: Unable to determine python virtualenv"
      return
    fi
  fi

    watchman-make \
        -p 'include/**/*.h' 'lib/*' 'cmake/*.cmake' 'src/*.cpp' 'src/*.h' 'src/**/*.cpp' 'src/**/*.h' \
        -t install
}

watchman_build












