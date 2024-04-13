elastic_add_base_mapping(json_dumper *dumper)
{
	json_dumper_set_member_name(dumper, "index_patterns");
	json_dumper_value_string(dumper, "packets-*");

	json_dumper_set_member_name(dumper, "settings");
	json_dumper_begin_object(dumper);
	json_dumper_set_member_name(dumper, "index.mapping.total_fields.limit");
	json_dumper_value_anyf(dumper, "%d", 1000000);
	json_dumper_end_object(dumper);
}