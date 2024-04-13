warn_if_hs_unreachable(const edge_connection_t *conn, uint8_t reason)
{
  tor_assert(conn);

  if (conn->base_.type == CONN_TYPE_EXIT &&
      connection_edge_is_rendezvous_stream(conn) &&
      (reason == END_STREAM_REASON_CONNECTREFUSED ||
       reason == END_STREAM_REASON_TIMEOUT)) {
#define WARN_FAILED_HS_CONNECTION 300
    static ratelim_t warn_limit = RATELIM_INIT(WARN_FAILED_HS_CONNECTION);
    char *m;
    if ((m = rate_limit_log(&warn_limit, approx_time()))) {
      log_warn(LD_EDGE, "Onion service connection to %s failed (%s)",
               connection_describe_peer(TO_CONN(conn)),
               stream_end_reason_to_string(reason));
      tor_free(m);
    }
  }
}