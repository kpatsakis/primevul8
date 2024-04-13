static UINT rdpei_add_frame(RdpeiClientContext* context)
{
	int i;
	RDPINPUT_CONTACT_DATA* contact;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)context->handle;
	rdpei->frame.contactCount = 0;

	for (i = 0; i < rdpei->maxTouchContacts; i++)
	{
		contact = (RDPINPUT_CONTACT_DATA*)&(rdpei->contactPoints[i].data);

		if (rdpei->contactPoints[i].dirty)
		{
			CopyMemory(&(rdpei->contacts[rdpei->frame.contactCount]), contact,
			           sizeof(RDPINPUT_CONTACT_DATA));
			rdpei->contactPoints[i].dirty = FALSE;
			rdpei->frame.contactCount++;
		}
		else if (rdpei->contactPoints[i].active)
		{
			if (contact->contactFlags & CONTACT_FLAG_DOWN)
			{
				contact->contactFlags = CONTACT_FLAG_UPDATE;
				contact->contactFlags |= CONTACT_FLAG_INRANGE;
				contact->contactFlags |= CONTACT_FLAG_INCONTACT;
			}

			CopyMemory(&(rdpei->contacts[rdpei->frame.contactCount]), contact,
			           sizeof(RDPINPUT_CONTACT_DATA));
			rdpei->frame.contactCount++;
		}
	}

	return CHANNEL_RC_OK;
}