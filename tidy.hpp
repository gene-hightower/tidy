#ifndef TIDY_DOT_HPP_INCLUDED
#define TIDY_DOT_HPP_INCLUDED

#include <tidy.h>

namespace tidy {
class doc {
public:
  doc(doc const&) = delete;
  doc(doc&&) = delete;
  doc& operator=(doc const&) = delete;
  doc& operator=(doc&&) = delete;

  doc() { doc_ = tidyCreate(); }

  ~doc() { tidyRelease(doc_); }

private:
  TidyDoc doc_;
};

} // namespace tidy

#endif // TIDY_DOT_HPP_INCLUDED
