static const char *wsgi_set_newrelic_config_file(
        cmd_parms *cmd, void *mconfig, const char *f)
{
    const char *error = NULL;
    WSGIServerConfig *sconfig = NULL;

    error = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (error != NULL)
        return error;

    sconfig = ap_get_module_config(cmd->server->module_config, &wsgi_module);
    sconfig->newrelic_config_file = f;

    return NULL;
}