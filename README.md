# BottleV2
![Bottle Design](images/logo.png)
**BOTTLE : Une messagerie sécurisée écrite en C. Utilise uniquement : SDL, OPENSSL, TTF.**

Cette application est une simulation de messagerie qui fonctionne grâce aux blockchains. 
Assurant ainsi la sécurité des messages transmis, l’application permet aussi la création et la gestion de comptes dont les informations confidentielles sont chiffrées.

Installation
------------

### 1. Télécharger le code


### 2. Installer les librairies (Linux):

- sudo apt-get install libsdl1.2-dev
- sudo apt-get install libsdl2-ttf-dev
- sudo apt-get install libsdl2-image-dev
- sudo apt-get install libssl-dev  

### 3. Executer le code une fois compilé:

- aller dans le dossier src
- make all
- make runServeur (lance le serveur sur le port 2000)
- make runBottle (lance Bottle en localhost sur le port 2000)

### 4. Fonctionnalités :

- Créer un nouveau compte : Vous pouvez créer un compte en écrivant votre identifiant et votre mot de passe

![Bottle Design](images/compte.jpg)

Cependant, il est seulement possible de créer 5 comptes différents.
Il n'y a pas de bouton pour supprimer un compte : il faut directement supprimer la ligne correspondante dans le fichier SaveID.log

- Se connecter :  Pour vous connecter il vous suffit de rentrer les identifiants préalablement crées. En cas d'erreur, appuyez sur le bouton retour

- Choisir un destinataire : Rentrez directement le nom du destinataire que vous souhaitez parmi les choix proposés

![Bottle Design](images/destinataire.jpg)

- Envoyer des messages : Communiquez directement avec les autres utilisateurs via la messagerie qui affichera les derniers messages de la conversation. Vous pouvez rafraichir la page en appuyant sur la flèche en bas à gauche. Si vous voulez changer de destinataire, appuyez sur le bouton retour et reconnectez-vous

![Bottle Design](images/messagerie.jpg)

Sécurité
------------

  Bottle est un projet qui a été créé pour garantir la sécurité lors de la transmission des données d'un utilisateur à un autre. Pour cela nous avons utilisé des blockchains pour garantir une sécurité maximale mais nous permettons aussi la création de comptes dont les identifiants sont stockés et cryptés. L'algorithme est aussi capable de reconnaître toute modification du fichier de sauvegarde de la blockchain et supprimera toutes les conversations précdentes : ne jouez pas aux hackers!
