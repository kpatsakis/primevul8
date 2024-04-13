gst_rtsp_connection_set_tls_interaction (GstRTSPConnection * conn,
    GTlsInteraction * interaction)
{
  GTlsInteraction *old_interaction;

  g_return_if_fail (conn != NULL);

  if (interaction)
    g_object_ref (interaction);

  old_interaction = conn->tls_interaction;
  conn->tls_interaction = interaction;

  if (old_interaction)
    g_object_unref (old_interaction);
}