parse_extended_hostname(char *address, hostname_type_t *type_out)
{
  char *s;
  char *q;
  char query[HS_SERVICE_ADDR_LEN_BASE32+1];

  s = strrchr(address,'.');
  if (!s) {
    *type_out = NORMAL_HOSTNAME; /* no dot, thus normal */
    goto success;
  }
  if (!strcmp(s+1,"exit")) {
    *s = 0; /* NUL-terminate it */
    *type_out = EXIT_HOSTNAME; /* .exit */
    goto success;
  }
  if (strcmp(s+1,"onion")) {
    *type_out = NORMAL_HOSTNAME; /* neither .exit nor .onion, thus normal */
    goto success;
  }

  /* so it is .onion */
  *s = 0; /* NUL-terminate it */
  /* locate a 'sub-domain' component, in order to remove it */
  q = strrchr(address, '.');
  if (q == address) {
    *type_out = BAD_HOSTNAME;
    goto failed; /* reject sub-domain, as DNS does */
  }
  q = (NULL == q) ? address : q + 1;
  if (strlcpy(query, q, HS_SERVICE_ADDR_LEN_BASE32+1) >=
      HS_SERVICE_ADDR_LEN_BASE32+1) {
    *type_out = BAD_HOSTNAME;
    goto failed;
  }
  if (q != address) {
    memmove(address, q, strlen(q) + 1 /* also get \0 */);
  }
  /* v2 onion address check. */
  if (strlen(query) == REND_SERVICE_ID_LEN_BASE32) {
    *type_out = ONION_V2_HOSTNAME;
    if (rend_valid_v2_service_id(query)) {
      goto success;
    }
    goto failed;
  }

  /* v3 onion address check. */
  if (strlen(query) == HS_SERVICE_ADDR_LEN_BASE32) {
    *type_out = ONION_V3_HOSTNAME;
    if (hs_address_is_valid(query)) {
      goto success;
    }
    goto failed;
  }

  /* Reaching this point, nothing was recognized. */
  *type_out = BAD_HOSTNAME;
  goto failed;

 success:
  return true;
 failed:
  /* otherwise, return to previous state and return 0 */
  *s = '.';
  const bool is_onion = (*type_out == ONION_V2_HOSTNAME) ||
    (*type_out == ONION_V3_HOSTNAME);
  log_warn(LD_APP, "Invalid %shostname %s; rejecting",
           is_onion ? "onion " : "",
           safe_str_client(address));
  if (*type_out == ONION_V3_HOSTNAME) {
      *type_out = BAD_HOSTNAME;
  }
  return false;
}