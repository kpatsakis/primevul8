incomplete_transaction_log(uschar *what)
{
if (sender_address == NULL ||                 /* No transaction in progress */
    !LOGGING(smtp_incomplete_transaction))
  return;

/* Build list of recipients for logging */

if (recipients_count > 0)
  {
  int i;
  raw_recipients = store_get(recipients_count * sizeof(uschar *));
  for (i = 0; i < recipients_count; i++)
    raw_recipients[i] = recipients_list[i].address;
  raw_recipients_count = recipients_count;
  }

log_write(L_smtp_incomplete_transaction, LOG_MAIN|LOG_SENDER|LOG_RECIPIENTS,
  "%s incomplete transaction (%s)", host_and_ident(TRUE), what);
}