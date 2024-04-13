bluetooth_client_setup_device_finish (BluetoothClient  *client,
				      GAsyncResult     *res,
				      char            **path,
				      GError          **error)
{
	GSimpleAsyncResult *simple;

	simple = (GSimpleAsyncResult *) res;

	g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == bluetooth_client_setup_device);

	if (path != NULL)
		*path = g_object_get_data (G_OBJECT (res), "device-object-path");

	if (g_simple_async_result_get_op_res_gboolean (simple))
		return TRUE;
	g_simple_async_result_propagate_error (simple, error);
	return FALSE;
}