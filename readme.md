# Octahedron (ESP32-S3)

Projet C++ pour **ESP32-S3** avec **affichage TFT**, **Bluetooth (BLE)** et **système de fichiers (MicroSD)**.
contrôleurs séparés (BLE / SD / TFT), gestion de paquets avec **CRC32**, et fonction `error()` pour redémarrer l’ESP en cas d’erreur.

## Caractéristiques

* **Contrôleurs dédiés** : `bluetooth_controller`, `file_controller`, `tft_controller`
* **Affichage** : `printLogo` au boot (remplace l’ancien “Hello World” dans `loop`)
* **Fichiers** : opérations de lecture/écriture (ex. SD/Flash) via `file_controller.cpp`
* **Communication** : gestion des paquets dans `packet_handler.cpp` (CRC32)
* **Plateforme** : ESP32 Arduino **3.3.0**

## Prérequis

* Windows + **Visual Studio 2022**
* Extension **Visual Micro**
* **ESP32 Arduino Core 3.3.0**
* Carte **ESP32-S3 Dev Module** (N16R8), écran **TFT SPI**, (optionnel) module **SD**

## Démarrage rapide

1. Cloner le dépôt et ouvrir `Octahedron.sln` dans Visual Studio.
2. Dans la toolbar Visual Micro :

   * **Board** : `ESP32S3 Dev Module (esp32_esp32s3)`
   * **Core** : `ESP32 3.3.0`
   * **Port** : COM de la carte
3. **Build** puis **Upload**. Au démarrage, l’écran affiche le logo via `printLogo`.

## Structure

```
Octahedron/
├─ Octahedron.ino                 # point d’entrée: setup()/loop()
├─ config.h                       # constantes / configuration globale
├─ readme.md
├─ src/
│  ├─ communication/
│  │  ├─ packets/
│  │  │  ├─ file_display.h       # paquet: afficher un fichier (TFT)
│  │  │  ├─ file_exists.h        # paquet: vérification d’existence
│  │  │  ├─ file_start_write.h   # paquet: début d’écriture de fichier
│  │  │  └─ health_status.h      # paquet: statut/health
│  │  ├─ packet_base.h           # base commune des paquets
│  │  ├─ packet_defs.h           # définitions/ids de paquets
│  │  ├─ packet_handler.cpp      # logique de parsing/dispatch (CRC32, etc.)
│  │  └─ packet_handler.h
│  ├─ bluetooth_controller.cpp
│  ├─ bluetooth_controller.h     # contrôleur Bluetooth (RX/processing)
│  ├─ file_controller.cpp
│  ├─ file_controller.h          # I/O fichiers (SD/flash)
│  ├─ tft_controller.cpp
│  └─ tft_controller.h           # affichage TFT (init, printLogo, rendu)
└─ 

```
## Objectif

Ce code C++ fait partie d’un projet **ESP32-S3 + application Android**.
L’objectif est de **récupérer, afficher et archiver sur carte SD** les pochettes (“cover art”) des morceaux écoutés sur le téléphone.

### Ce que le module ESP32 fait

* **Réception depuis l’app Android (Bluetooth)** : images de couverture envoyés sous forme de paquets (240 paquets).
* **Affichage sur TFT** : le logo s’affiche au démarrage, puis la pochette du morceau courant.
* **Enregistrement sur stockage** : les images reçues sont **écrites sur la SD**, avec un flux de paquets `start → write → end`.
* **Intégrité & robustesse** : chaque paquet est **validé par CRC32** ; en cas d’erreur critique, la fonction `error()` redémarre proprement l’ESP.

### Périmètre (côté firmware)

* **Contrôleurs dédiés** :
  `bluetooth_controller` (transport BLE),
  `file_controller` (écriture/lecture fichiers),
  `tft_controller` (init & rendu).
* **Gestion de paquets** : déclarations dans `communication/packets/`, traitement dans `packet_handler.cpp`.
* **Flux type** :

  1. l’app Android envoie les métadonnées,
  2. transfert de l’image par chunks,
  3. validation CRC32, écriture sur SD,
  4. **affichage immédiat** sur le TFT. (env 0.4 s pour l'affichage d’une image 240x240))

## Volet technique — Réception d’images

* **Format d’image** : **RGB565** (16 bits/pixel)
  Chaque pixel = 2 octets : `rrrrr gggggg bbbbb` (5/6/5 bits). donc 240*240*2 = 115200 octets pour une image complète.

* **Découpage en paquets** : image envoyée **ligne par ligne**
  **240 paquets au total** → **1 paquet = 1 ligne** de l’écran.

* **Rendu TFT** (écran 240×240)

  * Envoi de la ligne : **480 octets** (240 pixels × 2 octets)
  * Aucune conversion nécessaire (RGB565 → RGB565).

* **Robustesse**

  * Traiter les paquets **dans l’ordre** (`lineIndex` croissant).
  * Si **CRC invalide** ou ligne manquante : demander **retransmission** ou appeler `error()` si irrécupérable.
