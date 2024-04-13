on_retrieved_unix_uid_pid (GObject              *src,
			   GAsyncResult         *res,
			   gpointer              user_data)
{
  AsyncGetBusNameCredsData *data = user_data;
  GVariant *v;

  v = g_dbus_connection_call_finish ((GDBusConnection*)src, res,
				     data->caught_error ? NULL : data->error);
  if (!v)
    {
      data->caught_error = TRUE;
    }
  else
    {
      guint32 value;
      g_variant_get (v, "(u)", &value);
      g_variant_unref (v);
      if (!data->retrieved_uid)
	{
	  data->retrieved_uid = TRUE;
	  data->uid = value;
	}
      else
	{
	  g_assert (!data->retrieved_pid);
	  data->retrieved_pid = TRUE;
	  data->pid = value;
	}
    }
}