consider_plaintext_ports(entry_connection_t *conn, uint16_t port)
{
  const or_options_t *options = get_options();
  int reject = smartlist_contains_int_as_string(
                                     options->RejectPlaintextPorts, port);

  if (smartlist_contains_int_as_string(options->WarnPlaintextPorts, port)) {
    log_warn(LD_APP, "Application request to port %d: this port is "
             "commonly used for unencrypted protocols. Please make sure "
             "you don't send anything you would mind the rest of the "
             "Internet reading!%s", port, reject ? " Closing." : "");
    control_event_client_status(LOG_WARN, "DANGEROUS_PORT PORT=%d RESULT=%s",
                                port, reject ? "REJECT" : "WARN");
  }

  if (reject) {
    log_info(LD_APP, "Port %d listed in RejectPlaintextPorts. Closing.", port);
    connection_mark_unattached_ap(conn, END_STREAM_REASON_ENTRYPOLICY);
    return -1;
  }

  return 0;
}