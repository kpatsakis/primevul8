resolve_data_descendant_schema_nodeid(const char *nodeid, struct lyd_node *start)
{
    char *str, *token, *p;
    struct lyd_node *result = NULL, *iter;
    const struct lys_node *schema = NULL;

    assert(nodeid && start);

    if (nodeid[0] == '/') {
        return NULL;
    }

    str = p = strdup(nodeid);
    LY_CHECK_ERR_RETURN(!str, LOGMEM(start->schema->module->ctx), NULL);

    while (p) {
        token = p;
        p = strchr(p, '/');
        if (p) {
            *p = '\0';
            p++;
        }

        if (p) {
            /* inner node */
            if (resolve_descendant_schema_nodeid(token, schema ? schema->child : start->schema,
                                                 LYS_CONTAINER | LYS_CHOICE | LYS_CASE | LYS_LEAF, 0, &schema)
                    || !schema) {
                result = NULL;
                break;
            }

            if (schema->nodetype & (LYS_CHOICE | LYS_CASE)) {
                continue;
            }
        } else {
            /* final node */
            if (resolve_descendant_schema_nodeid(token, schema ? schema->child : start->schema, LYS_LEAF, 0, &schema)
                    || !schema) {
                result = NULL;
                break;
            }
        }
        LY_TREE_FOR(result ? result->child : start, iter) {
            if (iter->schema == schema) {
                /* move in data tree according to returned schema */
                result = iter;
                break;
            }
        }
        if (!iter) {
            /* instance not found */
            result = NULL;
            break;
        }
    }
    free(str);

    return result;
}