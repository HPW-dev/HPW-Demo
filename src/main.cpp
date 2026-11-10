#include "pch.hpp"
#include "port/port.hpp"


int main(int argc, char** argv) {
  try {
    // TODO
    return EXIT_SUCCESS;
  } catch(cr<hpw::Error> err) {
    const Str err_str = err.what();
    Port::process_error(err_str);
  } catch(cr<std::bad_alloc> ba) {
    Port::process_error("Need more RAM");
  } catch(cr<std::exception> err) {
    const Str err_str = Str{"STD Error: "} + err.what();
    Port::process_error(err_str);
  } catch(...) {
    cauto err_str = "Unknown error";
    Port::process_error(err_str);
  }

  return EXIT_FAILURE;
}
