int main(int ac, char *av[])
{
  int ret;
  int verbose = 0, print_sz = 0, print_unpack = 0, print_frame = 0,
	  print_wb = 0, use_map = 0, use_timing = 0;
  struct file_mapping mapping;
  int compact = 0, print_0 = 0, print_1 = 0, print_2 = 0;
  LibRaw MyCoolRawProcessor;
  char *filelistfile = NULL;
  char *outputfilename = NULL;
  FILE *outfile = stdout;
  std::vector <std::string> filelist;
  starttime_t started;

  filelist.reserve(ac - 1);

  for (int i = 1; i < ac; i++)
  {
	  if (av[i][0] == '-')
	  {
		  if (!strcmp(av[i], "-c"))	compact++;
		  if (!strcmp(av[i], "-v"))	verbose++;
		  if (!strcmp(av[i], "-w"))  print_wb++;
		  if (!strcmp(av[i], "-u"))  print_unpack++;
		  if (!strcmp(av[i], "-m"))  use_map++;
		  if (!strcmp(av[i], "-t"))  use_timing++;
		  if (!strcmp(av[i], "-s"))  print_sz++;
		  if (!strcmp(av[i], "-h"))	O.half_size = 1;
		  if (!strcmp(av[i], "-f"))	print_frame++;
		  if (!strcmp(av[i], "-0")) print_0++;
		  if (!strcmp(av[i], "-1")) print_1++;
		  if (!strcmp(av[i], "-2")) print_2++;
		  if (!strcmp(av[i], "-M")) MyCoolRawProcessor.imgdata.params.use_camera_matrix = 0;
		  if (!strcmp(av[i], "-L") && i < ac - 1)
		  {
			  filelistfile = av[i + 1];
			  i++;
		  }
		  if (!strcmp(av[i], "-o") && i < ac - 1)
		  {
			  outputfilename = av[i + 1];
			  i++;
		  }
		  continue;
	  }
	  else if (!strcmp(av[i], "+M"))
	  {
		  MyCoolRawProcessor.imgdata.params.use_camera_matrix = 3;
		  continue;
	  }
	  filelist.push_back(av[i]);
  }
  if(filelistfile)
  {
	  char *p;
	  char path[MAX_PATH + 1];
	  FILE *f = fopen(filelistfile, "r");
	  if (f)
	  {
		  while (fgets(path,MAX_PATH,f))
		  {
			  if ((p = strchr(path, '\n'))) *p = 0;
			  if ((p = strchr(path, '\r'))) *p = 0;
			  filelist.push_back(path);
		  }
		  fclose(f);
	  }
  }
  if (filelist.size()<1)
  {
	  print_usage(av[0]);
	  return 1;
  }
  if (outputfilename)
	  outfile = fopen(outputfilename, "wt");

  timer_start(started);
  for (int i = 0; i < filelist.size();i++)
  {
	  if (use_map)
	  {
		  create_mapping(mapping, filelist[i]);
		  if (!mapping.map)
		  {
			  fprintf(stderr, "Cannot map %s\n", filelist[i].c_str());
			  close_mapping(mapping);
			  continue;
		  }

		  if ((ret = MyCoolRawProcessor.open_buffer(mapping.map, mapping.fsize)) != LIBRAW_SUCCESS)
		  {
			  fprintf(stderr, "Cannot decode %s: %s\n", filelist[i].c_str(), libraw_strerror(ret));
			  close_mapping(mapping);
			  continue;
		  }
	  }
    else
        if ((ret = MyCoolRawProcessor.open_file(filelist[i].c_str())) != LIBRAW_SUCCESS)
		{
			fprintf(stderr, "Cannot decode %s: %s\n", filelist[i].c_str(), libraw_strerror(ret));
			continue; // no recycle, open_file will recycle
		}

	  if (use_timing)
	  {
		  /* nothing!*/
	  }
	  else if (print_sz)
		  print_szfun(outfile, MyCoolRawProcessor, filelist[i]);
	  else if (print_0)
		  print_0fun(outfile, MyCoolRawProcessor, filelist[i]);
	  else if (print_1)
		  print_1fun(outfile, MyCoolRawProcessor, filelist[i]);
	  else if (print_2)
		  print_2fun(outfile, MyCoolRawProcessor, filelist[i]);
	  else if (verbose)
		  print_verbose(outfile, MyCoolRawProcessor, filelist[i]);
	  else if (print_unpack)
		  print_unpackfun(outfile, MyCoolRawProcessor, print_frame, filelist[i]);
	  else if (print_wb)
		  print_wbfun(outfile, MyCoolRawProcessor, filelist[i]);
	  else if (compact)
		  print_compactfun(outfile, MyCoolRawProcessor, filelist[i]);
	  else
		  fprintf(outfile, "%s is a %s %s image.\n", filelist[i].c_str(), P1.make, P1.model);

    MyCoolRawProcessor.recycle();
	if (use_map)
		close_mapping(mapping);
  } // endfor

  if (use_timing && filelist.size() > 0)
	  print_timer(outfile, started, filelist.size());
  return 0;
}