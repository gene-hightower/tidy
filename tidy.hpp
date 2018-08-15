#ifndef TIDY_DOT_HPP_INCLUDED
#define TIDY_DOT_HPP_INCLUDED

#include <iostream>
#include <optional>

#include <tidy.h>
#include <tidybuffio.h>

#include <glog/logging.h>

namespace tidy {

class doc;

class buffer {
public:
  buffer(buffer const&) = delete;
  buffer& operator=(buffer const&) = delete;

  // move only
  buffer(buffer&& other)
  {
    buffer_ = other.buffer_;
    other.buffer_.bp = nullptr;
  }
  buffer& operator=(buffer&& rhs)
  {
    buffer_ = rhs.buffer_;
    rhs.buffer_.bp = nullptr;
    return *this;
  }

  buffer()
    : buffer_{0}
  {
  }
  ~buffer()
  {
    if (buffer_.bp)
      tidyBufFree(&buffer_);
  }

  unsigned char const* bp() const { return buffer_.bp; }

private:
  TidyBuffer buffer_;

  explicit buffer(TidyBuffer b)
    : buffer_(b)
  {
  }

  friend std::ostream& operator<<(std::ostream& os, buffer const& buf)
  {
    return os << buf.buffer_.bp;
  }

  friend doc;
};

class doc {
public:
  doc(doc const&) = delete;
  doc(doc&&) = delete;
  doc& operator=(doc const&) = delete;
  doc& operator=(doc&&) = delete;

  doc()
    : doc_{CHECK_NOTNULL(tidyCreate())}
  {
  }
  ~doc() { tidyRelease(doc_); }

  bool opt_set_bool(TidyOptionId id, bool val) const
  {
    return yes == tidyOptSetBool(doc_, id, val ? yes : no);
  }

  bool opt_set_int(TidyOptionId id, int val) const
  {
    return yes == tidyOptSetInt(doc_, id, val);
  }

  // clang-format off
  int set_error_buffer(buffer& err)   const { return tidySetErrorBuffer(doc_, &err.buffer_); }
  int parse_string(char const* input) const { return tidyParseString   (doc_, input); }
  int clean_and_repair()              const { return tidyCleanAndRepair(doc_); }
  int run_diagnostics()               const { return tidyRunDiagnostics(doc_); }
  int save_buffer(buffer& output)     const { return tidySaveBuffer    (doc_, &output.buffer_); }
  // clang-format on

  TidyNode get_root() const { return tidyGetRoot(doc_); }
  TidyNode get_body() const { return tidyGetBody(doc_); }

  std::optional<buffer> node_get_value(TidyNode node) const
  {
    TidyBuffer value{0};
    if (tidyNodeGetValue(doc_, node, &value)) {
      return buffer(value);
    }
    return {};
  }

  std::optional<buffer> node_get_text(TidyNode node) const
  {
    TidyBuffer text{0};
    if (tidyNodeGetValue(doc_, node, &text)) {
      return buffer(text);
    }
    return {};
  }

  TidyDoc get_tdoc() { return doc_; }

private:
  TidyDoc doc_;
};

TidyNode get_child(TidyNode n) { return tidyGetChild(n); }
TidyNode get_next(TidyNode n) { return tidyGetNext(n); }
char const* node_get_name(TidyNode n) { return tidyNodeGetName(n); }
TidyNodeType node_get_type(TidyNode n) { return tidyNodeGetType(n); }

} // namespace tidy

#endif // TIDY_DOT_HPP_INCLUDED
