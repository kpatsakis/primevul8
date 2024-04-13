ipf_count(struct ipf *ipf, bool v6, enum ipf_counter_type cntr)
{
    atomic_count_inc64(v6 ? &ipf->n6frag_cnt[cntr] : &ipf->n4frag_cnt[cntr]);
}