urlIsRelative(const char *url)
{
    if (!url)
        return false; // no URL

    /*
     * RFC 3986 section 5.2.3
     *
     * path          = path-abempty    ; begins with "/" or is empty
     *               / path-absolute   ; begins with "/" but not "//"
     *               / path-noscheme   ; begins with a non-colon segment
     *               / path-rootless   ; begins with a segment
     *               / path-empty      ; zero characters
     */

    if (*url == '\0')
        return true; // path-empty

    if (*url == '/') {
        // RFC 3986 section 5.2.3
        // path-absolute   ; begins with "/" but not "//"
        if (url[1] == '/')
            return true; // network-path reference, aka. 'scheme-relative URI'
        else
            return true; // path-absolute, aka 'absolute-path reference'
    }

    for (const auto *p = url; *p != '\0' && *p != '/' && *p != '?' && *p != '#'; ++p) {
        if (*p == ':')
            return false; // colon is forbidden in first segment
    }

    return true; // path-noscheme, path-abempty, path-rootless
}