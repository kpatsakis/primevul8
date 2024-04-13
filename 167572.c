poppler_ps_file_init (PopplerPSFile *ps_file)
{
        ps_file->out = nullptr;
        ps_file->paper_width = -1;
        ps_file->paper_height = -1;
        ps_file->duplex = FALSE;
}