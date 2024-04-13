register_type_length_mismatch(void)
{
	static ei_register_info ei[] = {
		{ &ei_type_length_mismatch_error, { "_ws.type_length.mismatch", PI_MALFORMED, PI_ERROR, "Trying to fetch X with length Y", EXPFILL }},
		{ &ei_type_length_mismatch_warn, { "_ws.type_length.mismatch", PI_MALFORMED, PI_WARN, "Trying to fetch X with length Y", EXPFILL }},
	};

	expert_module_t* expert_type_length_mismatch;

	proto_type_length_mismatch = proto_register_protocol("Type Length Mismatch", "Type length mismatch", "_ws.type_length");

	expert_type_length_mismatch = expert_register_protocol(proto_type_length_mismatch);
	expert_register_field_array(expert_type_length_mismatch, ei, array_length(ei));

	/* "Type Length Mismatch" isn't really a protocol, it's an error indication;
	   disabling them makes no sense. */
	proto_set_cant_toggle(proto_type_length_mismatch);
}