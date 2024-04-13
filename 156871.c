make_file_name(const char *name, const char *prog, const char *namespace, const char *instance)
{
	const char *extn_start;
	const char *dir_end;
	size_t len;
	char *file_name;

	if (!name)
		return NULL;

	len = strlen(name);
	if (prog)
		len += strlen(prog) + 1;
	if (namespace)
		len += strlen(namespace) + 1;
	if (instance)
		len += strlen(instance) + 1;

	file_name = MALLOC(len + 1);
	dir_end = strrchr(name, '/');
	extn_start = strrchr(dir_end ? dir_end : name, '.');
	strncpy(file_name, name, extn_start ? (size_t)(extn_start - name) : len);

	if (prog) {
		strcat(file_name, "_");
		strcat(file_name, prog);
	}
	if (namespace) {
		strcat(file_name, "_");
		strcat(file_name, namespace);
	}
	if (instance) {
		strcat(file_name, "_");
		strcat(file_name, instance);
	}
	if (extn_start)
		strcat(file_name, extn_start);

	return file_name;
}