urlCanonicalFakeHttps(const HttpRequest * request)
{
    LOCAL_ARRAY(char, buf, MAX_URL);

    // method CONNECT and port HTTPS
    if (request->method == Http::METHOD_CONNECT && request->url.port() == 443) {
        snprintf(buf, MAX_URL, "https://%s/*", request->url.host());
        return buf;
    }

    // else do the normal complete canonical thing.
    return request->canonicalCleanUrl();
}