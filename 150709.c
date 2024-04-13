void VariationalRefinementImpl::updateRepeatedBorders(RedBlackBuffer &dst)
{
    int buf_w = dst.red.cols;
    for (int i = 0; i < dst.red.rows - 2; i++)
    {
        float *r_buf = dst.red.ptr<float>(i + 1);
        float *b_buf = dst.black.ptr<float>(i + 1);

        if (i % 2 == 0)
        {
            b_buf[0] = r_buf[1];
            if (dst.red_even_len > dst.black_even_len)
                b_buf[dst.black_even_len + 1] = r_buf[dst.red_even_len];
            else
                r_buf[dst.red_even_len + 1] = b_buf[dst.black_even_len];
        }
        else
        {
            r_buf[0] = b_buf[1];
            if (dst.red_odd_len < dst.black_odd_len)
                r_buf[dst.red_odd_len + 1] = b_buf[dst.black_odd_len];
            else
                b_buf[dst.black_odd_len + 1] = r_buf[dst.red_odd_len];
        }
    }
    {
        float *r_buf_border = dst.red.ptr<float>(dst.red.rows - 1);
        float *b_buf_border = dst.black.ptr<float>(dst.black.rows - 1);
        float *r_buf = dst.red.ptr<float>(dst.red.rows - 2);
        float *b_buf = dst.black.ptr<float>(dst.black.rows - 2);
        memcpy(r_buf_border, b_buf, buf_w * sizeof(float));
        memcpy(b_buf_border, r_buf, buf_w * sizeof(float));
    }
    {
        float *r_buf_border = dst.red.ptr<float>(0);
        float *b_buf_border = dst.black.ptr<float>(0);
        float *r_buf = dst.red.ptr<float>(1);
        float *b_buf = dst.black.ptr<float>(1);
        memcpy(r_buf_border, b_buf, buf_w * sizeof(float));
        memcpy(b_buf_border, r_buf, buf_w * sizeof(float));
    }
}