#!/usr/bin/env bash
set -euo pipefail

echo "[*] Setup local de base (sans installation complète d'ESP-IDF)."

if command -v python3 >/dev/null 2>&1; then
  echo "[*] Python3 OK"
  if command -v pip3 >/dev/null 2>&1; then
    echo "[*] pip3 OK"
    pip3 install --user -U pre-commit
    pre-commit install || true
  else
    echo "[!] pip3 manquant (facultatif)."
  fi
else
  echo "[!] python3 manquant."
fi

if ! command -v clang-format >/dev/null 2>&1; then
  echo "[!] clang-format absent (recommandé pour formatage C/C++)."
fi

echo "[*] Terminé. Pense à sourcer export.sh d'ESP-IDF avant build."
