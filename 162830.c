gst_rtsp_connection_set_tls_database (GstRTSPConnection * conn,
    GTlsDatabase * database)
{
  GTlsDatabase *old_db;

  g_return_if_fail (conn != NULL);

  if (database)
    g_object_ref (database);

  old_db = conn->tls_database;
  conn->tls_database = database;

  if (old_db)
    g_object_unref (old_db);
}