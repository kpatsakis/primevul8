SEC_WINNT_AUTH_IDENTITY* nego_get_identity(rdpNego* nego)
{
	if (!nego)
		return NULL;

	return nla_get_identity(nego->transport->nla);
}