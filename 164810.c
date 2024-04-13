get_unique_stream_id_by_circ(origin_circuit_t *circ)
{
  edge_connection_t *tmpconn;
  streamid_t test_stream_id;
  uint32_t attempts=0;

 again:
  test_stream_id = circ->next_stream_id++;
  if (++attempts > 1<<16) {
    /* Make sure we don't loop forever if all stream_id's are used. */
    log_warn(LD_APP,"No unused stream IDs. Failing.");
    return 0;
  }
  if (test_stream_id == 0)
    goto again;
  for (tmpconn = circ->p_streams; tmpconn; tmpconn=tmpconn->next_stream)
    if (tmpconn->stream_id == test_stream_id)
      goto again;

  if (connection_half_edge_find_stream_id(circ->half_streams,
                                           test_stream_id))
    goto again;

  return test_stream_id;
}