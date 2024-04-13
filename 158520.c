smtp_get_interface(uschar *istring, int host_af, address_item *addr,
  uschar **interface, uschar *msg)
{
const uschar * expint;
uschar *iface;
int sep = 0;

if (!istring) return TRUE;

if (!(expint = expand_string(istring)))
  {
  if (f.expand_string_forcedfail) return TRUE;
  addr->transport_return = PANIC;
  addr->message = string_sprintf("failed to expand \"interface\" "
      "option for %s: %s", msg, expand_string_message);
  return FALSE;
  }

while (isspace(*expint)) expint++;
if (*expint == 0) return TRUE;

while ((iface = string_nextinlist(&expint, &sep, big_buffer,
          big_buffer_size)))
  {
  if (string_is_ip_address(iface, NULL) == 0)
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("\"%s\" is not a valid IP "
      "address for the \"interface\" option for %s",
      iface, msg);
    return FALSE;
    }

  if (((Ustrchr(iface, ':') == NULL)? AF_INET:AF_INET6) == host_af)
    break;
  }

if (iface) *interface = string_copy(iface);
return TRUE;
}