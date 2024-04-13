uint64_t GetFileSize(const std::string& file_name) {
  uint64_t file_size = 0;
#ifndef _MSC_VER
  struct stat st;
  st.st_size = 0;
  if (stat(file_name.c_str(), &st) == 0) {
#else
  struct _stat st;
  st.st_size = 0;
  if (_stat(file_name.c_str(), &st) == 0) {
#endif
    file_size = st.st_size;
  }
  return file_size;
}

bool GetFileContents(const std::string& file_name, std::string* contents) {
  std::ifstream file(file_name.c_str());
  *contents = std::string(static_cast<size_t>(GetFileSize(file_name)), 0);
  if (file.good() && contents->size()) {
    file.read(&(*contents)[0], contents->size());
  }
  return !file.fail();
}

TempFileDeleter::TempFileDeleter() { file_name_ = GetTempFileName(); }

TempFileDeleter::~TempFileDeleter() {
  std::ifstream file(file_name_.c_str());
  if (file.good()) {
    file.close();
    std::remove(file_name_.c_str());
  }
}

}  // namespace libwebm