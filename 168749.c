static UINT rdpei_touch_update(RdpeiClientContext* context, int externalId, int x, int y,
                               int* contactId)
{
	unsigned int i;
	int contactIdlocal = -1;
	RDPINPUT_CONTACT_DATA contact;
	RDPINPUT_CONTACT_POINT* contactPoint = NULL;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)context->handle;
	UINT error = CHANNEL_RC_OK;

	for (i = 0; i < rdpei->maxTouchContacts; i++)
	{
		contactPoint = (RDPINPUT_CONTACT_POINT*)&rdpei->contactPoints[i];

		if (!contactPoint->active)
			continue;

		if (contactPoint->externalId == externalId)
		{
			contactIdlocal = contactPoint->contactId;
			break;
		}
	}

	if (contactIdlocal >= 0)
	{
		ZeroMemory(&contact, sizeof(RDPINPUT_CONTACT_DATA));
		contactPoint->lastX = x;
		contactPoint->lastY = y;
		contact.x = x;
		contact.y = y;
		contact.contactId = (UINT32)contactIdlocal;
		contact.contactFlags |= CONTACT_FLAG_UPDATE;
		contact.contactFlags |= CONTACT_FLAG_INRANGE;
		contact.contactFlags |= CONTACT_FLAG_INCONTACT;
		error = context->AddContact(context, &contact);
	}

	*contactId = contactIdlocal;
	return error;
}