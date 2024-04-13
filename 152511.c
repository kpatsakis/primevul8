int LibRaw::open_datastream(LibRaw_abstract_datastream *stream)
{

  if(!stream)
    return ENOENT;
  if(!stream->valid())
    return LIBRAW_IO_ERROR;
  recycle();

  try {
    ID.input = stream;
    SET_PROC_FLAG(LIBRAW_PROGRESS_OPEN);

    identify();

	if (!imgdata.idata.dng_version && !strcmp(imgdata.idata.make, "Leaf") && !strcmp(imgdata.idata.model, "Credo 50"))
	{
		imgdata.color.pre_mul[0] = 1.f / 0.3984f;
		imgdata.color.pre_mul[2] = 1.f / 0.7666f;
		imgdata.color.pre_mul[1] = imgdata.color.pre_mul[3] = 1.0;
	}

	// S3Pro DNG patch
	if(imgdata.idata.dng_version && !strcmp(imgdata.idata.make,"Fujifilm") && !strcmp(imgdata.idata.model,"S3Pro") && imgdata.sizes.raw_width == 4288 )
	{
		imgdata.sizes.left_margin++;
		imgdata.sizes.width--;
	}
	if(imgdata.idata.dng_version && !strcmp(imgdata.idata.make,"Fujifilm") && !strcmp(imgdata.idata.model,"S5Pro") && imgdata.sizes.raw_width == 4288 )
	{
		imgdata.sizes.left_margin++;
		imgdata.sizes.width--;
	}
	if(!imgdata.idata.dng_version && !strcmp(imgdata.idata.make,"Fujifilm") 
           && (!strncmp(imgdata.idata.model,"S20Pro",6) || !strncmp(imgdata.idata.model,"F700",4))
           )
	{
          imgdata.sizes.raw_width/=2;
          load_raw= &LibRaw::unpacked_load_raw_fuji_f700s20;
	}
	if(load_raw == &LibRaw::packed_load_raw && !strcasecmp(imgdata.idata.make,"Nikon")
		 && !libraw_internal_data.unpacker_data.load_flags
		 && (!strncasecmp(imgdata.idata.model,"D810",4) || !strcasecmp(imgdata.idata.model,"D4S"))
		 && libraw_internal_data.unpacker_data.data_size*2 == imgdata.sizes.raw_height*imgdata.sizes.raw_width*3)
	{
		libraw_internal_data.unpacker_data.load_flags = 80;
	}
	// Adjust BL for Sony A900/A850
    if(load_raw == &LibRaw::packed_load_raw && !strcasecmp(imgdata.idata.make,"Sony")) // 12 bit sony, but metadata may be for 14-bit range
      {
        if(C.maximum>4095)
          C.maximum = 4095;
        if(C.black > 256 || C.cblack[0] > 256)
          {
            C.black /=4;
            for(int c=0; c< 4; c++)
              C.cblack[c]/=4;
            for(int c=0; c< C.cblack[4]*C.cblack[5];c++)
              C.cblack[6+c]/=4;
          }
      }
    if(  load_raw == &LibRaw::nikon_yuv_load_raw  ) // Is it Nikon sRAW?
      {
           load_raw= &LibRaw::nikon_load_sraw;
           C.black =0;
           memset(C.cblack,0,sizeof(C.cblack));
           imgdata.idata.filters = 0;
           libraw_internal_data.unpacker_data.tiff_samples=3;
           imgdata.idata.colors = 3;
           double beta_1 = -5.79342238397656E-02;
           double beta_2 = 3.28163551282665;
           double beta_3 = -8.43136004842678;
           double beta_4 = 1.03533181861023E+01;
           for(int i=0; i<=3072;i++)
           {
               double x = (double)i/3072.;
               double y = (1.-exp(-beta_1*x-beta_2*x*x-beta_3*x*x*x-beta_4*x*x*x*x));
               if(y<0.)y=0.;
               imgdata.color.curve[i] = (y*16383.);
           }
           for(int i=0;i<3;i++)
             for(int j=0;j<4;j++)
               imgdata.color.rgb_cam[i][j]=float(i==j);
      }
    // Adjust BL for Nikon 12bit
    if((
        load_raw == &LibRaw::nikon_load_raw
        || load_raw == &LibRaw::packed_load_raw)
       && !strcasecmp(imgdata.idata.make,"Nikon")
       && strncmp(imgdata.idata.model,"COOLPIX",7)
//	   && strncmp(imgdata.idata.model,"1 ",2)
       && libraw_internal_data.unpacker_data.tiff_bps == 12)
      {
        C.maximum = 4095;
        C.black /=4;
        for(int c=0; c< 4; c++)
          C.cblack[c]/=4;
        for(int c=0; c< C.cblack[4]*C.cblack[5];c++)
          C.cblack[6+c]/=4;
      }

	// Adjust BL for Panasonic
    if(load_raw == &LibRaw::panasonic_load_raw && (!strcasecmp(imgdata.idata.make,"Panasonic") || !strcasecmp(imgdata.idata.make,"Leica"))
       &&  ID.pana_black[0] && ID.pana_black[1] && ID.pana_black[2])
      {
        C.black=0;
        C.cblack[0] = ID.pana_black[0]+ID.pana_black[3];
        C.cblack[1] = C.cblack[3] = ID.pana_black[1]+ID.pana_black[3];
        C.cblack[2] = ID.pana_black[2]+ID.pana_black[3];
        int i = C.cblack[3];
        for(int c=0; c<3; c++) if(i>C.cblack[c]) i = C.cblack[c];
        for(int c=0; c< 4; c++) C.cblack[c]-=i;
        C.black = i;
      }

    // Adjust sizes for X3F processing
    if(load_raw == &LibRaw::x3f_load_raw)
    {
        for(int i=0; i< foveon_count;i++)
            if(!strcasecmp(imgdata.idata.make,foveon_data[i].make) && !strcasecmp(imgdata.idata.model,foveon_data[i].model)
                && imgdata.sizes.raw_width == foveon_data[i].raw_width
                && imgdata.sizes.raw_height == foveon_data[i].raw_height
                )
            {
                imgdata.sizes.top_margin = foveon_data[i].top_margin;
                imgdata.sizes.left_margin = foveon_data[i].left_margin;
                imgdata.sizes.width = imgdata.sizes.iwidth = foveon_data[i].width;
                imgdata.sizes.height = imgdata.sizes.iheight = foveon_data[i].height;
                C.maximum = foveon_data[i].white;
                break;
            }
    }
#if 0
    size_t bytes = ID.input->size()-libraw_internal_data.unpacker_data.data_offset;
    float bpp = float(bytes)/float(S.raw_width)/float(S.raw_height);
    float bpp2 = float(bytes)/float(S.width)/float(S.height);
    printf("RawSize: %dx%d data offset: %d data size:%d bpp: %g bpp2: %g\n",S.raw_width,S.raw_height,libraw_internal_data.unpacker_data.data_offset,bytes,bpp,bpp2);
    if(!strcasecmp(imgdata.idata.make,"Hasselblad") && bpp == 6.0f)
      {
        load_raw = &LibRaw::hasselblad_full_load_raw;
        S.width = S.raw_width;
        S.height = S.raw_height;
        P1.filters = 0;
        P1.colors=3;
        P1.raw_count=1;
        C.maximum=0xffff;
        printf("3 channel hassy found\n");
      }
#endif
    if(C.profile_length)
      {
        if(C.profile) free(C.profile);
        C.profile = malloc(C.profile_length);
        merror(C.profile,"LibRaw::open_file()");
        ID.input->seek(ID.profile_offset,SEEK_SET);
        ID.input->read(C.profile,C.profile_length,1);
      }

    SET_PROC_FLAG(LIBRAW_PROGRESS_IDENTIFY);
  }
  catch ( LibRaw_exceptions err) {
    EXCEPTION_HANDLER(err);
  }
  catch (std::exception ee) {
    EXCEPTION_HANDLER(LIBRAW_EXCEPTION_IO_CORRUPT);
  }

  if(P1.raw_count < 1)
    return LIBRAW_FILE_UNSUPPORTED;


  write_fun = &LibRaw::write_ppm_tiff;

  if (load_raw == &LibRaw::kodak_ycbcr_load_raw)
    {
      S.height += S.height & 1;
      S.width  += S.width  & 1;
    }

  IO.shrink = P1.filters && (O.half_size ||
                             ((O.threshold || O.aber[0] != 1 || O.aber[2] != 1) ));

  S.iheight = (S.height + IO.shrink) >> IO.shrink;
  S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;

  // Save color,sizes and internal data into raw_image fields
  memmove(&imgdata.rawdata.color,&imgdata.color,sizeof(imgdata.color));
  memmove(&imgdata.rawdata.sizes,&imgdata.sizes,sizeof(imgdata.sizes));
  memmove(&imgdata.rawdata.iparams,&imgdata.idata,sizeof(imgdata.idata));
  memmove(&imgdata.rawdata.ioparams,&libraw_internal_data.internal_output_params,sizeof(libraw_internal_data.internal_output_params));

  SET_PROC_FLAG(LIBRAW_PROGRESS_SIZE_ADJUST);


  return LIBRAW_SUCCESS;
}