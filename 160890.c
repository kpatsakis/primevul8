device_pair_callback (GDBusProxy         *proxy,
		      GAsyncResult       *res,
		      GSimpleAsyncResult *simple)
{
	GError *error = NULL;

	if (device1_call_pair_finish (DEVICE1(proxy), res, &error) == FALSE) {
		g_debug ("Pair() failed for %s: %s",
			 g_dbus_proxy_get_object_path (proxy),
			 error->message);
		g_simple_async_result_take_error (simple, error);
	} else {
		g_simple_async_result_set_op_res_gboolean (simple, TRUE);
	}

	g_simple_async_result_complete_in_idle (simple);

	g_object_unref (simple);
}