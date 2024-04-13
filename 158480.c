smtp_data_timeout_exit(void)
{
log_write(L_lost_incoming_connection,
  LOG_MAIN, "SMTP data timeout (message abandoned) on connection from %s F=<%s>",
  sender_fullhost ? sender_fullhost : US"local process", sender_address);
receive_bomb_out(US"data-timeout", US"SMTP incoming data timeout");
/* Does not return */
}