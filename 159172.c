free_ct_states(struct ovs_list *ct_states)
{
    while (!ovs_list_is_empty(ct_states)) {
        oftrace_pop_ct_state(ct_states);
    }
}