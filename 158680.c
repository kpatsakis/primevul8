host_item_get_port(host_item *h)
{
const uschar *p;
int port, x;
int len = Ustrlen(h->name);

if (len < 3 || (p = h->name + len - 1, !isdigit(*p))) return PORT_NONE;

/* Extract potential port number */

port = *p-- - '0';
x = 10;

while (p > h->name + 1 && isdigit(*p))
  {
  port += (*p-- - '0') * x;
  x *= 10;
  }

/* The smallest value of p at this point is h->name + 1. */

if (*p != ':') return PORT_NONE;

if (p[-1] == ']' && h->name[0] == '[')
  h->name = string_copyn(h->name + 1, p - h->name - 2);
else if (Ustrchr(h->name, ':') == p)
  h->name = string_copyn(h->name, p - h->name);
else return PORT_NONE;

DEBUG(D_route|D_host_lookup) debug_printf("host=%s port=%d\n", h->name, port);
return port;
}