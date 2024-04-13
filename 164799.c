export_hs_client_circuit_id(edge_connection_t *edge_conn,
                            hs_circuit_id_protocol_t protocol)
{
  /* We only support HAProxy right now. */
  if (protocol != HS_CIRCUIT_ID_PROTOCOL_HAPROXY)
    return;

  char *buf = NULL;
  const char dst_ipv6[] = "::1";
  /* See RFC4193 regarding fc00::/7 */
  const char src_ipv6_prefix[] = "fc00:dead:beef:4dad:";
  uint16_t dst_port = 0;
  uint16_t src_port = 1; /* default value */
  uint32_t gid = 0; /* default value */

  /* Generate a GID and source port for this client */
  if (edge_conn->on_circuit != NULL) {
    gid = TO_ORIGIN_CIRCUIT(edge_conn->on_circuit)->global_identifier;
    src_port = gid & 0x0000ffff;
  }

  /* Grab the original dest port from the hs ident */
  if (edge_conn->hs_ident) {
    dst_port = edge_conn->hs_ident->orig_virtual_port;
  }

  /* Build the string */
  tor_asprintf(&buf, "PROXY TCP6 %s:%x:%x %s %d %d\r\n",
               src_ipv6_prefix,
               gid >> 16, gid & 0x0000ffff,
               dst_ipv6, src_port, dst_port);

  connection_buf_add(buf, strlen(buf), TO_CONN(edge_conn));

  tor_free(buf);
}