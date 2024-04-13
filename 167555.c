poppler_font_info_finalize (GObject *object)
{
        PopplerFontInfo *font_info = POPPLER_FONT_INFO (object);

        delete font_info->scanner;
        g_object_unref (font_info->document);

        G_OBJECT_CLASS (poppler_font_info_parent_class)->finalize (object);
}