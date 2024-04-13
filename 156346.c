kbdint_alloc(const char *devs)
{
	KbdintAuthctxt *kbdintctxt;
	Buffer b;
	int i;

	kbdintctxt = xmalloc(sizeof(KbdintAuthctxt));
	if (strcmp(devs, "") == 0) {
		buffer_init(&b);
		for (i = 0; devices[i]; i++) {
			if (buffer_len(&b) > 0)
				buffer_append(&b, ",", 1);
			buffer_append(&b, devices[i]->name,
			    strlen(devices[i]->name));
		}
		buffer_append(&b, "\0", 1);
		kbdintctxt->devices = xstrdup(buffer_ptr(&b));
		buffer_free(&b);
	} else {
		kbdintctxt->devices = xstrdup(devs);
	}
	debug("kbdint_alloc: devices '%s'", kbdintctxt->devices);
	kbdintctxt->ctxt = NULL;
	kbdintctxt->device = NULL;
	kbdintctxt->nreq = 0;

	return kbdintctxt;
}