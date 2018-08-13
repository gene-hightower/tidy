#ifndef TIDY_DOT_HPP_INCLUDED
#define TIDY_DOT_HPP_INCLUDED

#include <iostream>

#include <tidy.h>
#include <tidybuffio.h>

namespace tidy {

class doc;

class buffer {
public:
  buffer(buffer const&) = delete;
  buffer(buffer&&) = delete;
  buffer& operator=(buffer const&) = delete;
  buffer& operator=(buffer&&) = delete;

  buffer()
    : buf_{0}
  {
  }
  ~buffer() { tidyBufFree(&buf_); }

  friend doc;

private:
  TidyBuffer buf_;

  friend std::ostream& operator<<(std::ostream& os, buffer const& buf)
  {
    return os << buf.buf_.bp;
  }
};

class doc {
public:
  doc(doc const&) = delete;
  doc(doc&&) = delete;
  doc& operator=(doc const&) = delete;
  doc& operator=(doc&&) = delete;

  doc()
    : doc_{tidyCreate();}
  {
  }
  ~doc() { tidyRelease(doc_); }

  bool opt_set_bool(TidyOptionId id, bool val)
  {
    return yes == tidyOptSetBool(doc_, id, val ? yes : no);
  }

  int set_error_buffer(buffer& err)
  {
    return tidySetErrorBuffer(doc_, &err.buf_);
  }
  int parse_string(char const* input) { return tidyParseString(doc_, input); }
  int clean_and_repair() { return tidyCleanAndRepair(doc_); }
  int run_diagnostics() { return tidyRunDiagnostics(doc_); }
  int save_buffer(buffer& output) { return tidySaveBuffer(doc_, &output.buf_); }

private:
  TidyDoc doc_;
};

} // namespace tidy

#endif // TIDY_DOT_HPP_INCLUDED
