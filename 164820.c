attach_pending_entry_connections_cb(mainloop_event_t *ev, void *arg)
{
  (void)ev;
  (void)arg;
  connection_ap_attach_pending(0);
}