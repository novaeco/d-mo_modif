#!/usr/bin/env bash
# setup_codex_env.sh — Provisionne un environnement Codex/CI pour le repo ESP-IDF (ESP32‑S3)
# Cible principale : Ubuntu/Debian (VM/runner GitHub) et compatibilité avec le conteneur espressif/idf.
# Idempotent autant que possible.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OS_ID=""
if [ -f /etc/os-release ]; then
  # shellcheck disable=SC1091
  . /etc/os-release
  OS_ID="${ID:-}"
fi

echo "[*] Repo root: ${REPO_ROOT}"
echo "[*] Detected OS ID: ${OS_ID:-unknown}"

need_cmd() {
  command -v "$1" >/dev/null 2>&1
}

apt_install() {
  sudo apt-get update -y
  sudo apt-get install -y --no-install-recommends "$@"
}

# 0) Si on est déjà dans l'image espressif/idf, on a déjà ESP-IDF et toolchains
if [ -n "${IDF_PATH:-}" ] && [[ "${IDF_PATH}" == /opt/esp/idf* ]]; then
  echo "[*] IDF_PATH détecté (${IDF_PATH}) → conteneur espressif/idf probable. On évite l'installation lourde."
  IN_IDF_CONTAINER=1
else
  IN_IDF_CONTAINER=0
fi

# 1) Paquets de base (Ubuntu/Debian)
if [ "${IN_IDF_CONTAINER}" -eq 0 ]; then
  if [ "${OS_ID}" = "ubuntu" ] || [ "${OS_ID}" = "debian" ] || [ -f /etc/debian_version ]; then
    echo "[*] Installation des dépendances système de base…"
    apt_install git curl ca-certificates build-essential cmake ninja-build ccache \
                python3 python3-pip python3-venv \
                pkg-config libffi-dev libssl-dev \
                dfu-util libusb-1.0-0 \
                flex bison gperf \
                wget unzip xz-utils
  else
    echo "[!] OS non Debian-like détecté. Installe manuellement les équivalents (cmake, ninja, python3, pip, git, libssl-dev, libffi-dev, dfu-util, libusb-1.0-0, flex, bison, gperf, ccache, wget, unzip, xz-utils)."
  fi
fi

# 2) Pré-commit, clang-format, cppcheck, gitleaks
echo "[*] Outils qualité…"
python3 -m pip install --user --upgrade pip pre-commit

if ! need_cmd clang-format; then
  if [ "${IN_IDF_CONTAINER}" -eq 0 ] && ([ "${OS_ID}" = "ubuntu" ] || [ "${OS_ID}" = "debian" ]); then
    apt_install clang-format
  else
    echo "[!] clang-format introuvable : installe-le via ton gestionnaire de paquets."
  fi
fi

if ! need_cmd cppcheck; then
  if [ "${IN_IDF_CONTAINER}" -eq 0 ] && ([ "${OS_ID}" = "ubuntu" ] || [ "${OS_ID}" = "debian" ]); then
    apt_install cppcheck
  else
    echo "[!] cppcheck introuvable : installe-le via ton gestionnaire de paquets."
  fi
fi

if ! need_cmd gitleaks; then
  if [ "${IN_IDF_CONTAINER}" -eq 0 ] && ([ "${OS_ID}" = "ubuntu" ] || [ "${OS_ID}" = "debian" ]); then
    curl -sSfL https://raw.githubusercontent.com/gitleaks/gitleaks/master/install.sh | sudo bash -s -- -b /usr/local/bin
  else
    echo "[!] gitleaks introuvable : installe-le depuis https://github.com/gitleaks/gitleaks/releases"
  fi
fi

# 3) ESP-IDF (si hors conteneur) — v5.5 stable
ESP_IDF_VERSION="${ESP_IDF_VERSION:-v5.5}"
ESP_SETUP_DIR="${HOME}/esp"
if [ "${IN_IDF_CONTAINER}" -eq 0 ]; then
  mkdir -p "${ESP_SETUP_DIR}"
  if [ ! -d "${ESP_SETUP_DIR}/esp-idf" ]; then
    echo "[*] Clonage ESP-IDF ${ESP_IDF_VERSION}…"
    git clone --depth 1 --branch "${ESP_IDF_VERSION}" https://github.com/espressif/esp-idf.git "${ESP_SETUP_DIR}/esp-idf"
  else
    echo "[*] ESP-IDF déjà présent : ${ESP_SETUP_DIR}/esp-idf"
  fi

  echo "[*] Installation des toolchains ESP-IDF (peut être long)…"
  bash "${ESP_SETUP_DIR}/esp-idf/install.sh" esp32s3

  echo "[*] Export des variables ESP-IDF pour la session courante…"
  # shellcheck disable=SC1090
  . "${ESP_SETUP_DIR}/esp-idf/export.sh"
else
  echo "[*] Skip installation ESP-IDF (image espressif/idf)."
fi

# 4) Pré-commit hooks du repo
if [ -f "${REPO_ROOT}/.pre-commit-config.yaml" ]; then
  echo "[*] Installation des hooks pre-commit du dépôt…"
  python3 -m pre_commit install || true
else
  echo "[*] Aucun .pre-commit-config.yaml dans le dépôt — étape ignorée."
fi

# 5) Vérifications rapides
echo "[*] Vérifications rapides…"
if need_cmd idf.py; then
  echo " - idf.py OK: $(idf.py --version || true)"
else
  echo " - [!] idf.py introuvable dans PATH de la session (tu dois sourcer export.sh : '. ${IDF_PATH}/export.sh')."
fi

if need_cmd clang-format; then echo " - clang-format OK"; else echo " - [!] clang-format manquant"; fi
if need_cmd cppcheck; then echo " - cppcheck OK"; else echo " - [!] cppcheck manquant"; fi
if need_cmd gitleaks; then echo " - gitleaks OK"; else echo " - [!] gitleaks manquant"; fi

echo "[*] Setup terminé."
echo "Astuce : pour chaque nouvelle session shell locale, exécute :"
echo "   . ${IDF_PATH:-$HOME/esp/esp-idf}/export.sh"
