static void reset_embedded_font_parsing(ASS_ParserPriv *parser_priv)
{
    free(parser_priv->fontname);
    free(parser_priv->fontdata);
    parser_priv->fontname = NULL;
    parser_priv->fontdata = NULL;
    parser_priv->fontdata_size = 0;
    parser_priv->fontdata_used = 0;
}