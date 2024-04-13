polkit_system_bus_name_get_user_sync (PolkitSystemBusName  *system_bus_name,
				      GCancellable         *cancellable,
				      GError              **error)
{
  PolkitUnixUser *ret = NULL;
  guint32 uid;

  g_return_val_if_fail (POLKIT_IS_SYSTEM_BUS_NAME (system_bus_name), NULL);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (!polkit_system_bus_name_get_creds_sync (system_bus_name, &uid, NULL,
					      cancellable, error))
    goto out;

  ret = (PolkitUnixUser*)polkit_unix_user_new (uid);

 out:
  return ret;
}