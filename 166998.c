proto_registrar_dump_ftypes(void)
{
	int fte;

	for (fte = 0; fte < FT_NUM_TYPES; fte++) {
		ws_debug_printf("%s\t%s\n", ftype_name((ftenum_t)fte), ftype_pretty_name((ftenum_t)fte));
	}
}