kbdint_free(KbdintAuthctxt *kbdintctxt)
{
	if (kbdintctxt->device)
		kbdint_reset_device(kbdintctxt);
	if (kbdintctxt->devices) {
		xfree(kbdintctxt->devices);
		kbdintctxt->devices = NULL;
	}
	xfree(kbdintctxt);
}