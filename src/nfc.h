#ifndef NFC_H
#define NFC_H

#include <nfc/nfc.h>

// Esta función inicializa el lector NFC
// Retorna el dispositivo listo para leer
// Si hay error, retorna NULL
nfc_device* nfc_init_device();

// Esta función lee una tarjeta
// Toma el dispositivo como parámetro
// Retorna el UID como string (ej: "A1B2C3D4")
// Malloc'd = TÚ debes hacer free() cuando termines
char* nfc_read_uid(nfc_device *device);

// Limpia los recursos cuando termines
void nfc_cleanup(nfc_context *ctx, nfc_device *dev);

#endif