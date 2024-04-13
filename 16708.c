camel_gpg_context_new (CamelSession *session)
{
	g_return_val_if_fail (CAMEL_IS_SESSION (session), NULL);

	return g_object_new (
		CAMEL_TYPE_GPG_CONTEXT,
		"session", session, NULL);
}