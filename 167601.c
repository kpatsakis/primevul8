#if HAVE_JSON
static struct json_object *get_pe_property(cli_ctx *ctx)
{
    struct json_object *pe;

    if (!(ctx) || !(ctx->wrkproperty))
        return NULL;

    if (!json_object_object_get_ex(ctx->wrkproperty, "PE", &pe)) {
        pe = json_object_new_object();
        if (!(pe))
            return NULL;

        json_object_object_add(ctx->wrkproperty, "PE", pe);
    }

    return pe;