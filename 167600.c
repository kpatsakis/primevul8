
static void pe_add_heuristic_property(cli_ctx *ctx, const char *key)
{
    struct json_object *heuristics;
    struct json_object *pe;
    struct json_object *str;

    pe = get_pe_property(ctx);
    if (!(pe))
        return;

    if (!json_object_object_get_ex(pe, "Heuristics", &heuristics)) {
        heuristics = json_object_new_array();
        if (!(heuristics))
            return;

        json_object_object_add(pe, "Heuristics", heuristics);
    }

    str = json_object_new_string(key);
    if (!(str))
        return;

    json_object_array_add(heuristics, str);