nm_utils_parse_dns_domain(const char *domain, gboolean *is_routing)
{
    g_return_val_if_fail(domain, NULL);
    g_return_val_if_fail(domain[0], NULL);

    if (domain[0] == '~') {
        domain++;
        NM_SET_OUT(is_routing, TRUE);
    } else
        NM_SET_OUT(is_routing, FALSE);

    return domain;
}