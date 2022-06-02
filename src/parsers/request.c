#include "../../include/parsers/request.h";

static void remaining_set (struct request_parser* p, const int n) {
  p -> bytes_read = 0;
  p -> total_bytes_to_read = n;
}

static int remaining_is_done (struct request_parser* p) {
  return p->bytes_read >= p -> total_bytes_to_read;
}

static enum request_state version (const uint8_t c, struct request_parser* p) {
  enum request_state next;

  if (c == 0x05) {
    next = request_cmd;
  } else {
    next = reques_error_unsupported_version;
  }

  return next;
}

static enum request_state cmd (const uint8_t c, struct request_parser* p) {
  p -> request -> cmd = c;
  return request_rsv;
}

static enum request_state rsv (const uint8_t c, struct request_parser* p) {
  return request_atyp;
}

static enum request_state atyp (const uint8_t c, struct request_parser* p) {
  enum request_state next;
  p -> request -> dst_addr_type = c;

  switch (p -> request -> dst_addr_type) {
    case socks_req_addrtype_ipv4:
      remaining_set(p,4);
      memset(&(p -> request -> dst_addr.ipv4), 0, sizeof(p -> request -> dst_addr.ipv4));
      p -> request -> dst_addr.ipv4.sin_family = AF_INET;
      next = request_dstaddr;
      break;
    
    case socks_req_addrtype_ipv6:
      remaining_set(p,16);
      memset(&(p -> request -> dst_addr.ipv6), 0, sizeof(p -> request -> dst_addr.ipv6));
      p -> request -> dst_addr.ipv6.sin_family = AF_INET6;
      next = request_dstaddr;
      break;
    
    case socks_req_addrtype_domain:
      next = request_dstaddr_fqdn;
      break;
    
    default:
      next = request_error_unsupported_addresstype;
      break;
  }

  return next;
}

static enum request_state dstaddr_fqdn(const uint8_t c, struct request_parser* p) {
  // el byte nos dice cuanto nos queda por leer
  remaining_set(p,c);
  // TODO este puede llegar a ser bytes_read
  // TODO no entendi que hace
  p -> request -> dst_addr.fqdn[p->total_bytes_to_read - 1] = 0; 

  return request_dstaddr;
}

static enum request_state dstaddr(const uint8_t c, struct request_parser* p) {
  enum request_state next;

  // TODO no entiendo esto
  // TODO completar
  switch (p -> request -> dst_addr_type) {
    case socks_req_addrtype_ipv4:
      // TODO completar
      // Esta linea se ve cortada en la clase
      ((uint8_t *)&(p -> request -> dst_addr.ipv4.sin_addr))[p -> bytes_read++] = c;
      break;
    case socks_req_addrtype_ipv6:
      ((uint8_t *)&(p -> request -> dst_addr.ipv6.sin6_addr))[p -> bytes_read++] = c;
      break;
    case socks_req_addrtype_domain:
      p -> request -> dst_addr.fqdn[p -> i++] = c;
      break;
  }

  if (remaining_is_done(p)) {
    remaining_set(p,2);
    p -> request -> dst_port = 0;
    next = request_dstport;
  } else {
    next = request_dstaddr;
  }

  return next;
}

static enum request_state dstport(const uint8_t c, struct request_parser* p) {
  enum request_state next;
  // TODO no entiendo que es esto
  *(((uint8_t *) &(p -> request -> dst_port)) + p -> bytes_read) = c;
  p -> bytes_read++;
  if ( p -> bytes_read >= p -> total_bytes_to_read) {
    next = request_done;
  } else {
    next = request_dstport;
  }

  return next;
}

extern void request_parser_init (struct reques_parser* p) {
  p -> state = request_version;
  memset(p -> request, 0, sizeof(*(p -> request)));
}

extern enum request_state request_state_feed (struct request_parser* p, const uint8_t c) {
  enum request_state next;

  switch (p -> state) {
    case request_version:
  }
}

extern enum request_state reques_parser_feed (struct request_parser* p, const uint8_t c) {
  enum request_state next;

  switch (p -> state) {
    case request_version:
      next = version(c,p);
      break;
    case request_cmd:
      next = cmd(c,p);
      break;
    case request_rsv:
      next = rsv(c,p);
      break;
    case request_atyp:
      next = atyp(c,p);
      break;
    case request_dstaddr_fqdn:
      next = dstaddr_fqdn(c,p);
      break;
    case request_dstaddr:
      next = dst_addr(c,p);
      break;
    case request_dstport:
      next = dst_port(c,p);
      break;
    case request_done:
    case request_error:
    case reques_error_unsupported_version:
    case request_error_unsupported_addresstype:
      next = p -> state;
      break;
    default:
      next = request_error;
      break;
  }

  return next;
}