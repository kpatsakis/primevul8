poppler_ps_file_set_paper_size (PopplerPSFile *ps_file,
                                double width, double height)
{
        g_return_if_fail (ps_file->out == nullptr);
        
        ps_file->paper_width = width;
        ps_file->paper_height = height;
}