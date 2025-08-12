# AGENTS.md — Orchestration multi‑agents pour `d-mo_modif`

> **Objectif :** Définir des règles claires et exécutables pour des agents (Codex/LLM) travaillant sur ce dépôt ESP‑IDF (ESP32‑S3), avec un mode **multi‑agents** coordonné et des procédures de build/test **reproductibles**.

---

## 1) Contexte projet
- **Plateforme :** ESP32‑S3 (ESP‑IDF v5.5 recommandé).
- **UI :** LVGL / LovyanGFX (selon modules).
- **Cible :** `esp32s3` (par défaut).
- **CI & dev env :** Docker `espressif/idf:release-v5.5`, Devcontainer VS Code, scripts sous `scripts/`, Makefile de confort.

**Hypothèses fixes** (à respecter sauf demande explicite) :  
- Pas d’accès réseau en runtime pour les tests CI “smoke”.  
- Les artefacts (bin/elf) sont produits sous `build/`.  
- Les dépendances IDF sont gelées par `dependencies.lock` si présent.

---

## 2) Rôles & multi‑agents
Le dépôt utilise **un AGENTS.md global** + un dossier `agents/` contenant **un fichier par agent**.  
Chaque agent **n’outrepasse pas** son périmètre et **réfère** les sections pertinentes ci‑dessous.

- `agents/build_agent.md` — Build, taille, artefacts.
- `agents/test_agent.md` — Tests smoke (LVGL init), hooks futurs.
- `agents/ui_agent.md` — UI/UX LVGL & assets, ergonomie.
- `agents/refactor_agent.md` — Refactor/qualité/performances C/C++.
- `agents/docs_agent.md` — Documentation, README, schémas.
- `agents/cmake_agent.md` — CMake/IDF component manager, dépendances.

**Orchestration recommandée :**
1. `refactor_agent` (petits remaniements sécurisés) →
2. `cmake_agent` (dépendances/projets) →
3. `build_agent` (build) →
4. `test_agent` (smoke tests) →
5. `ui_agent` (améliorations UI) →
6. `docs_agent` (doc finale/PR).

Chaque agent doit **laisser l’arbre de travail propre** et pousser un **commit** autonome si la PR est large.

---

## 3) Commandes standard (agents & humains)

### 3.1 Scripts
- Export IDF + build rapide :
  ```bash
  bash scripts/build.sh
  ```
- Clean + build + taille :
  ```bash
  bash scripts/fullclean_build.sh
  ```
- Vérifications de base (style, présence CMake) :
  ```bash
  bash scripts/checks.sh
  ```

### 3.2 Makefile (équivalents)
```make
make set-target     # idf.py set-target esp32s3
make build          # idf.py build
make fullclean      # idf.py fullclean
make size           # idf.py size-components
make checks         # style + sanity
```

### 3.3 CI GitHub Actions
- Déclenchée sur `push`/`PR` → job Docker `espressif/idf:release-v5.5` → `idf.py build` + publication artefacts.

---

## 4) Règles de contribution pour agents
- **Aucune branche nouvelle** : commit direct sur la branche de travail/PR (suivre instructions de la plateforme).  
- **Ne pas amender** des commits existants.  
- **Pré‑commit** : si configuré localement, corriger et relancer.  
- **État propre requis** : `git status --short` doit être vide avant de terminer.  
- **Citations (obligatoire en réponse finale des agents)** :
  - Fichier : `F:path/to/file†Lstart(-Lend)?`
  - Terminal : `chunk_id†Lstart(-Lend)?`
- **Pas de secrets** dans les commits/CI.  
- **Pas d’assets non libres** ajoutés sans licence claire.

---

## 5) Style & structure
- **C/C++** : respecter `.clang-format`, limiter les headers non utilisés, séparer interface/impl.  
- **CMake** : composants IDF avec `idf_component_register(...)`, `REQUIRES` minimal.  
- **Logs** : privilégier `ESP_LOG*` ; niveaux `INFO/DEBUG` uniquement dans les chemins non critiques.  
- **LVGL** : initialisation idempotente, buffers/ticks gérés dans un module dédié, pas d’allocation sauvage dans les handlers.

---

## 6) Tests (smoke)
- Initialisation minimale LVGL (sans UI complexe).  
- Vérifier la compilation des assets s’il y en a.  
- Alerter si `sdkconfig` critique manquant (couleur, PSRAM, etc.).

---

## 7) PR & Revue
- **Titre** concis, mention du module (ex. `ui:` / `build:` / `cmake:`).  
- **Corps** : résumé, changements, impact, risques, étapes de test (commandes exactes).  
- **Artefacts** : joindre logs de build/tailles si pertinents.

---

## 8) Hiérarchie AGENTS.md
- Le présent **AGENTS.md (racine)** s’applique à tout le dépôt.  
- Des fichiers `AGENTS.md` **plus profonds** (dans des sous‑dossiers) **priment localement** en cas de conflit.  
- Les **instructions explicites utilisateur/développeur** priment sur les fichiers AGENTS.md.

---

## 9) Environnement & limites
- CI/Devcontainer : **pas d’accès réseau** durant l’exécution (sauf configuration explicite).  
- Si des tests échouent pour cause réseau/dépendances : documenter la raison et proposer un script de setup offline.

---

## 10) Sécurité & conformité
- Respect des licences de tiers.  
- Pas d’introduction de dépendances natives sans justification.  
- Pas d’upload de données privées/logs sensibles.

---

## 11) Points d’entrée agents
- Lire `AGENTS.md` **racine** avant toute action.  
- Ouvrir uniquement les fichiers nécessaires.  
- S’appuyer sur `agents/*.md` pour les règles détaillées par rôle.

**Fin — AGENTS.md (global)**.
