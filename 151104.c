flatpak_context_devices_to_args (FlatpakContextDevices devices,
                                 FlatpakContextDevices valid,
                                 GPtrArray *args)
{
  return flatpak_context_bitmask_to_args (devices, valid, flatpak_context_devices, "--device", "--nodevice", args);
}