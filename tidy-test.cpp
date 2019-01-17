#include "tidy.hpp"

#include <stdio.h>

using namespace std::string_literals;

void dumpNode(tidy::doc& tdoc, TidyNode tnod, int indent)
{
  for (auto child = tidy::get_child(tnod); child;
       child = tidy::get_next(child)) {
    char const* n = tidy::node_get_name(child);
    char const* name{nullptr};

    // clang-format off
    switch (tidy::node_get_type(child)) {
    case TidyNode_Root:    name = "Root";                   break;
    case TidyNode_DocType: name = "DOCTYPE";                break;
    case TidyNode_Comment: name = "Comment";                break;
    case TidyNode_ProcIns: name = "Processing Instruction"; break;
    case TidyNode_Text:    name = "Text";                   break;
    case TidyNode_CDATA:   name = "CDATA";                  break;
    case TidyNode_Section: name = "XML Section";            break;
    case TidyNode_Asp:     name = "ASP";                    break;
    case TidyNode_Jste:    name = "JSTE";                   break;
    case TidyNode_Php:     name = "PHP";                    break;
    case TidyNode_XmlDecl: name = "XML Declaration";        break;

    case TidyNode_Start:
    case TidyNode_End:
    case TidyNode_StartEnd:
    default:               name = n;                         break;
    }
    // clang-format on
    CHECK_NOTNULL(name);

    printf("\%*.*sNode:  \%s\n", indent, indent, " ", name);

    auto value = tdoc.node_get_value(child);
    if (value) {
      printf("\%*.*sValue: \%s\n", indent, indent, " ", value->bp());
    }
    auto text = tdoc.node_get_text(child);
    if (text) {
      printf("\%*.*sText: \%s\n", indent, indent, " ", text->bp());
    }

    for (auto attr = tidy::attr_first(child); attr;
         attr = tidy::attr_next(attr)) {
      if ("href"s == tidyAttrName(attr)) {
        printf("\%*.*sattr name: \%s\n", indent, indent, " ",
               tidyAttrName(attr));
        printf("\%*.*sattr value: \%s\n", indent, indent, " ",
               tidyAttrValue(attr));
      }
    }

    dumpNode(tdoc, child, indent + 4);
  }
}

void dumpDoc(tidy::doc& tdoc)
{
  dumpNode(tdoc, tdoc.get_root(), 0);
  printf("\n");
}

void dumpBody(tidy::doc& tdoc)
{
  dumpNode(tdoc, tdoc.get_body(), 0);
  printf("\n");
}

int main()
{
  auto const input = u8R"EOS(<!DOCTYPE html>
<html lang="en">
 <head>
  <title>Sample page</title>
 </head>
 <body>
  <h1>Sample page</h1>
  <p>This is a <a href="demo.&#x68;tml">simple</a> sample.</p>
  <!-- this is a comment -->
 </body>
</html>)EOS";

  tidy::buffer output;
  tidy::buffer errbuf;

  int rc = -1;

  tidy::doc tdoc{};

  // std::cout << "Tidying:\t" << input << '\n';

  // auto ok = tdoc.opt_set_bool(TidyXhtmlOut, true);
  auto ok = tdoc.opt_set_bool(TidyHtmlOut, true);
  if (ok)
    ok = tdoc.opt_set_int(TidyWrapLen, 0xFFFFFFFF);
  if (ok)
    ok = tdoc.opt_set_bool(TidyMark, false);
  if (ok)
    ok = tdoc.opt_set_bool(TidyIndentContent, true);
  if (ok)
    rc = tdoc.set_error_buffer(errbuf);
  if (rc >= 0)
    rc = tdoc.parse_string(input);

  // dumpDoc(tdoc);

  if (rc >= 0)
    rc = tdoc.clean_and_repair();
  if (rc >= 0)
    rc = tdoc.run_diagnostics();
  if (rc > 1)
    rc = (tdoc.opt_set_bool(TidyForceOutput, true) ? rc : -1);
  if (rc >= 0)
    rc = tdoc.save_buffer(output);

  dumpDoc(tdoc);

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
