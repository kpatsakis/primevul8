polkit_system_bus_name_exists_finish (PolkitSubject  *subject,
                                      GAsyncResult   *res,
                                      GError        **error)
{
  GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (res);
  gboolean ret;

  g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == polkit_system_bus_name_exists);

  ret = FALSE;

  if (g_simple_async_result_propagate_error (simple, error))
    goto out;

  ret = g_simple_async_result_get_op_res_gboolean (simple);

 out:
  return ret;
}