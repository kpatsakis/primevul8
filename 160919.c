disconnect_callback (GDBusProxy   *proxy,
		     GAsyncResult *res,
		     GSimpleAsyncResult *simple)
{
	gboolean retval;
	GError *error = NULL;

	retval = device1_call_disconnect_finish (DEVICE1(proxy), res, &error);
	if (retval == FALSE) {
		g_debug ("Disconnect failed for %s: %s",
			 g_dbus_proxy_get_object_path (proxy),
			 error->message);
		g_simple_async_result_take_error (simple, error);
	} else {
		g_debug ("Disconnect succeeded for %s",
			 g_dbus_proxy_get_object_path (proxy));
		g_simple_async_result_set_op_res_gboolean (simple, retval);
	}

	g_simple_async_result_complete_in_idle (simple);

	g_object_unref (simple);
}