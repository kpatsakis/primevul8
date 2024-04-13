impl_permission_manager_test (EphyPermissionManager *manager,
                              const char *host,
                              const char *type)
{
  g_return_val_if_fail (type != NULL && type[0] != '\0', EPHY_PERMISSION_DEFAULT);

  return (EphyPermission)0;
}