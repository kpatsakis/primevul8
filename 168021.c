nm_utils_machine_id_is_fake(void)
{
    return _machine_id_get(TRUE)->is_fake;
}