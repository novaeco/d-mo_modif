| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

| Supported LCD Controller    | ST7262 |
| ----------------------------| -------|

| Supported TOUCH Controller    | GT911 |
| ----------------------------| -------|
## How to use the example

## ESP-IDF Required

### Hardware Required

* An Waveshare ESP32-S3-Touch-LCD-4.3 development board

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```
       ESP Board                           RGB  Panel
+-----------------------+              +-------------------+
|                   GND +--------------+GND                |
|                       |              |                   |
|                   3V3 +--------------+VCC                |
|                       |              |                   |
|                   PCLK+--------------+PCLK               |
|                       |              |                   |
|             DATA[15:0]+--------------+DATA[15:0]         |
|                       |              |                   |
|                  HSYNC+--------------+HSYNC              |
|                       |              |                   |
|                  VSYNC+--------------+VSYNC              |
|                       |              |                   |
|                     DE+--------------+DE                 |
|                       |              |                   |
|               BK_LIGHT+--------------+BLK                |
       ESP Board                             TOUCH  
+-----------------------+              +-------------------+
|                    GND+--------------+GND                |
|                       |              |                   |
|                    3V3+--------------+VCC                |
|                       |              |                   |
|                  GPIO8+--------------+SDA                |
|                       |              |                   |
|                  GPIO9+--------------+SCL                |
|                       |              |                   |
       ESP Board                                LED
+-----------------------+              +-------------------+
|                   GND +--------------+GND                |
|                       |              |                   |
|                   3V3 +--------------+VCC                |
|                       |              |                   |
|                    AD +--------------+LED                |
+-----------------------+              |                   |
|                       |              |                   |
       IO EXTENSION.EXIO1+--------------+TP_RST            |
|                       |              |                   |
       IO EXTENSION.EXIO2+--------------+DISP_EN           |          
                                       +-------------------+
```

* Demonstrates an LVGL slider to control LED brightness.

### Fonctionnalités

- **Login utilisateur** : écran d’authentification et création de comptes.
- **Wi‑Fi STA/AP** : scan des réseaux, connexion ou point d’accès.
- **RS485 half‑duplex** : envoi/réception UART avec interface graphique dédiée.
- **Contrôle LED & UI LVGL** : exemple de gradation via slider.

### Static crop buffer

`waveshare_rgb_lcd_display_window` réutilise un tampon RGB565 préalloué
(`RGB_LCD_WINDOW_BUF_SIZE`, ≈1,2 Mio pour 1024×600) afin d'éviter les
allocations dynamiques à chaque appel.

### Configuration, compilation et flash

1. **Configurer l’environnement**

```bash
export IDF_PATH=~/esp-idf           # Chemin vers l’ESP‑IDF
. "${IDF_PATH}/export.sh"          # Ajoute outils et variables
export IDF_TARGET=esp32s3           # Cible par défaut
export ESPPORT=/dev/ttyUSB0         # Port série à adapter
export ESPBAUD=921600               # Vitesse de flash optionnelle
```

2. **Sélectionner la cible et paramétrer le projet**

```bash
idf.py set-target "${IDF_TARGET}"
idf.py menuconfig                  # Configuration optionnelle
```

3. **Compiler**

```bash
idf.py build
```

4. **Flasher et lancer le moniteur série**

```bash
idf.py -p "${ESPPORT}" -b "${ESPBAUD}" flash monitor
```

La première exécution d’`idf.py` peut être plus longue.

(Pour quitter le moniteur série : `Ctrl-]`).

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an https://service.waveshare.com/. We will get back to you soon.

---
## Build rapide / CI / Agents

### Docker (identique CI)
```bash
docker run --rm -it -v "$PWD:/project" -w /project espressif/idf:release-v5.5 bash -lc   '. ${IDF_PATH}/export.sh && idf.py set-target esp32s3 && idf.py build'
```

### Devcontainer / Codespaces
- Ouvrir dans VS Code → “Reopen in Container”
- Utiliser tâches : IDF: build / size / fullclean

### Local (ESP-IDF déjà installé)
```bash
. $IDF_PATH/export.sh
idf.py set-target esp32s3
idf.py build
```

### Multi-agents
- Lire `AGENTS.md` (global) et fichiers `agents/`
- Rôles : build, test, ui, refactor, docs, cmake

### Guide de test rapide

Des scénarios de test minimaux sont fournis sous `tests/` :

```bash
# LVGL : initialisation de la pile graphique
idf.py -C tests/smoke build

# Wi‑Fi : initialisation du driver
idf.py -C tests/wifi build

# RS485 : vérification de l’UART half‑duplex
idf.py -C tests/rs485 build

# CAN : tests du contrôleur CAN intégré
idf.py -C tests/can build
```

Ajouter `-p "${ESPPORT}" flash` pour flasher un test sur la carte.
