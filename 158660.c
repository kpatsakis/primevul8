d_log_interface(gstring * g)
{
if (LOGGING(incoming_interface) && LOGGING(outgoing_interface)
    && sending_ip_address)
  {
  g = string_fmt_append(g, " I=[%s]", sending_ip_address);
  if (LOGGING(outgoing_port))
    g = string_fmt_append(g, "%d", sending_port);
  }
return g;
}