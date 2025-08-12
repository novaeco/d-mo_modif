# coverage_agent — Rôle & Procédures

## Mission
Couverture de code.

## Périmètre
gcov/lcov (si support off-target).

## Livrables
- Rapports de couverture.

## Étapes d’exécution (standard)
1. **Préparer l’environnement** : utiliser les scripts/Makefile existants (`bash scripts/build.sh`, `make build`) si applicable.
2. **Appliquer les actions du périmètre** en respectant `AGENTS.md` (racine) et les conventions de commit (pas d’amend).
3. **Valider** par build/tests/analyses selon la section *Checks*.
4. **Rédiger le rapport** (résumé, changements, impact, commandes exécutées, citations de fichiers/terminal).
5. **Assurer état Git propre** (`git status --short` vide) avant de terminer.

## Checks (obligatoires)
- Seuils atteints ; pas de baisse non justifiée.

## Citations dans la réponse finale
- Fichier : `F:path/to/file†Lstart(-Lend)?`
- Terminal : `chunk_id†Lstart(-Lend)?`

## Garde‑fous
- Ne créer **aucune nouvelle branche**.
- Ne **pas** amender des commits existants.
- Ne pas introduire de secrets/clefs/certificats.
- Respecter `.clang-format`, `.editorconfig`, CI YAML et scripts standards.
