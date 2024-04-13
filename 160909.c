bluetooth_client_connect_service_finish (BluetoothClient *client,
					 GAsyncResult    *res,
					 GError         **error)
{
	GSimpleAsyncResult *simple;

	simple = (GSimpleAsyncResult *) res;

	g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == bluetooth_client_connect_service);

	if (g_simple_async_result_get_op_res_gboolean (simple))
		return TRUE;
	g_simple_async_result_propagate_error (simple, error);
	return FALSE;
}