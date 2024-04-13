examineFile(char *path, URLFile *uf)
{
    struct stat stbuf;

    uf->guess_type = NULL;
    if (path == NULL || *path == '\0' ||
	stat(path, &stbuf) == -1 || NOT_REGULAR(stbuf.st_mode)) {
	uf->stream = NULL;
	return;
    }
    uf->stream = openIS(path);
    if (!do_download) {
	if (use_lessopen && getenv("LESSOPEN") != NULL) {
	    FILE *fp;
	    uf->guess_type = guessContentType(path);
	    if (uf->guess_type == NULL)
		uf->guess_type = "text/plain";
	    if (is_html_type(uf->guess_type))
		return;
	    if ((fp = lessopen_stream(path))) {
		UFclose(uf);
		uf->stream = newFileStream(fp, (void (*)())pclose);
		uf->guess_type = "text/plain";
		return;
	    }
	}
	check_compression(path, uf);
	if (uf->compression != CMP_NOCOMPRESS) {
	    char *ext = uf->ext;
	    char *t0 = uncompressed_file_type(path, &ext);
	    uf->guess_type = t0;
	    uf->ext = ext;
	    uncompress_stream(uf, NULL);
	    return;
	}
    }
}