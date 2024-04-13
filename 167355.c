static void hso_free_shared_int(struct hso_shared_int *mux)
{
	usb_free_urb(mux->shared_intr_urb);
	kfree(mux->shared_intr_buf);
	mutex_unlock(&mux->shared_int_lock);
	kfree(mux);
}