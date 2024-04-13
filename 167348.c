static void hso_free_serial_device(struct hso_device *hso_dev)
{
	struct hso_serial *serial = dev2ser(hso_dev);

	if (!serial)
		return;

	hso_serial_common_free(serial);

	if (serial->shared_int) {
		mutex_lock(&serial->shared_int->shared_int_lock);
		if (--serial->shared_int->ref_count == 0)
			hso_free_shared_int(serial->shared_int);
		else
			mutex_unlock(&serial->shared_int->shared_int_lock);
	}
	hso_free_tiomget(serial);
	kfree(serial);
	kfree(hso_dev);
}