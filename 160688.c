int main(int argc, char *argv[]) {
  GooString *fileName;
  UnicodeMap *uMap;
  GooString *ownerPW, *userPW;
  PDFDoc *doc;
  char uBuf[8];
  char path[1024];
  char *p;
  bool ok;
  int exitCode;
  GooList *embeddedFiles = nullptr;
  int nFiles, nPages, n, i, j;
  FileSpec *fileSpec;
  Page *page;
  Annots *annots;
  Annot *annot;
  const GooString *s1;
  Unicode u;
  bool isUnicode;

  Win32Console win32Console(&argc, &argv);
  exitCode = 99;

  // parse args
  ok = parseArgs(argDesc, &argc, argv);
  if ((doList ? 1 : 0) +
      ((saveNum != 0) ? 1 : 0) +
      (saveAll ? 1 : 0) != 1) {
    ok = false;
  }
  if (!ok || argc != 2 || printVersion || printHelp) {
    fprintf(stderr, "pdfdetach version %s\n", PACKAGE_VERSION);
    fprintf(stderr, "%s\n", popplerCopyright);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdfdetach", "<PDF-file>", argDesc);
    }
    goto err0;
  }
  fileName = new GooString(argv[1]);

  // read config file
  globalParams = new GlobalParams();
  if (textEncName[0]) {
    globalParams->setTextEncoding(textEncName);
  }

  // get mapping to output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    error(errConfig, -1, "Couldn't get text encoding");
    delete fileName;
    goto err1;
  }

  // open PDF file
  if (ownerPassword[0] != '\001') {
    ownerPW = new GooString(ownerPassword);
  } else {
    ownerPW = nullptr;
  }
  if (userPassword[0] != '\001') {
    userPW = new GooString(userPassword);
  } else {
    userPW = nullptr;
  }

  doc = PDFDocFactory().createPDFDoc(*fileName, ownerPW, userPW);

  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    exitCode = 1;
    goto err2;
  }

  embeddedFiles = new GooList();
  for (i = 0; i < doc->getCatalog()->numEmbeddedFiles(); ++i)
    embeddedFiles->push_back(doc->getCatalog()->embeddedFile(i));

  nPages = doc->getCatalog()->getNumPages();
  for (i = 0; i < nPages; ++i) {
    page = doc->getCatalog()->getPage(i + 1);
    if (!page)
      continue;
    annots = page->getAnnots();
    if (!annots)
      break;

    for (j = 0; j < annots->getNumAnnots(); ++j) {
      annot = annots->getAnnot(j);
      if (annot->getType() != Annot::typeFileAttachment)
        continue;
      embeddedFiles->push_back(new FileSpec(static_cast<AnnotFileAttachment *>(annot)->getFile()));
    }
  }

  nFiles = embeddedFiles->getLength();

  // list embedded files
  if (doList) {
    printf("%d embedded files\n", nFiles);
    for (i = 0; i < nFiles; ++i) {
      fileSpec = static_cast<FileSpec *>(embeddedFiles->get(i));
      printf("%d: ", i+1);
      s1 = fileSpec->getFileName();
      if (!s1) {
	exitCode = 3;
	goto err2;
      }
      if (s1->hasUnicodeMarker()) {
        isUnicode = true;
        j = 2;
      } else {
        isUnicode = false;
        j = 0;
      }
      while (j < s1->getLength()) {
        if (isUnicode) {
          u = ((s1->getChar(j) & 0xff) << 8) | (s1->getChar(j+1) & 0xff);
          j += 2;
        } else {
          u = pdfDocEncoding[s1->getChar(j) & 0xff];
          ++j;
        }
        n = uMap->mapUnicode(u, uBuf, sizeof(uBuf));
        fwrite(uBuf, 1, n, stdout);
      }
      fputc('\n', stdout);
    }

  // save all embedded files
  } else if (saveAll) {
    for (i = 0; i < nFiles; ++i) {
      fileSpec = static_cast<FileSpec *>(embeddedFiles->get(i));
      if (savePath[0]) {
	n = strlen(savePath);
	if (n > (int)sizeof(path) - 2) {
	  n = sizeof(path) - 2;
	}
	memcpy(path, savePath, n);
	path[n] = '/';
	p = path + n + 1;
      } else {
	p = path;
      }
      s1 = fileSpec->getFileName();
      if (!s1) {
	exitCode = 3;
	goto err2;
      }
      if (s1->hasUnicodeMarker()) {
        isUnicode = true;
        j = 2;
      } else {
        isUnicode = false;
        j = 0;
      }
      while (j < s1->getLength()) {
        if (isUnicode) {
          u = ((s1->getChar(j) & 0xff) << 8) | (s1->getChar(j+1) & 0xff);
          j += 2;
        } else {
          u = pdfDocEncoding[s1->getChar(j) & 0xff];
          ++j;
        }
        n = uMap->mapUnicode(u, uBuf, sizeof(uBuf));
        if (p + n >= path + sizeof(path))
          break;
        memcpy(p, uBuf, n);
        p += n;
      }
      *p = '\0';

      auto *embFile = fileSpec->getEmbeddedFile();
      if (!embFile || !embFile->isOk()) {
	exitCode = 3;
	goto err2;
      }
      if (!embFile->save(path)) {
	error(errIO, -1, "Error saving embedded file as '{0:s}'", p);
	exitCode = 2;
	goto err2;
      }
    }

  // save an embedded file
  } else {
    if (saveNum < 1 || saveNum > nFiles) {
      error(errCommandLine, -1, "Invalid file number");
      goto err2;
    }

    fileSpec = static_cast<FileSpec *>(embeddedFiles->get(saveNum - 1));
    if (savePath[0]) {
      p = savePath;
    } else {
      p = path;
      s1 = fileSpec->getFileName();
      if (!s1) {
	exitCode = 3;
	goto err2;
      }
      if (s1->hasUnicodeMarker()) {
        isUnicode = true;
        j = 2;
      } else {
        isUnicode = false;
        j = 0;
      }
      while (j < s1->getLength()) {
        if (isUnicode) {
          u = ((s1->getChar(j) & 0xff) << 8) | (s1->getChar(j+1) & 0xff);
          j += 2;
        } else {
          u = pdfDocEncoding[s1->getChar(j) & 0xff];
          ++j;
        }
        n = uMap->mapUnicode(u, uBuf, sizeof(uBuf));
        if (p + n >= path + sizeof(path))
          break;
        memcpy(p, uBuf, n);
        p += n;
      }
      *p = '\0';
      p = path;
    }

    auto *embFile = fileSpec->getEmbeddedFile();
    if (!embFile || !embFile->isOk()) {
      exitCode = 3;
      goto err2;
    }
    if (!embFile->save(p)) {
      error(errIO, -1, "Error saving embedded file as '{0:s}'", p);
      exitCode = 2;
      goto err2;
    }
  }

  exitCode = 0;

  // clean up
 err2:
  if (embeddedFiles)
    deleteGooList<FileSpec>(embeddedFiles);
  uMap->decRefCnt();
  delete doc;
 err1:
  delete globalParams;
 err0:

  return exitCode;
}