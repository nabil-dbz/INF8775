#! /usr/bin/env bash

display_results=0
input_file_name=

while getopts hpte:a:c: opt; do
  case $opt in
    p) display_results=1 ;;
    e) input_file_name=$OPTARG ;;
    *)
      echo "ERROR: Please make sure to use the right options."
      exit 1
  esac
done

if [ -z "$input_file_name" ] then
  echo "ERRO: Missing required input: <file-name>"
  exit 1
fi

if ! [[ -f "$input_file_name" ]]; then
  echo "The provided file path $input_file_name doesn't exist."
  exit 1
fi

./runner "$input_file_name" "$display_results"
