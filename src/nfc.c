#include "nfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static nfc_context *global_context = NULL;

nfc_device* nfc_init_device() {
    nfc_device *device = NULL;
    
    nfc_init(&global_context);
    if (!global_context) {
        printf("Error al inicializar\n");
        return NULL;
    }
    printf("libnfc OK\n");
    
    device = nfc_open(global_context, NULL);
    if (!device) {
        printf("Error al abrir dispositivo\n");
        nfc_exit(global_context);
        return NULL;
    }
    printf("Dispositivo OK: %s\n", nfc_device_get_name(device));
    
    if (nfc_initiator_init(device) < 0) {
        printf("Error al iniciar lector\n");
        nfc_close(device);
        nfc_exit(global_context);
        return NULL;
    }
    printf("Lector OK\nAcerca una tarjeta...\n");
    
    return device;
}

char* nfc_read_uid(nfc_device *device) {
    nfc_target target;
    const nfc_modulation mod = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
    };
    
    printf("Esperando tarjeta...\n");
    
    int r = nfc_initiator_select_passive_target(device, mod, NULL, 0, &target);
    
    if (r <= 0) {
        return NULL;
    }
    
    char *uid = (char*)malloc(32);
    int offset = 0;
    
    for (int i = 0; i < target.nti.nai.szUidLen; i++) {
        offset += sprintf(uid + offset, "%02X", target.nti.nai.abtUid[i]);
    }
    
    printf("Tarjeta leída!\n");
    printf("UID: %s\n", uid);
    
    return uid;
}

void nfc_cleanup(nfc_context *ctx, nfc_device *dev) {
    if (dev) nfc_close(dev);
    if (global_context) nfc_exit(global_context);
}