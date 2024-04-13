host_is_tls_on_connect_port(int port)
{
int sep = 0;
uschar buffer[32];
const uschar *list = tls_in.on_connect_ports;
uschar *s;
uschar *end;

if (tls_in.on_connect) return TRUE;

while ((s = string_nextinlist(&list, &sep, buffer, sizeof(buffer))))
  if (Ustrtol(s, &end, 10) == port)
    return TRUE;

return FALSE;
}