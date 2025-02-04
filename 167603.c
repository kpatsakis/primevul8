
static void add_section_info(cli_ctx *ctx, struct cli_exe_section *s)
{
    struct json_object *sections, *section, *obj;
    char address[16];

    sections = get_section_json(ctx);
    if (!(sections))
        return;

    section = json_object_new_object();
    if (!(section))
        return;

    obj = json_object_new_int((int32_t)(s->rsz));
    if (!(obj))
        return;

    json_object_object_add(section, "RawSize", obj);

    obj = json_object_new_int((int32_t)(s->raw));
    if (!(obj))
        return;

    json_object_object_add(section, "RawOffset", obj);

    snprintf(address, sizeof(address), "0x%08x", s->rva);

    obj = json_object_new_string(address);
    if (!(obj))
        return;

    json_object_object_add(section, "VirtualAddress", obj);

    obj = json_object_new_boolean((s->chr & 0x20000000) == 0x20000000);
    if ((obj))
        json_object_object_add(section, "Executable", obj);

    obj = json_object_new_boolean((s->chr & 0x80000000) == 0x80000000);
    if ((obj))
        json_object_object_add(section, "Writable", obj);

    obj = json_object_new_boolean(s->urva>>31 || s->uvsz>>31 || (s->rsz && s->uraw>>31) || s->ursz>>31);
    if ((obj))
        json_object_object_add(section, "Signed", obj);

    json_object_array_add(sections, section);