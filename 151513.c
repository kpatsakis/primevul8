resolve_choice_default_schema_nodeid(const char *nodeid, const struct lys_node *start, const struct lys_node **ret)
{
    /* cannot actually be a path */
    if (strchr(nodeid, '/')) {
        return -1;
    }

    return resolve_descendant_schema_nodeid(nodeid, start, LYS_NO_RPC_NOTIF_NODE, 0, ret);
}