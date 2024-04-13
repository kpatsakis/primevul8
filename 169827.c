lou_registerTableResolver(
		char **(EXPORT_CALL *resolver)(const char *tableList, const char *base)) {
	tableResolver = resolver;
}