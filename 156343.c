kbdint_next_device(KbdintAuthctxt *kbdintctxt)
{
	size_t len;
	char *t;
	int i;

	if (kbdintctxt->device)
		kbdint_reset_device(kbdintctxt);
	do {
		len = kbdintctxt->devices ?
		    strcspn(kbdintctxt->devices, ",") : 0;

		if (len == 0)
			break;
		for (i = 0; devices[i]; i++)
			if (strncmp(kbdintctxt->devices, devices[i]->name, len) == 0)
				kbdintctxt->device = devices[i];
		t = kbdintctxt->devices;
		kbdintctxt->devices = t[len] ? xstrdup(t+len+1) : NULL;
		xfree(t);
		debug2("kbdint_next_device: devices %s", kbdintctxt->devices ?
		   kbdintctxt->devices : "<empty>");
	} while (kbdintctxt->devices && !kbdintctxt->device);

	return kbdintctxt->device ? 1 : 0;
}