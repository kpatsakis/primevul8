void VariationalRefinementImpl::mergeCheckerboard(Mat &dst, RedBlackBuffer &src)
{
    int buf_j, j;
    for (int i = 0; i < dst.rows; i++)
    {
        float *src_r_buf = src.red.ptr<float>(i + 1);
        float *src_b_buf = src.black.ptr<float>(i + 1);
        float *dst_buf = dst.ptr<float>(i);
        buf_j = 1;

        if (i % 2 == 0)
        {
            for (j = 0; j < dst.cols - 1; j += 2)
            {
                dst_buf[j] = src_r_buf[buf_j];
                dst_buf[j + 1] = src_b_buf[buf_j];
                buf_j++;
            }
            if (j < dst.cols)
                dst_buf[j] = src_r_buf[buf_j];
        }
        else
        {
            for (j = 0; j < dst.cols - 1; j += 2)
            {
                dst_buf[j] = src_b_buf[buf_j];
                dst_buf[j + 1] = src_r_buf[buf_j];
                buf_j++;
            }
            if (j < dst.cols)
                dst_buf[j] = src_b_buf[buf_j];
        }
    }
}