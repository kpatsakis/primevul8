
static struct json_object *get_section_json(cli_ctx *ctx)
{
    struct json_object *pe;
    struct json_object *section;

    pe = get_pe_property(ctx);
    if (!(pe))
        return NULL;

    if (!json_object_object_get_ex(pe, "Sections", &section)) {
        section = json_object_new_array();
        if (!(section))
            return NULL;

        json_object_object_add(pe, "Sections", section);
    }

    return section;