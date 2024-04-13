camel_gpg_context_get_always_trust (CamelGpgContext *context)
{
	g_return_val_if_fail (CAMEL_IS_GPG_CONTEXT (context), FALSE);

	return context->priv->always_trust;
}