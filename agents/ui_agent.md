# ui_agent — Rôle & Procédures

## Mission
Améliorer l’UI/UX (LVGL), l’ergonomie, la cohérence visuelle et la performance (buffers, ticks).

## Lignes directrices
- Initialisation LVGL unique, gestion mémoire stricte.  
- Assets : intégrer via code généré (si applicable) ou via FS, éviter les formats non supportés.  
- Respect `.clang-format` et séparation logique (widgets vs services).

## Commandes utiles
```bash
bash scripts/build.sh
```
