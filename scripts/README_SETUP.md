# scripts/setup_codex_env.sh — Guide rapide

Ce script prépare un **environnement Codex/CI** pour ton dépôt ESP32‑S3 (ESP‑IDF v5.5) :

- Installe outils de qualité : `pre-commit`, `clang-format`, `cppcheck`, `gitleaks`  
- (Optionnel) Installe **ESP-IDF v5.5** + toolchains si tu n’es pas dans l’image Docker `espressif/idf`  
- Configure les hooks `pre-commit` si `.pre-commit-config.yaml` est présent  

## Utilisation

```bash
# Depuis la racine du repo
bash scripts/setup_codex_env.sh
```

- Sur Ubuntu/Debian, le script utilise `apt` et nécessite `sudo`.  
- Dans l’image Docker `espressif/idf:release-v5.5`, l’installation ESP-IDF est **skippée** (déjà fournie).

## Après l’installation

Pour chaque nouvelle session shell locale :

```bash
. ${IDF_PATH:-$HOME/esp/esp-idf}/export.sh
idf.py set-target esp32s3
idf.py build
```

## Dépannage

- `idf.py` introuvable → il faut **sourcer** `export.sh` (voir ci-dessus).  
- `clang-format`/`cppcheck`/`gitleaks` manquants → installe-les avec ton gestionnaire de paquets si le script ne l’a pas fait.
