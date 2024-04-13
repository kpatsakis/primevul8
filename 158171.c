urlCanonicalCleanWithoutRequest(const SBuf &url, const HttpRequestMethod &method, const AnyP::UriScheme &scheme)
{
    LOCAL_ARRAY(char, buf, MAX_URL);

    snprintf(buf, sizeof(buf), SQUIDSBUFPH, SQUIDSBUFPRINT(url));
    buf[sizeof(buf)-1] = '\0';

    // URN, CONNECT method, and non-stripped URIs can go straight out
    if (Config.onoff.strip_query_terms && !(method == Http::METHOD_CONNECT || scheme == AnyP::PROTO_URN)) {
        // strip anything AFTER a question-mark
        // leaving the '?' in place
        if (auto t = strchr(buf, '?')) {
            *(++t) = '\0';
        }
    }

    if (stringHasCntl(buf))
        xstrncpy(buf, rfc1738_escape_unescaped(buf), MAX_URL);

    return buf;
}