static char *addr_to_string(const char *format,
                            struct sockaddr_storage *sa,
                            socklen_t salen)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int err;

    if ((err = getnameinfo((struct sockaddr *)sa, salen,
                           host, sizeof(host),
                           serv, sizeof(serv),
                           NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
        spice_warning("Cannot resolve address %d: %s",
                      err, gai_strerror(err));
        return NULL;
    }

    return g_strdup_printf(format, host, serv);
}