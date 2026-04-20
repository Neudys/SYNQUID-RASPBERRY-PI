#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "nfc.h"

int main() {
    // Inicializar el lector
    nfc_device *device = nfc_init_device();
    
    if (!device) {
        printf("No se pudo inicializar NFC\n");
        return 1;
    }
    
    // Leer 3 tarjetas para probar
    for (int i = 0; i < 3; i++) {
        char *uid = nfc_read_uid(device);
        
        if (uid) {
            printf("Tarjeta %d: %s\n", i + 1, uid);
            free(uid);  // IMPORTANTE: liberar la memoria
        }
        
        printf("Esperando...\n");
        usleep(500000);  // Espera 500ms
    }
    
    // Limpiar cuando terminas
    nfc_cleanup(NULL, device);
    
    printf("Listo!\n");
    return 0;
}