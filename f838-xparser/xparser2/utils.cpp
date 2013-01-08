#include <string>
#include <boost/filesystem.hpp>

namespace xparser { namespace utils {

std::string locate_template(const char* template_name) {
  const char* tmpl_dir = "xparser2/templates";  // template subdir
  
  // first, look for the file relative to the current working directory
  boost::filesystem::path cwd(boost::filesystem::current_path());
  boost::filesystem::path out = cwd / tmpl_dir / template_name;
  std::cout << "... looking in " << out << std::endl;
  if (boost::filesystem::exists(out) &&
      boost::filesystem::is_regular_file(out)) {
    return out.string();
  }

  // if package data install dir defined, look there next
  #ifdef PKGDATADIR
    boost::filesystem::path pkgdir(PKGDATADIR);
    out = pkgdir / tmpl_dir / template_name;
    std::cout << "... looking in " << out << std::endl;
    if (boost::filesystem::exists(out) &&
      boost::filesystem::is_regular_file(out)) {
    return out.string();
  }
  #endif

  std::cout << ".. not found!" << std::endl;
  return "";
}


}}  // namespace xparser::utils
