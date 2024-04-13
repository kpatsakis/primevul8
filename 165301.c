get_user_display_name (void)
{
        const char *name;
        char       *utf8_name;

        name = g_get_real_name ();

        if (name == NULL || strcmp (name, "Unknown") == 0) {
                name = g_get_user_name ();
        }

        utf8_name = NULL;

        if (name != NULL) {
                utf8_name = g_locale_to_utf8 (name, -1, NULL, NULL, NULL);
        }

        return utf8_name;
}