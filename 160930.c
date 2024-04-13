phone_oui_to_icon_name (const char *bdaddr)
{
	char *vendor;
	const char *ret = NULL;

	vendor = oui_to_vendor (bdaddr);
	if (vendor == NULL)
		return NULL;

	if (strstr (vendor, "Apple") != NULL)
		ret = "phone-apple-iphone";
	else if (strstr (vendor, "Samsung") != NULL)
		ret = "phone-samsung-galaxy-s";
	else if (strstr (vendor, "Google") != NULL)
		ret = "phone-google-nexus-one";
	g_free (vendor);

	return ret;
}