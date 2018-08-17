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
    case TidyNode_Root:    name = "Root†";                   break;
    case TidyNode_DocType: name = "DOCTYPE†";                break;
    case TidyNode_Comment: name = "Comment†";                break;
    case TidyNode_ProcIns: name = "Processing Instruction†"; break;
    case TidyNode_Text:    name = "Text†";                   break;
    case TidyNode_CDATA:   name = "CDATA†";                  break;
    case TidyNode_Section: name = "XML Section†";            break;
    case TidyNode_Asp:     name = "ASP†";                    break;
    case TidyNode_Jste:    name = "JSTE†";                   break;
    case TidyNode_Php:     name = "PHP†";                    break;
    case TidyNode_XmlDecl: name = "XML Declaration†";        break;

    case TidyNode_Start:
    case TidyNode_End:
    case TidyNode_StartEnd:
    default:               name = n;                         break;
    }
    if (n == nullptr) {
      auto typ = tidy::node_get_type(child);
      printf("\%*.*sType:  \%d\n", indent, indent, " ", typ);
    }

    // clang-format on
    CHECK_NOTNULL(name);

    if (("a"s == name) || ("link"s == name) || ("script"s == name)) {

      printf("\%*.*sNode:  \%s\n", indent, indent, " ", name);
      printf("\%*.*sName:  \%s\n", indent, indent, " ", n ? n : "nullptr");

      auto value = tdoc.node_get_value(child);
      if (value) {
        printf("\%*.*sValue: \%s\n", indent, indent, " ", value->bp());
      }
      auto text = tdoc.node_get_text(child);
      if (text) {
        printf("\%*.*sText: \%s\n", indent, indent, " ", text->bp());
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
  auto const input = u8R"EOS(
<!DOCTYPE html>
<html>

  <head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <title>Challenge 2</title>
  <meta name="description" content="This site has lots of amusing gotchas for people who attempt to scrape it!
">

  <link rel="stylesheet" href="http://triplebyte.github.io/web-crawler-test-site/css/main.css">
  <link rel="canonical" href="http://triplebyte.github.io//web-crawler-test-site/test2/">
  <link rel="alternate" type="application/rss+xml" title="Triplebyte web crawler test site" href="http://triplebyte.github.io//web-crawler-test-site
/feed.xml">
</head>


  <body>

    <header class="site-header">

  <div class="wrapper">

    <a class="site-title" href="http://triplebyte.github.io/web-crawler-test-site/">Triplebyte web crawler test site</a>


  </div>

</header>


    <div class="page-content">
      <div class="wrapper">
        <h1 id="challenge-2">Challenge 2</h1>

<p>I am a paragraph!</p>

<p>Sometimes I am <a href="http://triplebyte.github.io/web-crawler-test-site/test2/cynical.html">overly cynical</a>, but sometimes I am <a href="htt
p://triplebyte.github.io/web-crawler-test-site/test2/page2.html#na&amp;#xEFve">overly naïve</a>.</p>


      </div>
    </div>

    <footer class="site-footer">

  <div class="wrapper">

    <h2 class="footer-heading">Triplebyte web crawler test site</h2>

    <div class="footer-col-wrapper">
      <div class="footer-col footer-col-1">
        <ul class="contact-list">
          <li>Triplebyte web crawler test site</li>
          <li><a href="mailto:buck@triplebyte.com">buck@triplebyte.com</a></li>
        </ul>
      </div>

      <div class="footer-col footer-col-3">
        <p>This site has lots of amusing gotchas for people who attempt to scrape it!
</p>
      </div>
    </div>

  </div>

</footer>


  </body>

</html>
)EOS";

  tidy::buffer output;
  tidy::buffer errbuf;

  int rc = -1;

  tidy::doc tdoc{};

  // std::cout << "Tidying:\t" << input << '\n';

  // auto ok = tdoc.opt_set_bool(TidyXhtmlOut, true);
  auto ok = tdoc.opt_set_bool(TidyXhtmlOut, true);
  if (ok)
    ok = tdoc.opt_set_int(TidyWrapLen, 0xFFFFFFFF);
  if (ok)
    ok = tdoc.opt_set_bool(TidyMark, false);
  if (ok)
    rc = tdoc.set_error_buffer(errbuf);
  if (rc >= 0)
    rc = tdoc.parse_string(input);

  dumpDoc(tdoc);

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
