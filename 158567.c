lmtp_transport_init(transport_instance *tblock)
{
lmtp_transport_options_block *ob =
  (lmtp_transport_options_block *)(tblock->options_block);

/* Either the command field or the socket field must be set */

if ((ob->cmd == NULL) == (ob->skt == NULL))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "one (and only one) of command or socket must be set for the %s transport",
    tblock->name);

/* If a fixed uid field is set, then a gid field must also be set. */

if (tblock->uid_set && !tblock->gid_set && tblock->expand_gid == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "user set without group for the %s transport", tblock->name);

/* Set up the bitwise options for transport_write_message from the various
driver options. Only one of body_only and headers_only can be set. */

ob->options |=
  (tblock->body_only? topt_no_headers : 0) |
  (tblock->headers_only? topt_no_body : 0) |
  (tblock->return_path_add? topt_add_return_path : 0) |
  (tblock->delivery_date_add? topt_add_delivery_date : 0) |
  (tblock->envelope_to_add? topt_add_envelope_to : 0) |
  topt_use_crlf | topt_end_dot;
}