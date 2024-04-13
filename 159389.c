polkit_system_bus_name_exists (PolkitSubject       *subject,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  GSimpleAsyncResult *simple;

  g_return_if_fail (POLKIT_IS_SYSTEM_BUS_NAME (subject));

  simple = g_simple_async_result_new (G_OBJECT (subject),
                                      callback,
                                      user_data,
                                      polkit_system_bus_name_exists);
  g_simple_async_result_run_in_thread (simple,
                                       exists_in_thread_func,
                                       G_PRIORITY_DEFAULT,
                                       cancellable);
  g_object_unref (simple);
}