gx_finish_output_page(gx_device *dev, int num_copies, int flush)
{
    dev->PageCount += num_copies;
    return 0;
}