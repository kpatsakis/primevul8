compute_retry_timeout(entry_connection_t *conn)
{
  int timeout = get_options()->CircuitStreamTimeout;
  if (timeout) /* if our config options override the default, use them */
    return timeout;
  if (conn->num_socks_retries < 2) /* try 0 and try 1 */
    return 10;
  return 15;
}