format_protocols_lines_for_vote(const networkstatus_t *v3_ns)
{
  char *recommended_relay_protocols_line = NULL;
  char *recommended_client_protocols_line = NULL;
  char *required_relay_protocols_line = NULL;
  char *required_client_protocols_line = NULL;

  recommended_relay_protocols_line =
    format_line_if_present("recommended-relay-protocols",
                           v3_ns->recommended_relay_protocols);
  recommended_client_protocols_line =
    format_line_if_present("recommended-client-protocols",
                           v3_ns->recommended_client_protocols);
  required_relay_protocols_line =
    format_line_if_present("required-relay-protocols",
                           v3_ns->required_relay_protocols);
  required_client_protocols_line =
    format_line_if_present("required-client-protocols",
                           v3_ns->required_client_protocols);

  char *result = NULL;
  tor_asprintf(&result, "%s%s%s%s",
               recommended_relay_protocols_line,
               recommended_client_protocols_line,
               required_relay_protocols_line,
               required_client_protocols_line);

  tor_free(recommended_relay_protocols_line);
  tor_free(recommended_client_protocols_line);
  tor_free(required_relay_protocols_line);
  tor_free(required_client_protocols_line);

  return result;
}