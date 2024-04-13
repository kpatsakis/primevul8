register_number_string_decoding_error(void)
{
	static ei_register_info ei[] = {
		{ &ei_number_string_decoding_failed_error,
			{ "_ws.number_string.decoding_error.failed", PI_MALFORMED, PI_ERROR,
			  "Failed to decode number from string", EXPFILL
			}
		},
		{ &ei_number_string_decoding_erange_error,
			{ "_ws.number_string.decoding_error.erange", PI_MALFORMED, PI_ERROR,
			  "Decoded number from string is out of valid range", EXPFILL
			}
		},
	};

	expert_module_t* expert_number_string_decoding_error;

	proto_number_string_decoding_error =
		proto_register_protocol("Number-String Decoding Error",
					"Number-string decoding error",
					"_ws.number_string.decoding_error");

	expert_number_string_decoding_error =
		expert_register_protocol(proto_number_string_decoding_error);
	expert_register_field_array(expert_number_string_decoding_error, ei, array_length(ei));

	/* "Number-String Decoding Error" isn't really a protocol, it's an error indication;
	   disabling them makes no sense. */
	proto_set_cant_toggle(proto_number_string_decoding_error);
}