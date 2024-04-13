smtp_connect(smtp_connect_args * sc, const blob * early_data)
{
int port = sc->host->port;
smtp_transport_options_block * ob = sc->ob;

callout_address = string_sprintf("[%s]:%d", sc->host->address, port);

HDEBUG(D_transport|D_acl|D_v)
  {
  uschar * s = US" ";
  if (sc->interface) s = string_sprintf(" from %s ", sc->interface);
#ifdef SUPPORT_SOCKS
  if (ob->socks_proxy) s = string_sprintf("%svia proxy ", s);
#endif
  debug_printf_indent("Connecting to %s %s%s... ", sc->host->name, callout_address, s);
  }

/* Create and connect the socket */

#ifdef SUPPORT_SOCKS
if (ob->socks_proxy)
  {
  int sock = socks_sock_connect(sc->host, sc->host_af, port, sc->interface,
				sc->tblock, ob->connect_timeout);
  
  if (sock >= 0)
    {
    if (early_data && early_data->data && early_data->len)
      if (send(sock, early_data->data, early_data->len, 0) < 0)
	{
	int save_errno = errno;
	HDEBUG(D_transport|D_acl|D_v)
	  {
	  debug_printf_indent("failed: %s", CUstrerror(save_errno));
	  if (save_errno == ETIMEDOUT)
	    debug_printf(" (timeout=%s)", readconf_printtime(ob->connect_timeout));
	  debug_printf("\n");
	  }
	(void)close(sock);
	sock = -1;
	errno = save_errno;
	}
    }
  return sock;
  }
#endif

return smtp_sock_connect(sc->host, sc->host_af, port, sc->interface,
			  sc->tblock, ob->connect_timeout, early_data);
}