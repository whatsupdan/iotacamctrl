#!/usr/bin/bash

if [[ -z $VIRTUAL_ENV ]]; then
  if [[ -d "venv" ]]; then
    source venv/bin/activate
  else
    printf "Error: Unable to determine python virtualenv"
    return
fi

pip install -e .











