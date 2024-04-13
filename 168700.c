rdpNego* nego_new(rdpTransport* transport)
{
	rdpNego* nego = (rdpNego*)calloc(1, sizeof(rdpNego));

	if (!nego)
		return NULL;

	nego->transport = transport;
	nego_init(nego);
	return nego;
}