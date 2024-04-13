static BOOL rdg_ntlm_init(rdpRdg* rdg, rdpTls* tls)
{
	BOOL continueNeeded = FALSE;
	rdpContext* context = rdg->context;
	rdpSettings* settings = context->settings;
	rdg->ntlm = ntlm_new();

	if (!rdg->ntlm)
		return FALSE;

	if (!rdg_get_gateway_credentials(context))
		return FALSE;

	if (!ntlm_client_init(rdg->ntlm, TRUE, settings->GatewayUsername, settings->GatewayDomain,
	                      settings->GatewayPassword, tls->Bindings))
		return FALSE;

	if (!ntlm_client_make_spn(rdg->ntlm, _T("HTTP"), settings->GatewayHostname))
		return FALSE;

	if (!ntlm_authenticate(rdg->ntlm, &continueNeeded))
		return FALSE;

	return continueNeeded;
}