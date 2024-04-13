ipf_is_v6_frag(ovs_be16 ip6f_offlg)
{
    if (ip6f_offlg & (IP6F_OFF_MASK | IP6F_MORE_FRAG)) {
        return true;
    }
    return false;
}