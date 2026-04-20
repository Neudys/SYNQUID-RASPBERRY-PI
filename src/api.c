#include "api.h"
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char BASE_URL[512];
static CURL *curl = NULL;

// Callback para guardar respuesta
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    Response *mem = (Response *)userp;
    
    char *ptr = realloc(mem->contenido, mem->longitud + realsize + 1);
    if (!ptr) {
        printf("No hay memoria para respuesta\n");
        return 0;
    }
    
    mem->contenido = ptr;
    memcpy(&(mem->contenido[mem->longitud]), contents, realsize);
    mem->longitud += realsize;
    mem->contenido[mem->longitud] = 0;
    
    return realsize;
}

// Obtiene timestamp ISO 8601
static char* get_iso_timestamp() {
    time_t now = time(NULL);
    struct tm *tm_info = gmtime(&now);
    
    char *timestamp = (char*)malloc(30);
    strftime(timestamp, 30, "%Y-%m-%dT%H:%M:%SZ", tm_info);
    
    return timestamp;
}

void api_init(const char *base_url) {
    strncpy(BASE_URL, base_url, sizeof(BASE_URL) - 1);
    curl = curl_easy_init();
    
    if (!curl) {
        printf("Error inicializando CURL\n");
    }
}

int api_send_attendance(const char *uuid, const char *module_uid) {
    if (!curl) {
        printf("API no inicializada\n");
        return 0;
    }
    
    // Obtener timestamps
    char *timestamp = get_iso_timestamp();
    
    // Construir JSON
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(payload, "uuid", uuid);
    cJSON_AddStringToObject(payload, "moduleUid", module_uid);
    cJSON_AddStringToObject(payload, "timeStampUtc", timestamp);
    cJSON_AddStringToObject(payload, "timeStampLocal", timestamp);
    cJSON_AddBoolToObject(payload, "isSynced", 1);
    
    char *json_str = cJSON_Print(payload);
    
    // Construir URL
    char url[1024];
    snprintf(url, sizeof(url), "%s/api/Attendance/Register", BASE_URL);
    
    // Preparar respuesta
    Response response = {malloc(1), 0};
    
    // Configurar CURL
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    
    // Headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // Ejecutar
    CURLcode res = curl_easy_perform(curl);
    int success = (res == CURLE_OK) ? 1 : 0;
    
    if (success) {
        printf("✓ Attendance registrado\n");
    } else {
        printf("✗ Error: %s\n", curl_easy_strerror(res));
    }
    
    // Cleanup
    curl_slist_free_all(headers);
    free(json_str);
    free(response.contenido);
    free(timestamp);
    cJSON_Delete(payload);
    curl_easy_reset(curl);
    
    return success;
}

void api_cleanup() {
    if (curl) curl_easy_cleanup(curl);
}