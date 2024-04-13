polkit_system_bus_name_get_process_sync (PolkitSystemBusName  *system_bus_name,
                                         GCancellable         *cancellable,
                                         GError              **error)
{
  PolkitSubject *ret = NULL;
  guint32 pid;
  guint32 uid;

  g_return_val_if_fail (POLKIT_IS_SYSTEM_BUS_NAME (system_bus_name), NULL);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (!polkit_system_bus_name_get_creds_sync (system_bus_name, &uid, &pid,
					      cancellable, error))
    goto out;

  ret = polkit_unix_process_new_for_owner (pid, 0, uid);

 out:
  return ret;
}