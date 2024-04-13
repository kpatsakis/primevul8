smtp_get_port(uschar *rstring, address_item *addr, int *port, uschar *msg)
{
uschar *pstring = expand_string(rstring);

if (!pstring)
  {
  addr->transport_return = PANIC;
  addr->message = string_sprintf("failed to expand \"%s\" (\"port\" option) "
    "for %s: %s", rstring, msg, expand_string_message);
  return FALSE;
  }

if (isdigit(*pstring))
  {
  uschar *end;
  *port = Ustrtol(pstring, &end, 0);
  if (end != pstring + Ustrlen(pstring))
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("invalid port number for %s: %s", msg,
      pstring);
    return FALSE;
    }
  }

else
  {
  struct servent *smtp_service = getservbyname(CS pstring, "tcp");
  if (!smtp_service)
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("TCP port \"%s\" is not defined for %s",
      pstring, msg);
    return FALSE;
    }
  *port = ntohs(smtp_service->s_port);
  }

return TRUE;
}