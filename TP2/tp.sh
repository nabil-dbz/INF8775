#! /usr/bin/env bash

usage() {
	echo "
Command: assignment
  Run the Skyline Problem Algorithm.

Usage:
  ./tp.sh -e <file-name> -a <algorithm-type> [-p] [-t]

Options:
  -h    [REQUIRED] Show this screen.
  -e    [REQUIRED] Specify input file path.
  -a    [REQUIRED] Specify the type of algirthme {[glouton, progdyn, tabou]}.
  -p    [OPTIONAL] If used, the results should be printed on the screen.
  -t    [OPTIONAL] If used, the execution time should be printed on the screen.
  "
}

display_results=0
display_execution_time=0
input_file_name=
algorithm_type=

while getopts hpte:a:c: opt; do
  case $opt in
    h) 
      usage
      exit 0
      ;;
    p) display_results=1 ;;
    t) display_execution_time=1 ;;
    e) input_file_name=$OPTARG ;;
    a) algorithm_type=$OPTARG ;;
    *)
      echo "ERROR: Please make sure to use the right options."
      exit 1
  esac
done

if [ -z "$input_file_name" ] || [ -z "$algorithm_type" ]; then
  echo "ERRO: Missing one of the required inputs: <file-name> or <algorithm-type>."
  exit 1
fi

if ! [[ -f "$input_file_name" ]]; then
  echo "The provided file path $input_file_name doesn't exist."
  exit 1
fi

algo_type=
case $algorithm_type in
  "glouton") algo_type=0 ;;
  "progdyn") algo_type=1 ;;
  "tabou") algo_type=2 ;;
  *) echo "ERRO: The type of algorithme $algo_type is not recongnized." && exit 1;;
esac

./runner "$input_file_name" "$display_execution_time" "$display_results" "$algo_type"
