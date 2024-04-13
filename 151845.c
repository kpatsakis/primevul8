static int socket_set_cork(int socket, int enabled)
{
    SPICE_VERIFY(sizeof(enabled) == sizeof(int));
    return setsockopt(socket, IPPROTO_TCP, TCP_CORK, &enabled, sizeof(enabled));
}