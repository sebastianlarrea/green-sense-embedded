#ifndef _NTP_CLIENT_H_
#define _NTP_CLIENT_H_

#include <stdint.h> 

typedef struct ntp_t_t {
    ip_addr_t ntp_server_address;
    bool dns_request_sent;
    struct udp_pcb *ntp_pcb;
} ntp_t;

static void ntp_request(ntp_t *);
static void ntp_dns_found(const char *, const ip_addr_t *, void *);
static void ntp_result(ntp_t *, uint16_t, time_t *);
static void ntp_recv(void *, struct udp_pcb *, struct pbuf *, const ip_addr_t *, uint16_t);
static ntp_t *init_ntp(void);
bool ntp_get_time(void);

extern struct tm *utc;
#endif /* _NTP_CLIENT_H_ */