

#include "mkn/kul/signal.hpp"

#include <maiken.hpp>

const std::string yArgs = R"(
style: google
types: cpp:cxx:cc:h:hpp
paths: .
)";

int main(int argc, char* argv[]) {
  mkn::kul::Signal sig;
  try {
    YAML::Node node = mkn::kul::yaml::String(yArgs).root();
    char* argv2[2] = {argv[0], (char*)"-O"};
    auto app = (maiken::Application::CREATE(2, argv2))[0];
    auto loader(maiken::ModuleLoader::LOAD(*app));
    loader->module()->link(*app, node);
    loader->unload();
  } catch (const mkn::kul::Exception& e) {
    KERR << e.what();
    return 2;
  } catch (const std::exception& e) {
    KERR << e.what();
    return 3;
  } catch (...) {
    KERR << "UNKNOWN EXCEPTION TYPE CAUGHT";
    return 5;
  }
  return 0;
}
