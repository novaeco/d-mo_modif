# UI Module Layout

La logique de l'interface est désormais découpée par fonctionnalité :

- `login.c` : écran d'authentification et gestion des entrées utilisateur.
- `create_user.c` : création de compte et visibilité du mot de passe.
- `main_screen.c` : objets de l'écran principal (navigation).
- `rs485.c` : navigation et callbacks pour la configuration RS485.
- `can.c` : gestion de l'écran CAN et des champs de messages.
- `pwm.c` : contrôle du PWM, arcs, sliders et bascule rapide.
- `wifi.c` : configuration Wi‑Fi, AP et liste des réseaux.
- `ui.c` : initialisation générale et jeux d'images partagés.

Les fichiers générés par SquareLine Studio restent présents dans `screens/`, tandis que `ui_helpers.c` et `ui_events.c` offrent des utilitaires communs.
