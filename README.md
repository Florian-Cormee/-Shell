# TP4 - µShell

## Questions traitées

* 1.1 - Exécution de programmes sans arguments au premier plan
* 1.2 - Exécution de programmes avec des arguments
* 1.3 - Builtin cd
* 2.1 - Commandes en arrière plan
* 2.2 - Tuyaux
* 2.3 - Redirection de stdout
* 3.1 - Chaîne de commandes
* 3.2 - Chaîne de tuyaux
* Bonus - Usage de [git](https://github.com/Florian-Cormee/-Shell)

## Compilation

Un fichier makefile est à dispostion. Pour compiler vous pouvez utiliser plusieurs cibles.

* `make build` - Construit le projet
* `make rebuild` - Force la construction intégrale du projet

## Exécution

### Sans débogueur

La compilation créer un exécutable, nommé `ushell`, à la racine du projet.

Vous pouvez l'exécuter en utilisant la command : `./ushell`

Cette commande peut être accompagnée d'un argument. Elle devient alors : `./ushell --debug`. Le programme affichera alors tous ses messages pour le débogage. Par défaut, seuls les avertissements et les erreurs sont affichés.

### Avec débogueur

Le fichier makefile propose d'exécuter le programme avec Valgrind via la commande : `make valgrind`.

Le programme peut aussi être exécuter avec GDB via la commande : `make debug`

Remarque : Ces deux commandes commencent par compiler le projet.

## Nettoyage du projet

Pour supprimer les fichier objets, utilisez la commande : `make clean`

Pour supprimer les fichier objets ainsi que l'exécutable, utilisez la commande : `make vclean`
