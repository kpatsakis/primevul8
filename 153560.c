xface2xpm(char *xface)
{
    Image image;
    ImageCache *cache;
    FILE *f;
    struct stat st;

    SKIP_BLANKS(xface);
    image.url = xface;
    image.ext = ".xpm";
    image.width = 48;
    image.height = 48;
    image.cache = NULL;
    cache = getImage(&image, NULL, IMG_FLAG_AUTO);
    if (cache->loaded & IMG_FLAG_LOADED && !stat(cache->file, &st))
	return cache->file;
    cache->loaded = IMG_FLAG_ERROR;

    f = popen(Sprintf("%s > %s", shell_quote(auxbinFile(XFACE2XPM)),
		      shell_quote(cache->file))->ptr, "w");
    if (!f)
	return NULL;
    fputs(xface, f);
    pclose(f);
    if (stat(cache->file, &st) || !st.st_size)
	return NULL;
    cache->loaded = IMG_FLAG_LOADED | IMG_FLAG_DONT_REMOVE;
    cache->index = 0;
    return cache->file;
}