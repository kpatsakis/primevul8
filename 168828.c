static void rdpei_print_contact_flags(UINT32 contactFlags)
{
	if (contactFlags & CONTACT_FLAG_DOWN)
		WLog_DBG(TAG, " CONTACT_FLAG_DOWN");

	if (contactFlags & CONTACT_FLAG_UPDATE)
		WLog_DBG(TAG, " CONTACT_FLAG_UPDATE");

	if (contactFlags & CONTACT_FLAG_UP)
		WLog_DBG(TAG, " CONTACT_FLAG_UP");

	if (contactFlags & CONTACT_FLAG_INRANGE)
		WLog_DBG(TAG, " CONTACT_FLAG_INRANGE");

	if (contactFlags & CONTACT_FLAG_INCONTACT)
		WLog_DBG(TAG, " CONTACT_FLAG_INCONTACT");

	if (contactFlags & CONTACT_FLAG_CANCELED)
		WLog_DBG(TAG, " CONTACT_FLAG_CANCELED");
}