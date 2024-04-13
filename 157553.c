util_curl_init(CurlObject *self)
{
    int res;

    /* Set curl error buffer and zero it */
    res = curl_easy_setopt(self->handle, CURLOPT_ERRORBUFFER, self->error);
    if (res != CURLE_OK) {
        return (-1);
    }
    memset(self->error, 0, sizeof(self->error));

    /* Set backreference */
    res = curl_easy_setopt(self->handle, CURLOPT_PRIVATE, (char *) self);
    if (res != CURLE_OK) {
        return (-1);
    }

    /* Enable NOPROGRESS by default, i.e. no progress output */
    res = curl_easy_setopt(self->handle, CURLOPT_NOPROGRESS, (long)1);
    if (res != CURLE_OK) {
        return (-1);
    }

    /* Disable VERBOSE by default, i.e. no verbose output */
    res = curl_easy_setopt(self->handle, CURLOPT_VERBOSE, (long)0);
    if (res != CURLE_OK) {
        return (-1);
    }

    /* Set FTP_ACCOUNT to NULL by default */
    res = curl_easy_setopt(self->handle, CURLOPT_FTP_ACCOUNT, NULL);
    if (res != CURLE_OK) {
        return (-1);
    }

    /* Set default USERAGENT */
    assert(g_pycurl_useragent);
    res = curl_easy_setopt(self->handle, CURLOPT_USERAGENT, g_pycurl_useragent);
    if (res != CURLE_OK) {
        return (-1);
    }
    return (0);
}