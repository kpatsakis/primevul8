_PUBLIC_ char *afdgets(int fd, TALLOC_CTX *mem_ctx, size_t hint)
{
	char *data = NULL;
	ssize_t alloc_size = 0, offset = 0, ret;
	int p;

	if (hint <= 0) hint = 0x100;

	do {
		alloc_size += hint;

		data = talloc_realloc(mem_ctx, data, char, alloc_size);

		if (!data)
			return NULL;

		ret = read(fd, data + offset, hint);

		if (ret == 0) {
			return NULL;
		}

		if (ret == -1) {
			talloc_free(data);
			return NULL;
		}

		/* Find newline */
		for (p = 0; p < ret; p++) {
			if (data[offset + p] == '\n')
				break;
		}

		if (p < ret) {
			data[offset + p] = '\0';

			/* Go back to position of newline */
			lseek(fd, p - ret + 1, SEEK_CUR);
			return data;
		}

		offset += ret;

	} while (ret == hint);

	data[offset] = '\0';

	return data;
}