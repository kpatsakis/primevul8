urlCheckRequest(const HttpRequest * r)
{
    int rc = 0;
    /* protocol "independent" methods
     *
     * actually these methods are specific to HTTP:
     * they are methods we receive on our HTTP port,
     * and if we had a FTP listener would not be relevant
     * there.
     *
     * So, we should delegate them to HTTP. The problem is that we
     * do not have a default protocol from the client side of HTTP.
     */

    if (r->method == Http::METHOD_CONNECT)
        return 1;

    // we support OPTIONS and TRACE directed at us (with a 501 reply, for now)
    // we also support forwarding OPTIONS and TRACE, except for the *-URI ones
    if (r->method == Http::METHOD_OPTIONS || r->method == Http::METHOD_TRACE)
        return (r->header.getInt64(Http::HdrType::MAX_FORWARDS) == 0 || r->url.path() != AnyP::Uri::Asterisk());

    if (r->method == Http::METHOD_PURGE)
        return 1;

    /* does method match the protocol? */
    switch (r->url.getScheme()) {

    case AnyP::PROTO_URN:

    case AnyP::PROTO_HTTP:

    case AnyP::PROTO_CACHE_OBJECT:
        rc = 1;
        break;

    case AnyP::PROTO_FTP:

        if (r->method == Http::METHOD_PUT)
            rc = 1;

    case AnyP::PROTO_GOPHER:

    case AnyP::PROTO_WAIS:

    case AnyP::PROTO_WHOIS:
        if (r->method == Http::METHOD_GET)
            rc = 1;
        else if (r->method == Http::METHOD_HEAD)
            rc = 1;

        break;

    case AnyP::PROTO_HTTPS:
#if USE_OPENSSL
        rc = 1;
#elif USE_GNUTLS
        rc = 1;
#else
        /*
        * Squid can't originate an SSL connection, so it should
        * never receive an "https:" URL.  It should always be
        * CONNECT instead.
        */
        rc = 0;
#endif
        break;

    default:
        break;
    }

    return rc;
}