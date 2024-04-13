exists_in_thread_func (GSimpleAsyncResult *res,
                       GObject            *object,
                       GCancellable       *cancellable)
{
  GError *error;
  error = NULL;
  if (!polkit_system_bus_name_exists_sync (POLKIT_SUBJECT (object),
                                           cancellable,
                                           &error))
    {
      g_simple_async_result_set_from_error (res, error);
      g_error_free (error);
    }
}