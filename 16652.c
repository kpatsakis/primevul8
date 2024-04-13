ipf_get_enabled(struct ipf *ipf)
{
    return ipf_get_v4_enabled(ipf) || ipf_get_v6_enabled(ipf);
}