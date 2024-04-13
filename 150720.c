    void averageOp(void *src1, void *src2, void *dst)
    {
        addWeighted(*(Mat *)src1, 0.5, *(Mat *)src2, 0.5, 0.0, *(Mat *)dst, CV_32F);
    }