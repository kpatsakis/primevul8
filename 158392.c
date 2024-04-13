void free_devices(device_t *devices, const unsigned n_devs) {
  unsigned i;

  if (!devices)
    return;

  for (i = 0; i < n_devs; i++) {
    free(devices[i].keyHandle);
    devices[i].keyHandle = NULL;

    free(devices[i].publicKey);
    devices[i].publicKey = NULL;
  }

  free(devices);
  devices = NULL;
}