static BOOL rdg_get_gateway_credentials(rdpContext* context)
{
	rdpSettings* settings = context->settings;
	freerdp* instance = context->instance;

	if (!settings->GatewayPassword || !settings->GatewayUsername ||
	    !strlen(settings->GatewayPassword) || !strlen(settings->GatewayUsername))
	{
		if (freerdp_shall_disconnect(instance))
			return FALSE;

		if (!instance->GatewayAuthenticate)
		{
			freerdp_set_last_error_log(context, FREERDP_ERROR_CONNECT_NO_OR_MISSING_CREDENTIALS);
			return FALSE;
		}
		else
		{
			BOOL proceed =
			    instance->GatewayAuthenticate(instance, &settings->GatewayUsername,
			                                  &settings->GatewayPassword, &settings->GatewayDomain);

			if (!proceed)
			{
				freerdp_set_last_error_log(context,
				                           FREERDP_ERROR_CONNECT_NO_OR_MISSING_CREDENTIALS);
				return FALSE;
			}

			if (settings->GatewayUseSameCredentials)
			{
				if (settings->GatewayUsername)
				{
					free(settings->Username);

					if (!(settings->Username = _strdup(settings->GatewayUsername)))
						return FALSE;
				}

				if (settings->GatewayDomain)
				{
					free(settings->Domain);

					if (!(settings->Domain = _strdup(settings->GatewayDomain)))
						return FALSE;
				}

				if (settings->GatewayPassword)
				{
					free(settings->Password);

					if (!(settings->Password = _strdup(settings->GatewayPassword)))
						return FALSE;
				}
			}
		}
	}

	return TRUE;
}