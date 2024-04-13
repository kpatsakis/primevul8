oftrace_push_ct_state(struct ovs_list *next_ct_states, uint32_t ct_state)
{
    struct oftrace_next_ct_state *next_ct_state =
        xmalloc(sizeof *next_ct_state);
    next_ct_state->state = ct_state;
    ovs_list_push_back(next_ct_states, &next_ct_state->node);
}