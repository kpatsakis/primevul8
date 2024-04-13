subject_iface_init (PolkitSubjectIface *subject_iface)
{
  subject_iface->hash          = polkit_system_bus_name_hash;
  subject_iface->equal         = polkit_system_bus_name_equal;
  subject_iface->to_string     = polkit_system_bus_name_to_string;
  subject_iface->exists        = polkit_system_bus_name_exists;
  subject_iface->exists_finish = polkit_system_bus_name_exists_finish;
  subject_iface->exists_sync   = polkit_system_bus_name_exists_sync;
}