poppler_font_info_class_init (PopplerFontInfoClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        gobject_class->finalize = poppler_font_info_finalize;
}