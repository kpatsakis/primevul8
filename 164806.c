connection_entry_set_controller_wait(entry_connection_t *conn)
{
  CONNECTION_AP_EXPECT_NONPENDING(conn);
  ENTRY_TO_CONN(conn)->state = AP_CONN_STATE_CONTROLLER_WAIT;
  control_event_stream_status(conn, STREAM_EVENT_CONTROLLER_WAIT, 0);
}