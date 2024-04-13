void CLASS setCanonBodyFeatures (unsigned id)
      {
      imgdata.lens.makernotes.CamID = id;
	if (
            (id == 0x80000001) ||	// 1D
            (id == 0x80000174) ||	// 1D2
            (id == 0x80000232) ||	// 1D2N
            (id == 0x80000169) ||	// 1D3
            (id == 0x80000281)		// 1D4
            )
          {
            imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSH;
            imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF;
          }
        else
          if (
              (id == 0x80000167) ||	// 1Ds
              (id == 0x80000188) ||	// 1Ds2
              (id == 0x80000215) ||	// 1Ds3
              (id == 0x80000213) ||	// 5D
              (id == 0x80000218) ||	// 5D2
              (id == 0x80000285) ||	// 5D3
              (id == 0x80000302) ||	// 6D
              (id == 0x80000269) ||	// 1DX
              (id == 0x80000324) ||	// 1DC
              (id == 0x80000382) ||	// 5DS
              (id == 0x80000401)	// 5DS R
              )
            {
              imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_FF;
              imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF;
            }
          else
            if (
                (id == 0x80000331) ||	// M
                (id == 0x80000355) ||	// M2
                (id == 0x80000374) 	  	// M3
                )
              {
                imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
                imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF_M;
              }
            else
              if (
                  (id == 0x01140000) ||	// D30
                  (id == 0x01668000) ||	// D60
                  (id > 0x80000000)
                  )
                {
                  imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
                  imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Canon_EF;
                  imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Unknown;
                }
              else
                {
                  imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
                  imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
                }

	return;
      }