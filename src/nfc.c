#include "nfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>

char* generate_uuid_from_uid(const uint8_t *uid, size_t uid_len) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha_ctx;
    
    SHA1_Init(&sha_ctx);
    SHA1_Update(&sha_ctx, uid, uid_len);
    SHA1_Final(hash, &sha_ctx);
    
    // Usar los primeros 16 bytes del hash como UUID
    char *uuid = (char*)malloc(37);  // 36 chars + null
    
    // Formato: xxxxxxxx-xxxx-5xxx-yxxx-xxxxxxxxxxxx (UUID v5)
    sprintf(uuid, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            hash[0], hash[1], hash[2], hash[3],
            hash[4], hash[5],
            (hash[6] & 0x0f) | 0x50,  // Version 5
            hash[7],
            (hash[8] & 0x3f) | 0x80,  // Variant
            hash[9],
            hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]);
    
    return uuid;
}

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
    
    char *uid = generate_uuid_from_uid(target.nti.nai.abtUid, target.nti.nai.szUidLen);
    
    printf("Tarjeta leída!\n");
    printf("UID: %s\n", uid);
    
    return uid; 
}

void nfc_cleanup(nfc_context *ctx, nfc_device *dev) {
    if (dev) nfc_close(dev);
}
