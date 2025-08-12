# build_agent — Rôle & Procédures

## Mission
Assurer un **build reproductible** pour ESP32‑S3 avec ESP‑IDF v5.5, publier les **artefacts** et rapporter la **taille**.

## Étapes
1) Exporter l’IDF et définir la cible :
```bash
bash scripts/build.sh
```
ou
```bash
make set-target && make build
```
2) (optionnel) taille des composants :
```bash
make size || true
```
3) CI : s’appuyer sur `.github/workflows/ci.yml` (Docker espressif/idf:release-v5.5).

## Règles
- État Git propre à la fin.  
- Ne pas modifier de commits existants.  
- Produire un résumé avec liens artefacts si CI.
