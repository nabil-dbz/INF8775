#!/usr/bin/env python3

# INF8775 - Analyse et conception d'algorithmes
#   TP2 - Box Stacking Problem
#
#   Author :
#     HAOUAS, Mohammed Najib - Oct 24th, 2020
#     BURLATS, Auguste - Feb 20th, 2022
#
#   Changelog:
#     10/26/2020 - Initial availability
#     02/20/2022 - Update the way to read solution
#
#   USAGE:
#     EN - This script verifies the given solution for consistency 
#          with class assignment requirements.
#          Requires Python 3.5 or higher.
#          ./tp.sh -a algorithm -e instance -p | ./verify_tp2.py [refcommand]

#     FR - Ce script vérifie la solution qui lui est donnée pour
#          conformité avec les exigences du TP.
#          Python 3.5 ou ultérieur exigé.
#          ./tp.sh -a algorithme -e exemplaire -p > fichier_solution
#          ./verify_tp2.py -s fichier_solution
#          ou 
#          python verify_tp2.py -s fichier_solution

import re
import argparse


def verify_candidate_stdout(candidate_file, target_pattern):
    lines = list(filter(lambda elmnt: elmnt != '', open(candidate_file, 'r').read().splitlines()))

    if len(lines) == 0:
        return 3, 0

    if not bool(re.match(target_pattern, lines[0])):
        return 1, -1
    previous_box = [int(dim) for dim in lines[0].split()]

    res_height = previous_box[0]
    for current_line in lines[1:]:
        if not bool(re.match(target_pattern, current_line)):
            return 1, -1
        
        current_box = [int(dim) for dim in current_line.split()]

        if previous_box[1] <= current_box[1] or previous_box[2] <= current_box[2]:
            return 2, -1

        res_height += current_box[0]
        previous_box = current_box

    return 0, res_height


if __name__ == "__main__":
    desired_regex_pattern = "^\\s*\\d+\\s+\\d+\\s+\\d+\\s*$"
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--solution", \
                        help="Représente la solution à vérifier", \
                        action='store', required=True, metavar='FICHIER_SOLUTION')

    args = parser.parse_args()

    ec, height = verify_candidate_stdout(args.solution , desired_regex_pattern)
    if ec == 0:
        print("OK, height = " + str(height) + ".")
    elif ec == 1:
        print("Cannot parse piped output.")
    elif ec == 2:
        print("Invalid solution.")
    elif ec == 3:
        print("Empty output.")

