#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "ntp_client.h"
#include "hardware/sync.h"

#define NTP_SERVER "pool.ntp.org" /*<! Servidor NTP */
#define NTP_MSG_LEN 48 /*<! Tamaño del mensaje NTP */
#define NTP_PORT 123 /*<! Puerto NTP */
#define NTP_DELTA 2208988800 /*<!segundos entre 1 de Ene de 1900 and 1 de Ene de 1970 */
#define NTP_MODE_MASK 0x7 /*<! Máscara para obtener el modo del mensaje NTP */
#define NTP_MODE_SERVER 4 /*<! Modo servidor del mensaje NTP */
#define NTP_TIMEOUT 1000 /*<! Tiempo de espera para la resolución  servidor NTP */

struct tm *utc;

static void ntp_result(ntp_t *state, uint16_t status, time_t *result)
{
    /**
     * @brief Función de callback para el resultado del NTP
     * @param state Puntero al estado del NTP
     * @param status Estado del NTP
     * @param result Puntero al resultado del NTP
    */

    if (status == 0 && result) {
        utc = gmtime(result);
    }
    else {
        utc = NULL;
    }
    state->dns_request_sent = false;
}


static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    /**
     * @brief Función de callback para recibir mensajes NTP
     * @param arg Puntero al estado del NTP
     * @param pcb Puntero al PCB (protocol control block) del NTP
     * @param p Puntero al buffer de recepción
     * @param addr Puntero a la dirección IP del remitente
     * @param port Puerto del remitente
    */
    ntp_t *state = (ntp_t *)arg;
    uint8_t mode = pbuf_get_at(p, 0) & NTP_MODE_MASK;
    uint8_t stratum = pbuf_get_at(p, 1); // Estrato del servidor NTP, indica la distancia al servidor de referencia

    /*
    * Si el mensaje es un NTP valid, el estrato es distinto a cero (servidor de referencia) 
    * y el modo es servidor (4), entonces se procesa el mensaje
    */
    if(ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN
      && mode == NTP_MODE_SERVER && stratum != 0) {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40); // Copia los segundos del mensaje NTP
        uint32_t seconds_since_1900 = (seconds_buf[0] << 24) | (seconds_buf[1] << 16) | (seconds_buf[2] << 8) | seconds_buf[3]; // Convierte los segundos a un entero de 32 bits
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA; // A los segundos desde 1900 se le resta el delta para obtener los segundos desde la noche de los tiempos del 1 de enero de 1970 :)
        //El problema del 2038 se solucionará en el año 2038
        time_t epoch = seconds_since_1970;
        ntp_result(state, 0, &epoch);

    } else {
        printf("Mensaje NTP inválido\n");
        ntp_result(state, -1, NULL);
    }
    pbuf_free(p);
}

static ntp_t *init_ntp(void)
{
    /**
     * @brief Inicializa el estado del NTP
     * @note Esta función reserva memoria para el estado del NTP y crea el PCB (protocol control block) para el NTP
    */
    ntp_t *state = calloc(1, sizeof(ntp_t));

    if(!state) {
        printf("Error reservando memoria para el estado del NTP\n");
        return NULL;
    }

    state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);

    if(!state->ntp_pcb) {
        printf("Error creando el PCB para el NTP\n");
        free(state);
        return NULL;
    }

    udp_recv(state->ntp_pcb, ntp_recv, state);
    return state;
}

static void ntp_dns_found(const char *hostname, const ip_addr_t *ip_addr, void *arg)
{
    /**
     * @brief Función de callback para la resolución DNS del servidor NTP
     * @param hostname Nombre del servidor NTP
     * @param ip_addr Puntero a la dirección IP del servidor NTP
     * @param arg Puntero al estado del NTP
     * @note Esta función envía un mensaje NTP al servidor NTP
    */
    ntp_t *state = (ntp_t *)arg;
    if(ip_addr) {
        state->ntp_server_address = *ip_addr; // Copia la dirección IP del servidor NTP
        printf("Servidor NTP: %s\n", ipaddr_ntoa(&state->ntp_server_address));
        ntp_request(state);
    } else {
        printf("Error resolviendo el servidor NTP\n");
        ntp_result(state, -1, NULL);
    }
}

static void ntp_request(ntp_t *state)
{
    cyw43_arch_lwip_begin(); // Inicializa el stack TCP/IP
    
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM); // Reserva memoria para el buffer de envío
    uint8_t *req = (uint8_t *)p->payload; // Puntero al buffer de envío
    memset(req, 0, NTP_MSG_LEN); // Limpia el buffer de envío
    req[0] = 0x1b; // Versión 3, modo cliente
    udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT); // Envía el mensaje NTP
    pbuf_free(p); // Libera la memoria del buffer de envío

    cyw43_arch_lwip_end(); // Detiene el stack TCP/IP
}

bool ntp_get_time(void)
{
    absolute_time_t request_time;
    bool result = false;

    ntp_t *state = init_ntp();
    if(!state) {
        return false;
    }
    
    cyw43_arch_lwip_begin();
    int err = dns_gethostbyname(NTP_SERVER, &state->ntp_server_address, ntp_dns_found, state);
    cyw43_arch_lwip_end();

    state->dns_request_sent = true;

    if(err == ERR_OK) {
        printf("Enviar solicitud\n");
        ntp_request(state);
    } else if (err != ERR_INPROGRESS){
        printf("Error resolviendo el servidor NTP\n");
        ntp_result(state, -1, NULL);
    }

    request_time = make_timeout_time_ms(NTP_TIMEOUT);
    while(!time_reached(request_time)) {
        continue;
    }
    
    if(state->dns_request_sent) {
        printf("Timeout resolviendo el servidor NTP\n");
        ntp_result(state, -1, NULL);
        result = false;
    } else {
        printf("Servidor NTP resuelto\n");
        result = true;
    }

    free(state);

    return result;
}