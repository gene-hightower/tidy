#include "tidy.hpp"

int main()
{
  auto const input = "<title>Foo</title><p>Foo!";

  tidy::buffer output;
  tidy::buffer errbuf;

  int rc = -1;

  tidy::doc tdoc{};

  std::cout << "Tidying:\t" << input << '\n';

  auto ok = tdoc.opt_set_bool(TidyXhtmlOut, true);
  if (ok)
    rc = tdoc.set_error_buffer(errbuf);
  if (rc >= 0)
    rc = tdoc.parse_string(input);
  if (rc >= 0)
    rc = tdoc.clean_and_repair();
  if (rc >= 0)
    rc = tdoc.run_diagnostics();
  if (rc > 1)
    rc = (tdoc.opt_set_bool(TidyForceOutput, true) ? rc : -1);
  if (rc >= 0)
    rc = tdoc.save_buffer(output);

  if (rc >= 0) {
    if (rc > 0)
      std::cout << "\nDiagnostics:\n\n" << errbuf;

    std::cout << "\nAnd here is the result:\n\n" << output;
  }
  else {
    std::cerr << "A severe error (" << rc << ") occurred.\n";
    return 1;
  }
}
