ephy_permission_manager_iface_init (EphyPermissionManagerIface *iface)
{
  iface->add = impl_permission_manager_add;
  iface->remove = impl_permission_manager_remove;
  iface->clear = impl_permission_manager_clear;
  iface->test = impl_permission_manager_test;
  iface->list = impl_permission_manager_list;
}