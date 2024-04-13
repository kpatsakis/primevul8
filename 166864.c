register_string_errors(void)
{
	static ei_register_info ei[] = {
		{ &ei_string_trailing_characters,
			{ "_ws.string.trailing_stray_characters", PI_UNDECODED, PI_WARN, "Trailing stray characters", EXPFILL }
		},
	};

	expert_module_t* expert_string_errors;

	proto_string_errors = proto_register_protocol("String Errors", "String errors", "_ws.string");

	expert_string_errors = expert_register_protocol(proto_string_errors);
	expert_register_field_array(expert_string_errors, ei, array_length(ei));

	/* "String Errors" isn't really a protocol, it's an error indication;
	   disabling them makes no sense. */
	proto_set_cant_toggle(proto_string_errors);
}