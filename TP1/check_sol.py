#!/usr/bin/env python3

# INF8775 - Analyse et conception d'algorithmes
#   TP1
#
#   AUTEUR :
#     BURLATS, Auguste - 17 Janvier 2022
#     PEZZOLI, Gauthier - 17 Janvier 2022
#
#   RÉSUMÉ DES CHANGEMENTS :
#     01/17/2022 - Création du script
#
#   USAGE :
#     Ce script vérifie le format du fichier solution donné pour conformité avec les exigences du TP1 tel que rédigé à la session H22.
#       $ ./check_sol.py -s FICHIER_SOLUTION
#     où :
#       * "FICHIER_SOLUTION" est l'adresse du fichier solution à l'emplaire donné au script
#
#   EXEMPLES D'USAGE :
#     $ ./tp.sh -a brute -e N5_0 -p > sol_N5_0
#     $ ./check_sol.py -s sol_N5_0
#       la première ligne exécute votre programme pour trouver un solution à l'instance N5_0 et sauvegarde la sortie dans le fichier sol_N5_0
#       la seconde ligne vérifie que le fichier sol_N5_0 respect le format de solution décrit dans le sujet
#
#
#   ATTENTION:
#     Lorsque vous exécutez votre programme pour générer un solution à vérifier, n'utilisez pas le flag '-t' pour afficher le temps de calcul
#     Le script considérera alors votre solution comme invalide
#
#     Il est nécessaire de rendre ce script exécutable en utilisant chmod +x
#     Python 3.5 ou ultérieur recommandé pour lancer ce script.


import sys
import re
import math
import argparse

def is_solution_format_valid(raw_solution):
    target_pattern = r"^(\d+\s+\d+\n)+\d+\s+\d+\s*$"
    return bool(re.match(target_pattern, raw_solution))


def parse_solution(raw_solution):
    solution_data = []

    for line in raw_solution.splitlines():
        if len(line.split()) == 2 :
            solution_data.append([int(x) for x in line.split()])

    return solution_data

def check_consistency(solution):
    outpout_text = 'Solution obtenue :\n'
    for index, pair in enumerate(solution):
        if index < len(solution)-1:
            if pair[0] > solution[index+1][0] :
                return f'Erreur :\nLe point {index} ({pair}) a une abscisse plus grande que le point suivant ({solution[index+1]})\nLes points ne sont donc pas tries par abscisse croissante'
            if pair[0] == solution[index+1][0] and solution[index+1][1] == pair[1] :
                return f'Erreur :\nLe point {index} et le point {index+1} sont identiques : {pair}'
        outpout_text += f'Point {index+1} : x = {pair[0]}, h = {pair[1]}\n'
    return outpout_text


if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--solution", \
                        help="Représente la solution à vérifier", \
                        action='store', required=True, metavar='FICHIER_SOLUTION')

    args = parser.parse_args()

    with open(args.solution, 'r') as fichier :
        solution_content = fichier.read()


    
    # Check whether format is as expected, ie an even number of space-separated integers on every line 
    if not is_solution_format_valid(solution_content):
        print("Erreur : les solutions fournies en pipe à stdin ont un format non valide. Revoyez la convention discutée dans l'énoncé.", file=sys.stderr)
        print("A reçu :", file=sys.stderr)
        print(solution_content, file=sys.stderr)
        sys.exit(1)

    # Structure piped solution in memory
    resolution_data = parse_solution(solution_content)

    # Check solution' consistency
    print(check_consistency(resolution_data))
    
