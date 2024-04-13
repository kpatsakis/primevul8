static UINT rdpei_touch_end(RdpeiClientContext* context, int externalId, int x, int y,
                            int* contactId)
{
	unsigned int i;
	int contactIdlocal = -1;
	int tempvalue;
	RDPINPUT_CONTACT_DATA contact;
	RDPINPUT_CONTACT_POINT* contactPoint = NULL;
	RDPEI_PLUGIN* rdpei = (RDPEI_PLUGIN*)context->handle;
	UINT error;

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

		if ((contactPoint->lastX != x) && (contactPoint->lastY != y))
		{
			if ((error = context->TouchUpdate(context, externalId, x, y, &tempvalue)))
			{
				WLog_ERR(TAG, "context->TouchUpdate failed with error %" PRIu32 "!", error);
				return error;
			}
		}

		contact.x = x;
		contact.y = y;
		contact.contactId = (UINT32)contactIdlocal;
		contact.contactFlags |= CONTACT_FLAG_UP;

		if ((error = context->AddContact(context, &contact)))
		{
			WLog_ERR(TAG, "context->AddContact failed with error %" PRIu32 "!", error);
			return error;
		}

		contactPoint->externalId = 0;
		contactPoint->active = FALSE;
		contactPoint->flags = 0;
		contactPoint->contactId = 0;
		contactPoint->state = RDPINPUT_CONTACT_STATE_OUT_OF_RANGE;
	}

	*contactId = contactIdlocal;
	return CHANNEL_RC_OK;
}