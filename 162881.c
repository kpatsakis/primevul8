gst_rtsp_watch_reset (GstRTSPWatch * watch)
{
  g_mutex_lock (&watch->mutex);
  if (watch->readsrc) {
    g_source_remove_child_source ((GSource *) watch, watch->readsrc);
    g_source_unref (watch->readsrc);
  }
  if (watch->writesrc) {
    g_source_remove_child_source ((GSource *) watch, watch->writesrc);
    g_source_unref (watch->writesrc);
    watch->writesrc = NULL;
  }
  if (watch->controlsrc) {
    g_source_remove_child_source ((GSource *) watch, watch->controlsrc);
    g_source_unref (watch->controlsrc);
    watch->controlsrc = NULL;
  }

  if (watch->conn->input_stream) {
    watch->readsrc =
        g_pollable_input_stream_create_source (G_POLLABLE_INPUT_STREAM
        (watch->conn->input_stream), NULL);
    g_source_set_callback (watch->readsrc,
        (GSourceFunc) gst_rtsp_source_dispatch_read, watch, NULL);
    g_source_add_child_source ((GSource *) watch, watch->readsrc);
  } else {
    watch->readsrc = NULL;
  }

  /* we create and add the write source when we actually have something to
   * write */

  /* when write source is not added we add read source on the write socket
   * instead to be able to detect when client closes get channel in tunneled
   * mode */
  if (watch->conn->control_stream) {
    watch->controlsrc =
        g_pollable_input_stream_create_source (G_POLLABLE_INPUT_STREAM
        (watch->conn->control_stream), NULL);
    g_source_set_callback (watch->controlsrc,
        (GSourceFunc) gst_rtsp_source_dispatch_read_get_channel, watch, NULL);
    g_source_add_child_source ((GSource *) watch, watch->controlsrc);
  } else {
    watch->controlsrc = NULL;
  }
  g_mutex_unlock (&watch->mutex);
}