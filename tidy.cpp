#include "tidy.hpp"

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
