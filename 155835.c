static int imagetoraw_common(opj_image_t * image, const char *outfile,
                             OPJ_BOOL big_endian)
{
    FILE *rawFile = NULL;
    size_t res;
    unsigned int compno;
    int w, h, fails;
    int line, row, curr, mask;
    int *ptr;
    unsigned char uc;
    (void)big_endian;

    if ((image->numcomps * image->x1 * image->y1) == 0) {
        fprintf(stderr, "\nError: invalid raw image parameters\n");
        return 1;
    }

    rawFile = fopen(outfile, "wb");
    if (!rawFile) {
        fprintf(stderr, "Failed to open %s for writing !!\n", outfile);
        return 1;
    }

    fails = 1;
    fprintf(stdout, "Raw image characteristics: %d components\n", image->numcomps);

    for (compno = 0; compno < image->numcomps; compno++) {
        fprintf(stdout, "Component %u characteristics: %dx%dx%d %s\n", compno,
                image->comps[compno].w,
                image->comps[compno].h, image->comps[compno].prec,
                image->comps[compno].sgnd == 1 ? "signed" : "unsigned");

        w = (int)image->comps[compno].w;
        h = (int)image->comps[compno].h;

        if (image->comps[compno].prec <= 8) {
            if (image->comps[compno].sgnd == 1) {
                mask = (1 << image->comps[compno].prec) - 1;
                ptr = image->comps[compno].data;
                for (line = 0; line < h; line++) {
                    for (row = 0; row < w; row++)    {
                        curr = *ptr;
                        if (curr > 127) {
                            curr = 127;
                        } else if (curr < -128) {
                            curr = -128;
                        }
                        uc = (unsigned char)(curr & mask);
                        res = fwrite(&uc, 1, 1, rawFile);
                        if (res < 1) {
                            fprintf(stderr, "failed to write 1 byte for %s\n", outfile);
                            goto fin;
                        }
                        ptr++;
                    }
                }
            } else if (image->comps[compno].sgnd == 0) {
                mask = (1 << image->comps[compno].prec) - 1;
                ptr = image->comps[compno].data;
                for (line = 0; line < h; line++) {
                    for (row = 0; row < w; row++)    {
                        curr = *ptr;
                        if (curr > 255) {
                            curr = 255;
                        } else if (curr < 0) {
                            curr = 0;
                        }
                        uc = (unsigned char)(curr & mask);
                        res = fwrite(&uc, 1, 1, rawFile);
                        if (res < 1) {
                            fprintf(stderr, "failed to write 1 byte for %s\n", outfile);
                            goto fin;
                        }
                        ptr++;
                    }
                }
            }
        } else if (image->comps[compno].prec <= 16) {
            if (image->comps[compno].sgnd == 1) {
                union {
                    signed short val;
                    signed char vals[2];
                } uc16;
                mask = (1 << image->comps[compno].prec) - 1;
                ptr = image->comps[compno].data;
                for (line = 0; line < h; line++) {
                    for (row = 0; row < w; row++)    {
                        curr = *ptr;
                        if (curr > 32767) {
                            curr = 32767;
                        } else if (curr < -32768) {
                            curr = -32768;
                        }
                        uc16.val = (signed short)(curr & mask);
                        res = fwrite(uc16.vals, 1, 2, rawFile);
                        if (res < 2) {
                            fprintf(stderr, "failed to write 2 byte for %s\n", outfile);
                            goto fin;
                        }
                        ptr++;
                    }
                }
            } else if (image->comps[compno].sgnd == 0) {
                union {
                    unsigned short val;
                    unsigned char vals[2];
                } uc16;
                mask = (1 << image->comps[compno].prec) - 1;
                ptr = image->comps[compno].data;
                for (line = 0; line < h; line++) {
                    for (row = 0; row < w; row++)    {
                        curr = *ptr;
                        if (curr > 65535) {
                            curr = 65535;
                        } else if (curr < 0) {
                            curr = 0;
                        }
                        uc16.val = (unsigned short)(curr & mask);
                        res = fwrite(uc16.vals, 1, 2, rawFile);
                        if (res < 2) {
                            fprintf(stderr, "failed to write 2 byte for %s\n", outfile);
                            goto fin;
                        }
                        ptr++;
                    }
                }
            }
        } else if (image->comps[compno].prec <= 32) {
            fprintf(stderr, "More than 16 bits per component no handled yet\n");
            goto fin;
        } else {
            fprintf(stderr, "Error: invalid precision: %d\n", image->comps[compno].prec);
            goto fin;
        }
    }
    fails = 0;
fin:
    fclose(rawFile);
    return fails;
}