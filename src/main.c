#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "nfc.h"
#include "api.h"

int main() {

    const char *api_url = "https://dentinal-uncompounded-erma.ngrok-free.dev";
    
    api_init(api_url);
    nfc_device *device = nfc_init_device();
    
    if (!device) {
        printf("No se pudo inicializar NFC\n");
        return 1;
    }
    
    while (true) {
        char *uid = nfc_read_uid(device);
        
        if (uid) {
            printf("Tarjeta: %s\n", uid);
            api_send_attendance(uid, "b8bb5795-c1dd-4fd1-bc15-893cd95f8213");
            free(uid);
        }
        
        printf("Esperando...\n");
        sleep(1);  
    }
    
    nfc_cleanup(NULL, device);
    
    printf("Listo!\n");
    return 0;
}
