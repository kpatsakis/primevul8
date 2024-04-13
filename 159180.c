oftrace_pop_ct_state(struct ovs_list *next_ct_states)
{
    struct oftrace_next_ct_state *s;
    LIST_FOR_EACH_POP (s, node, next_ct_states) {
        uint32_t state = s->state;
        free(s);
        return state;
    }
    OVS_NOT_REACHED();
}