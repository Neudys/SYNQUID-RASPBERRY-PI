#ifndef API_H
#define API_H

typedef struct {
    char *contenido;
    size_t longitud;
} Response;

// Inicializa la API con URL base
void api_init(const char *base_url);

// Envía asistencia. Retorna 1 si OK, 0 si falla
int api_send_attendance(const char *uuid, const char *module_uid);

// Limpia recursos
void api_cleanup();

#endif