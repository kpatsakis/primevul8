void nego_free_nla(rdpNego* nego)
{
	if (!nego || !nego->transport)
		return;

	nla_free(nego->transport->nla);
	nego->transport->nla = NULL;
}