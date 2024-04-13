static rdpPrinterDriver* printer_load_backend(const char* backend)
{
	typedef rdpPrinterDriver* (*backend_load_t)(void);
	union {
		PVIRTUALCHANNELENTRY entry;
		backend_load_t backend;
	} fktconv;

	fktconv.entry = freerdp_load_channel_addin_entry("printer", backend, NULL, 0);
	if (!fktconv.entry)
		return NULL;

	return fktconv.backend();
}