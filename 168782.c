static UINT rdpei_add_contact(RdpeiClientContext* context, const RDPINPUT_CONTACT_DATA* contact)
{
	UINT error;
	RDPINPUT_CONTACT_POINT* contactPoint;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)context->handle;

	contactPoint = (RDPINPUT_CONTACT_POINT*)&rdpei->contactPoints[contact->contactId];
	CopyMemory(&(contactPoint->data), contact, sizeof(RDPINPUT_CONTACT_DATA));
	contactPoint->dirty = TRUE;

	error = rdpei_add_frame(context);
	if (error != CHANNEL_RC_OK)
	{
		WLog_ERR(TAG, "rdpei_add_frame failed with error %" PRIu32 "!", error);
		return error;
	}

	if (rdpei->frame.contactCount > 0)
	{
		error = rdpei_send_frame(context);
		if (error != CHANNEL_RC_OK)
		{
			WLog_ERR(TAG, "rdpei_send_frame failed with error %" PRIu32 "!", error);
			return error;
		}
	}
	return CHANNEL_RC_OK;
}