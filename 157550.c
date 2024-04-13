write_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    return util_write_callback(0, ptr, size, nmemb, stream);
}