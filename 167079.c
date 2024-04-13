proto_free_deregistered_fields (void)
{
	expert_free_deregistered_expertinfos();

	g_ptr_array_foreach(deregistered_fields, free_deregistered_field, NULL);
	g_ptr_array_free(deregistered_fields, TRUE);
	deregistered_fields = g_ptr_array_new();

	g_ptr_array_foreach(deregistered_data, free_deregistered_data, NULL);
	g_ptr_array_free(deregistered_data, TRUE);
	deregistered_data = g_ptr_array_new();
}