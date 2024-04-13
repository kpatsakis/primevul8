ipf_get_v4_enabled(struct ipf *ipf)
{
    bool ifp_v4_enabled_;
    atomic_read_relaxed(&ipf->ifp_v4_enabled, &ifp_v4_enabled_);
    return ifp_v4_enabled_;
}