camel_gpg_context_set_always_trust (CamelGpgContext *context,
                                    gboolean always_trust)
{
	g_return_if_fail (CAMEL_IS_GPG_CONTEXT (context));

	if (context->priv->always_trust == always_trust)
		return;

	context->priv->always_trust = always_trust;

	g_object_notify (G_OBJECT (context), "always-trust");
}