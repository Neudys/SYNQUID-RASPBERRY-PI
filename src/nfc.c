#include "nfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

nfc_device* nfc_init_device() {
    nfc_context *context = NULL;
    nfc_device *device = NULL;
    
    nfc_init(&context);
    if (!context) {
        printf("Error al inicializar\n");
        return NULL;
    }
    printf("libnfc OK\n");
    
    device = nfc_open(context, NULL);
    if (!device) {
        printf("Error al abrir dispositivo\n");
        nfc_exit(context);
        return NULL;
    }
    printf("Dispositivo OK: %s\n", nfc_device_get_name(device));
    
    if (nfc_initiator_init(device) < 0) {
        printf("Error al iniciar lector\n");
        nfc_close(device);
        nfc_exit(context);
        return NULL;
    }
    printf("Lector OK\n");
    
    return device;
}

// Esta es la función que lee un UID
char* nfc_read_uid(nfc_device *device) {
    nfc_target target;
    memset(&target, 0, sizeof(nfc_target));
    const nfc_modulation mod = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
    };
    
    printf("Acerca Tarjeta...\n");
    int r = nfc_initiator_select_passive_target(device, mod, NULL, 0, &target);
    
    if (r <= 0) {
        return NULL;
    }
    
    char *uid = (char*)malloc(32);  
    memset(uid,0,32);
    int offset = 0;
    
    for (int i = 0; i < target.nti.nai.szUidLen; i++) {
        offset += sprintf(uid + offset, "%02X", target.nti.nai.abtUid[i]);
    }
    
    sprintf(uid + offset, "-0000-0000-0000-0000000000000");
    
    printf("Tarjeta leída!\n");
    printf("UID: %s\n", uid);
    
    return uid; 
}

void nfc_cleanup(nfc_context *ctx, nfc_device *dev) {
    if (dev) nfc_close(dev);
}
