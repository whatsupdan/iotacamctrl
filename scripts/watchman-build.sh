#!/usr/bin/env sh

watchman_build {

  if [[ -z $VIRTUAL_ENV ]]; then
    if [[ -d "venv" ]]; then
      source venv/bin/activate
    else
      printf "Error: Unable to determine python virtualenv"
      return
    fi
  fi

    run="make build"
    watchman-make \
        -p 'include/**/*.h' 'lib/*' 'cmake/*.cmake' 'src/*.cpp' 'src/*.h' 'src/**/*.cpp' 'src/**/*.h' \
        --make=$run
#        -t "$args"
}




watchman_build












