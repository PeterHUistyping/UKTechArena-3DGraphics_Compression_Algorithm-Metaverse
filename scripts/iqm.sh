#!/bin/bash

input_file_path=''
decoded_file_path=''

print_usage() {
  printf "Usage: ..."
}

while getopts 'i:d:v' flag; do
  case "${flag}" in
    i) input_file_path="${OPTARG}" ;;
    d) decoded_file_path="${OPTARG}" ;;
    *) print_usage
       exit 1 ;;
  esac
done

input_render_file_path="$input_file_path.png"
decoded_render_file_path="$decoded_file_path.png"

python3 ./scripts/offscreen_render.py -i $input_file_path -o $input_render_file_path 2> /dev/null
python3 ./scripts/offscreen_render.py -i $decoded_file_path -o $decoded_render_file_path 2> /dev/null

python3 ./scripts/iqm.py -r $input_render_file_path -m $decoded_render_file_path
