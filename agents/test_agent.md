# test_agent — Rôle & Procédures

## Mission
Exécuter des **tests smoke** minimaux (compilation et init LVGL).

## Étapes minimales
- Vérifier que la compilation passe :
```bash
bash scripts/build.sh
```
- (Option future) Ajouter des tests unitaires C (Unity/CTest) sous `tests/`.
- Signaler l’absence d’accès réseau si des dépendances externes sont requises.

## Politique
- Fail fast si `idf.py build` échoue.  
- Reporter la configuration `sdkconfig` notable si pertinente (PSRAM, couleur).
