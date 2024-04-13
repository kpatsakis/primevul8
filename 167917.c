static uint32_t buf_to_uint32(char *data) {
    size_t i;
    uint32_t result = 0;

    for (i = 0; i < sizeof(uint32_t); i++)
        result = (result << 8) | (unsigned char)data[i];

    return result;
}