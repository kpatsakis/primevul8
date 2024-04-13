proto_registrar_dump_fieldcount(void)
{
	guint32			i;
	header_field_info	*hfinfo;
	guint32			deregistered_count = 0;
	guint32			same_name_count = 0;
	guint32			protocol_count = 0;

	for (i = 0; i < gpa_hfinfo.len; i++) {
		if (gpa_hfinfo.hfi[i] == NULL) {
			deregistered_count++;
			continue; /* This is a deregistered protocol or header field */
		}

		PROTO_REGISTRAR_GET_NTH(i, hfinfo);

		if (proto_registrar_is_protocol(i))
			protocol_count++;

		if (hfinfo->same_name_prev_id != -1)
			same_name_count++;
	}

	ws_debug_printf("There are %u header fields registered, of which:\n"
		"\t%u are deregistered\n"
		"\t%u are protocols\n"
		"\t%u have the same name as another field\n\n",
		gpa_hfinfo.len, deregistered_count, protocol_count,
		same_name_count);

	ws_debug_printf("%d fields were pre-allocated.\n%s", PROTO_PRE_ALLOC_HF_FIELDS_MEM,
		(gpa_hfinfo.allocated_len > PROTO_PRE_ALLOC_HF_FIELDS_MEM) ?
		    "* * Please increase PROTO_PRE_ALLOC_HF_FIELDS_MEM (in epan/proto.c)! * *\n\n" :
		    "\n");

	ws_debug_printf("The header field table consumes %u KiB of memory.\n",
		(unsigned int)(gpa_hfinfo.allocated_len * sizeof(header_field_info *) / 1024));
	ws_debug_printf("The fields themselves consume %u KiB of memory.\n",
		(unsigned int)(gpa_hfinfo.len * sizeof(header_field_info) / 1024));

	return (gpa_hfinfo.allocated_len > PROTO_PRE_ALLOC_HF_FIELDS_MEM);
}