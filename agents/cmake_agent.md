# cmake_agent — Rôle & Procédures

## Mission
Sanitiser le système de build : `CMakeLists.txt`, composants IDF, `dependencies.lock` si présent.

## Étapes
- Vérifier `CMakeLists.txt` racine et sous `components/*`.  
- Réduire `REQUIRES` au strict nécessaire.  
- `idf.py reconfigure` si changements.  
- Build complet pour validation (`bash scripts/fullclean_build.sh`).
