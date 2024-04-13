void rdg_free(rdpRdg* rdg)
{
	if (!rdg)
		return;

	tls_free(rdg->tlsOut);
	tls_free(rdg->tlsIn);
	http_context_free(rdg->http);
	ntlm_free(rdg->ntlm);

	if (!rdg->attached)
		BIO_free_all(rdg->frontBio);

	DeleteCriticalSection(&rdg->writeSection);
	free(rdg);
}