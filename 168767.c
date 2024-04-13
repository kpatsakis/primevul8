UINT DVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints)
{
	UINT error;
	RDPEI_PLUGIN* rdpei = NULL;
	RdpeiClientContext* context = NULL;
	rdpei = (RDPEI_PLUGIN*)pEntryPoints->GetPlugin(pEntryPoints, "rdpei");

	if (!rdpei)
	{
		size_t size;
		rdpei = (RDPEI_PLUGIN*)calloc(1, sizeof(RDPEI_PLUGIN));

		if (!rdpei)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		rdpei->iface.Initialize = rdpei_plugin_initialize;
		rdpei->iface.Connected = NULL;
		rdpei->iface.Disconnected = NULL;
		rdpei->iface.Terminated = rdpei_plugin_terminated;
		rdpei->version = 1;
		rdpei->currentFrameTime = 0;
		rdpei->previousFrameTime = 0;
		rdpei->frame.contacts = (RDPINPUT_CONTACT_DATA*)rdpei->contacts;
		rdpei->maxTouchContacts = 10;
		size = rdpei->maxTouchContacts * sizeof(RDPINPUT_CONTACT_POINT);
		rdpei->contactPoints = (RDPINPUT_CONTACT_POINT*)calloc(1, size);
		rdpei->rdpcontext =
		    ((freerdp*)((rdpSettings*)pEntryPoints->GetRdpSettings(pEntryPoints))->instance)
		        ->context;

		if (!rdpei->contactPoints)
		{
			WLog_ERR(TAG, "calloc failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		context = (RdpeiClientContext*)calloc(1, sizeof(RdpeiClientContext));

		if (!context)
		{
			WLog_ERR(TAG, "calloc failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		context->handle = (void*)rdpei;
		context->GetVersion = rdpei_get_version;
		context->AddContact = rdpei_add_contact;
		context->TouchBegin = rdpei_touch_begin;
		context->TouchUpdate = rdpei_touch_update;
		context->TouchEnd = rdpei_touch_end;
		rdpei->iface.pInterface = (void*)context;

		if ((error = pEntryPoints->RegisterPlugin(pEntryPoints, "rdpei", (IWTSPlugin*)rdpei)))
		{
			WLog_ERR(TAG, "EntryPoints->RegisterPlugin failed with error %" PRIu32 "!", error);
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		rdpei->context = context;
	}

	return CHANNEL_RC_OK;
error_out:
	free(context);
	free(rdpei->contactPoints);
	free(rdpei);
	return error;
}