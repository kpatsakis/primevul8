ipf_get_v6_enabled(struct ipf *ipf)
{
    bool ifp_v6_enabled_;
    atomic_read_relaxed(&ipf->ifp_v6_enabled, &ifp_v6_enabled_);
    return ifp_v6_enabled_;
}