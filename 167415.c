static void hso_serial_ref_free(struct kref *ref)
{
	struct hso_device *hso_dev = container_of(ref, struct hso_device, ref);

	hso_free_serial_device(hso_dev);
}