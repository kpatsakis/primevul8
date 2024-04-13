void VariationalRefinementImpl::splitCheckerboard(RedBlackBuffer &dst, Mat &src)
{
    int buf_j, j;
    int buf_w = (int)ceil(src.cols / 2.0) + 2; //!< max width of red/black buffers with borders

    /* Rows of red and black buffers can have different actual width, some extra repeated values are
     * added for padding in such cases.
     */
    for (int i = 0; i < src.rows; i++)
    {
        float *src_buf = src.ptr<float>(i);
        float *r_buf = dst.red.ptr<float>(i + 1);
        float *b_buf = dst.black.ptr<float>(i + 1);
        r_buf[0] = b_buf[0] = src_buf[0];
        buf_j = 1;
        if (i % 2 == 0)
        {
            for (j = 0; j < src.cols - 1; j += 2)
            {
                r_buf[buf_j] = src_buf[j];
                b_buf[buf_j] = src_buf[j + 1];
                buf_j++;
            }
            if (j < src.cols)
                r_buf[buf_j] = b_buf[buf_j] = src_buf[j];
            else
                j--;
        }
        else
        {
            for (j = 0; j < src.cols - 1; j += 2)
            {
                b_buf[buf_j] = src_buf[j];
                r_buf[buf_j] = src_buf[j + 1];
                buf_j++;
            }
            if (j < src.cols)
                r_buf[buf_j] = b_buf[buf_j] = src_buf[j];
            else
                j--;
        }
        r_buf[buf_w - 1] = b_buf[buf_w - 1] = src_buf[j];
    }

    /* Fill top and bottom borders: */
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