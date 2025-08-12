# refactor_agent — Rôle & Procédures

## Mission
Améliorer lisibilité, structure, performance sans **changer le comportement**.

## Règles
- Petits commits atomiques.  
- Pas d’ajout de dépendances.  
- Couverture par build/test existants.

## Procédure
- Lancer `make checks` (style).  
- Refactor localisé (fonctions longues, duplication).  
- Rebuild & rapport.
