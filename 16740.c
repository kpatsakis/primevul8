gpg_context_get_property (GObject *object,
                          guint property_id,
                          GValue *value,
                          GParamSpec *pspec)
{
	switch (property_id) {
		case PROP_ALWAYS_TRUST:
			g_value_set_boolean (
				value,
				camel_gpg_context_get_always_trust (
				CAMEL_GPG_CONTEXT (object)));
			return;
	}

	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}