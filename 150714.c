    void gradHorizAndSplitOp(void *src, void *dst, void *dst_split)
    {
        Sobel(*(Mat *)src, *(Mat *)dst, -1, 1, 0, 1, 1, 0.00, BORDER_REPLICATE);
        splitCheckerboard(*(RedBlackBuffer *)dst_split, *(Mat *)dst);
    }