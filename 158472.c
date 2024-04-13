qualify_recipient(uschar ** recipient, uschar * smtp_cmd_data, uschar * tag)
{
int rd;
if (f.allow_unqualified_recipient || strcmpic(*recipient, US"postmaster") == 0)
  {
  DEBUG(D_receive) debug_printf("unqualified address %s accepted\n",
    *recipient);
  rd = Ustrlen(recipient) + 1;
  *recipient = rewrite_address_qualify(*recipient, TRUE);
  return rd;
  }
smtp_printf("501 %s: recipient address must contain a domain\r\n", FALSE,
  smtp_cmd_data);
log_write(L_smtp_syntax_error,
  LOG_MAIN|LOG_REJECT, "unqualified %s rejected: <%s> %s%s",
  tag, *recipient, host_and_ident(TRUE), host_lookup_msg);
return 0;
}