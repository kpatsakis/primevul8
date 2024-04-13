sdap_initgr_store_groups(struct sdap_initgr_nested_state *state)
{
    return sdap_nested_groups_store(state->sysdb, state->dom,
                                    state->opts, state->groups,
                                    state->groups_cur);
}