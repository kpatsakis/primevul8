ephy_certificate_manager_iface_init (EphyCertificateManagerIface *iface)
{
  iface->get_certificates = impl_get_certificates;
  iface->remove_certificate = impl_remove_certificate;
  iface->import = impl_import;
}