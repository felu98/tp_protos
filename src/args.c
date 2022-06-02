#include "../include/args.h"

static unsigned short port(const char *s) {
  char *end = 0;
  const long sl = strtol(s, &end, 10);

  if (end == s || '\0' != *end ||
      ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) || sl < 0 || sl > USHRT_MAX) {
    log(FATAL, "port should in in the range of 1-65536: %s\n", s);
    return 1;
  }
  return (unsigned short)sl;
}

static void user(char *s, struct users *user) {
  char *p = strchr(s, ':');
  if (p == NULL) {
    log(FATAL, "password not found\n");
  } else {
    *p = 0;
    p++;
    user->name = s;
    user->pass = p;
  }
}

static void version(void) {
  fprintf(stderr, "socks5v version 0.0\n"
                  "ITBA Protocolos de Comunicación 2022/1 -- Grupo 6\n"
                  "AQUI VA LA LICENCIA\n");
}

static void usage(const char *progname) {
  fprintf(
      stderr,
      "Usage: %s [OPTION]...\n"
      "\n"
      "   -h               Imprime la ayuda y termina.\n"
      "   -l <SOCKS addr>  Dirección donde servirá el proxy SOCKS.\n"
      "   -L <conf  addr>  Dirección donde servirá el servicio de management.\n"
      "   -p <SOCKS port>  Puerto entrante conexiones SOCKS.\n"
      "   -P <conf port>   Puerto entrante conexiones configuracion\n"
      "   -u <name>:<pass> Usuario y contraseña de usuario que puede usar el "
      "proxy. Hasta 10.\n"
      "   -v               Imprime información sobre la versión versión y "
      "termina.\n"
      "\n",
      progname);
  exit(0);
}

void parse_args(const int argc,  char **argv, struct socks5args *args) {
  memset(args, 0,sizeof(*args)); // sobre todo para setear en null los punteros de users

  args->socks_addr = "0.0.0.0";
  args->socks_port = 1080;

  args->mng_addr = "127.0.0.1";
  args->mng_port = 8080;

  args->disectors_enabled = true;

  int c;
  int nusers = 0;

  while ((c = getopt(argc, argv, "hl:L:Np:P:u:v" )) != -1) {

    switch (c) {
    case 'h':
      usage(argv[0]);
      break;
    case 'l':
      args->socks_addr = optarg;
      break;
    case 'L':
      args->mng_addr = optarg;
      break;
    case 'N':
      args->disectors_enabled = false;
      break;
    case 'p':
      args->socks_port = port(optarg);
      break;
    case 'P':
      args->mng_port = port(optarg);
      break;
    case 'u':
      if (nusers >= MAX_USERS) {
        log(FATAL,"maximun number of command line users reached: %d.\n",MAX_USERS );
      } else {
        user(optarg, args->users + nusers);
        nusers++;
      }
      break;
    case 'v':
      version();
      exit(0);
      break;
    default:
      log(FATAL,"unknown argument %d.\n", c);
    }
  }

  if (optind < argc) {
      log(ERROR,"argument not accepted: ");
    while (optind < argc) {
      log(ERROR,"%s ", argv[optind++]);
    }
    exit(1);
  }
}
