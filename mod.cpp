/**
Copyright (c) 2013, Philip Deegan.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Philip Deegan nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "maiken/module/init.hpp"

#include <unordered_set>

namespace mkn {
namespace clang {

class FormatModule : public maiken::Module {
 private:
  std::string ref = "ref";

  const std::string& getRef1() { return ref; }
  const std::string& getRef2() { return ref; }

 protected:
  static void VALIDATE_NODE(const YAML::Node& node) {
    using namespace kul::yaml;
    Validator({NodeValidator("args"), NodeValidator("paths"),
               NodeValidator("style"), NodeValidator("types")})
        .validate(node);
  }
  void FORMAT(const kul::File&& f, const YAML::Node& node) {
    kul::Process p("clang-format");

    std::string clangHome(kul::env::GET("CLANG_HOME"));
    if (!clangHome.empty()) {
      kul::Dir cBin("bin", clangHome);
      if (!cBin) KEXCEPT(kul::Exception, "$CLANG_HOME/bin does not exist");
      kul::cli::EnvVar pa("PATH", cBin.real(), kul::cli::EnvVarMode::PREP);
      p.var(pa.name(), pa.toString());
    };

    p << "-i";
    if (!node["style"])
      p << "-style=file";
    else
      p << std::string("-style=" + node["style"].Scalar());
    p << f.escm();

    KLOG(DBG) << p;
    p.start();
  }
  void run(maiken::Application& a, const YAML::Node& node)
      KTHROW(std::exception) {
    VALIDATE_NODE(node);
    kul::os::PushDir pushd(a.project().dir());

    std::unordered_set<std::string> types;
    if (!node["types"]) {
      types = {"cpp", "cxx", "cc", "cc", "h", "hpp"};
    } else
      for (const auto& s : kul::String::SPLIT(node["types"].Scalar(), ":"))
        types.insert(s);

    std::unordered_set<std::string> files;

    const auto sources = a.sourceMap();
    for (const auto& p1 : sources)
      if (types.count(p1.first))
        for (const auto& p2 : p1.second)
          for (const auto& p3 : p2.second) files.insert(p3);

    if (node["paths"])
      for (const auto& path : kul::cli::asArgs(node["paths"].Scalar())) {
        kul::Dir d(path);
        if (!d)
          KEXCEPT(kul::fs::Exception, "Directory does not exist: ") << d.path();
        for (const auto& file : d.files(1)) {
          const std::string name = file.name();
          if (name.find(".") == std::string::npos) continue;
          const std::string type = name.substr(name.rfind(".") + 1);
          if (types.count(type)) files.insert(file.real());
        }
      }
    for (const auto& file : files) FORMAT(kul::File(file), node);
  }

 public:
  void init(maiken::Application& a, const YAML::Node& node)
      KTHROW(std::exception) override {
    run(a, node);
  }

  void link(maiken::Application& a, const YAML::Node& node)
      KTHROW(std::exception) override {
    run(a, node);
  }
};
}  // namespace clang
}  // namespace mkn

extern "C" KUL_PUBLISH maiken::Module* maiken_module_construct() {
  return new mkn ::clang ::FormatModule;
}

extern "C" KUL_PUBLISH void maiken_module_destruct(maiken::Module* p) {
  delete p;
}
