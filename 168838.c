BOOL nego_connect(rdpNego* nego)
{
	rdpSettings* settings = nego->transport->settings;

	if (nego->state == NEGO_STATE_INITIAL)
	{
		if (nego->EnabledProtocols[PROTOCOL_HYBRID_EX])
		{
			nego->state = NEGO_STATE_EXT;
		}
		else if (nego->EnabledProtocols[PROTOCOL_HYBRID])
		{
			nego->state = NEGO_STATE_NLA;
		}
		else if (nego->EnabledProtocols[PROTOCOL_SSL])
		{
			nego->state = NEGO_STATE_TLS;
		}
		else if (nego->EnabledProtocols[PROTOCOL_RDP])
		{
			nego->state = NEGO_STATE_RDP;
		}
		else
		{
			WLog_ERR(TAG, "No security protocol is enabled");
			nego->state = NEGO_STATE_FAIL;
			return FALSE;
		}

		if (!nego->NegotiateSecurityLayer)
		{
			WLog_DBG(TAG, "Security Layer Negotiation is disabled");
			/* attempt only the highest enabled protocol (see nego_attempt_*) */
			nego->EnabledProtocols[PROTOCOL_HYBRID] = FALSE;
			nego->EnabledProtocols[PROTOCOL_SSL] = FALSE;
			nego->EnabledProtocols[PROTOCOL_RDP] = FALSE;
			nego->EnabledProtocols[PROTOCOL_HYBRID_EX] = FALSE;

			if (nego->state == NEGO_STATE_EXT)
			{
				nego->EnabledProtocols[PROTOCOL_HYBRID_EX] = TRUE;
				nego->EnabledProtocols[PROTOCOL_HYBRID] = TRUE;
				nego->SelectedProtocol = PROTOCOL_HYBRID_EX;
			}
			else if (nego->state == NEGO_STATE_NLA)
			{
				nego->EnabledProtocols[PROTOCOL_HYBRID] = TRUE;
				nego->SelectedProtocol = PROTOCOL_HYBRID;
			}
			else if (nego->state == NEGO_STATE_TLS)
			{
				nego->EnabledProtocols[PROTOCOL_SSL] = TRUE;
				nego->SelectedProtocol = PROTOCOL_SSL;
			}
			else if (nego->state == NEGO_STATE_RDP)
			{
				nego->EnabledProtocols[PROTOCOL_RDP] = TRUE;
				nego->SelectedProtocol = PROTOCOL_RDP;
			}
		}

		if (nego->SendPreconnectionPdu)
		{
			if (!nego_send_preconnection_pdu(nego))
			{
				WLog_ERR(TAG, "Failed to send preconnection pdu");
				nego->state = NEGO_STATE_FINAL;
				return FALSE;
			}
		}
	}

	if (!nego->NegotiateSecurityLayer)
	{
		nego->state = NEGO_STATE_FINAL;
	}
	else
	{
		do
		{
			WLog_DBG(TAG, "state: %s", nego_state_string(nego->state));
			nego_send(nego);

			if (nego->state == NEGO_STATE_FAIL)
			{
				if (freerdp_get_last_error(nego->transport->context) == FREERDP_ERROR_SUCCESS)
					WLog_ERR(TAG, "Protocol Security Negotiation Failure");

				nego->state = NEGO_STATE_FINAL;
				return FALSE;
			}
		} while (nego->state != NEGO_STATE_FINAL);
	}

	WLog_DBG(TAG, "Negotiated %s security", protocol_security_string(nego->SelectedProtocol));
	/* update settings with negotiated protocol security */
	settings->RequestedProtocols = nego->RequestedProtocols;
	settings->SelectedProtocol = nego->SelectedProtocol;
	settings->NegotiationFlags = nego->flags;

	if (nego->SelectedProtocol == PROTOCOL_RDP)
	{
		settings->UseRdpSecurityLayer = TRUE;

		if (!settings->EncryptionMethods)
		{
			/**
			 * Advertise all supported encryption methods if the client
			 * implementation did not set any security methods
			 */
			settings->EncryptionMethods = ENCRYPTION_METHOD_40BIT | ENCRYPTION_METHOD_56BIT |
			                              ENCRYPTION_METHOD_128BIT | ENCRYPTION_METHOD_FIPS;
		}
	}

	/* finally connect security layer (if not already done) */
	if (!nego_security_connect(nego))
	{
		WLog_DBG(TAG, "Failed to connect with %s security",
		         protocol_security_string(nego->SelectedProtocol));
		return FALSE;
	}

	return TRUE;
}