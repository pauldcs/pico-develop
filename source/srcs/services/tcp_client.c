/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "state.h"
#include "core.h"
#include "asserts.h"

#define TCP_SERVER_IP "192.168.1.169"
#define TCP_SERVER_PORT 4242

#define TCP_PORT 4242
#define BUF_SIZE 2048

#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    uint8_t buffer[BUF_SIZE];
    int buffer_len;
    int sent_len;
    bool complete;
    int run_count;
    bool connected;
} TCP_CLIENT_T;

static void dump_bytes(const uint8_t *bptr, uint32_t len) {
    unsigned int i = 0;

    for (i = 0; i < len;) {
      if ((i & 0x0f) == 0) {
        printf("\n");
      } else if ((i & 0x07) == 0) {
        printf(" ");
      }
      printf("%02x ", bptr[i++]);
    }
    printf("\n");
}

static err_t tcp_client_close(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    err_t err = ERR_OK;
  
    if (state->tcp_pcb != NULL) {
      tcp_arg(state->tcp_pcb, NULL);
      tcp_poll(state->tcp_pcb, NULL, 0);
      tcp_sent(state->tcp_pcb, NULL);
      tcp_recv(state->tcp_pcb, NULL);
      tcp_err(state->tcp_pcb, NULL);
      err = tcp_close(state->tcp_pcb);
      if (err != ERR_OK) {
        (void)fprintf(stderr,
          "%s: error: close failed: %s, calling abort\n",
            time_string(),
            lwip_strerr(err)
        );

        tcp_abort(state->tcp_pcb);
        err = ERR_ABRT;
      }
      state->tcp_pcb = NULL;
    }
  
    return (err);
}


static err_t tcp_result(void *arg, int status) {
  TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
  state->complete = true;
  return tcp_client_close((void*)state);
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
  state->sent_len += len;

  if (state->sent_len >= BUF_SIZE) {
    state->run_count++;
    if (state->run_count >= TEST_ITERATIONS) {
      return (tcp_result(state, 0));
    }

    state->buffer_len = 0;
    state->sent_len = 0;
  }

  return (ERR_OK);
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
  TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
  if (err != ERR_OK) {
    (void)fprintf(stderr,
      "%s: error: connection failed: %s\n",
        time_string(),
        lwip_strerr(err)
    );
      
    return tcp_result(arg, err);
  }
  
  const char *data = state_get();
  size_t data_size = strlen(data) + 1;

  state->connected = true;
  (void)fprintf(stderr,
    "%s: info: sending %d bytes\n",
      time_string(),
      data_size
  );
  
  return (tcp_write(tpcb, data, data_size, TCP_WRITE_FLAG_MORE));
}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb) {
  (void)fprintf(stderr,
    "%s: info: Timed out\n",
      time_string()
  );
  
  return tcp_result(arg, -1);
}

static void tcp_client_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
      (void)fprintf(stderr,
        "%s: error: tcp_client_err: %s\n",
          time_string(),
          lwip_strerr(err)
      );
      
      tcp_result(arg, err);
    }
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;

  if (!p) {
    return tcp_result(state, -1);
  }

  cyw43_arch_lwip_check();
  
  if (p->tot_len > 0) {
    (void)fprintf(stderr,
      "%s: info: Received: %d\n", time_string(), p->tot_len
    );
      
    const uint16_t buffer_left = BUF_SIZE - state->buffer_len;
    state->buffer_len += pbuf_copy_partial(
      p,
      state->buffer + state->buffer_len,
      p->tot_len > buffer_left
        ? buffer_left
        : p->tot_len,
      0
    );
      
    tcp_recved(tpcb, p->tot_len);
  }
  
  pbuf_free(p);

  // If we have received the whole buffer, send it back to the server
  if (state->buffer_len == BUF_SIZE) {
    (void)fprintf(stderr,
      "%s: info: Sending %d bytes to server\n",
        time_string(),
        state->buffer_len
    );

    err_t err = tcp_write(tpcb, state->buffer, state->buffer_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      (void)fprintf(stderr,
        "%s: error: failed to send data: %s\n",
          time_string(),
          lwip_strerr(err)
      );
 
      return tcp_result(state, -1);
    }
  }
  return (ERR_OK);
}

static bool tcp_client_open(void *arg) {
  __return_val_if_fail__(arg, false);

  TCP_CLIENT_T *client = (TCP_CLIENT_T*)arg;

  uint8_t ip_type = IP_GET_TYPE(&client->remote_addr);
  
  (void)fprintf(stderr,
    "%s: info: Attempting new TCP connection to %s:%d\n",
    time_string(), 
    ip4addr_ntoa(&client->remote_addr),
    TCP_PORT
  );

  struct tcp_pcb *tcp_pcb = tcp_new_ip_type(ip_type);
  if (!tcp_pcb) {
    (void)fprintf(stderr,
      "%s: error: Failed to get a new protocol control block\n",
      time_string()
    );
  
    return (false);
  }

  tcp_arg(tcp_pcb, client);
  tcp_poll(tcp_pcb, tcp_client_poll, POLL_TIME_S * 2);
  tcp_sent(tcp_pcb, tcp_client_sent);
  tcp_recv(tcp_pcb, tcp_client_recv);
  tcp_err(tcp_pcb, tcp_client_err);

  client->tcp_pcb = tcp_pcb;
  client->buffer_len = 0;

  cyw43_arch_lwip_begin();
  
  err_t err = tcp_connect(
    client->tcp_pcb,
    &client->remote_addr,
    TCP_PORT,
    tcp_client_connected
  );

  cyw43_arch_lwip_end();

  if (err != ERR_OK) {
    (void)fprintf(stderr,
      "%s: error: Failed to establish a connection: %s\n",
      time_string(),
      lwip_strerr(err)
    );
    return (false);
  }

  return (true);
}

static TCP_CLIENT_T* tcp_client_init(void) {
    TCP_CLIENT_T *state = malloc(sizeof(TCP_CLIENT_T));
    if (!state) {
      (void)fprintf(stderr,
        "%s: error: Out of memory\n",
        time_string()
      );
      return (NULL);
    }
  
    (void)memset(state, 0x00, sizeof(TCP_CLIENT_T));
    
    if (!ip4addr_aton(TCP_SERVER_IP, &state->remote_addr)) {
      (void)fprintf(stderr,
        "%s: error: IP is invalid\n",
        time_string()
      );
      free(state);
      return (NULL);
    }
  
    return state;
}

void tcp_signal(void) {
    TCP_CLIENT_T *state = tcp_client_init();
    if (!state) {
        goto hell;
    }
  
    if (!tcp_client_open(state)) {
        tcp_result(state, -1);
        return;
    }
  
    while(!state->complete) {
        sleep_ms(1000);
    }
  
    free(state);
    return;
  
hell:
  free(state);
  (void)fprintf(stderr,
    "%s: error: Could not send a tcp signal\n",
    time_string()
  );
}