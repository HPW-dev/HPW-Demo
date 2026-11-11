#include "pch.hpp"
#include "port/port.hpp"


int main(int argc, char** argv) {
  Str err_msg;
  try {

    Port::init();
    // TODO game init

    while (Port::is_ran()) {
      Port::update();

      /* TODO
      while accum_update:
        Game::update()

      Game::redraw() */
      Port::render_canvas();
      Port::sync();
    }

    Port::finalize();
    return EXIT_SUCCESS;

  } catch(cr<hpw::Error> err) { // ошибка игры
    err_msg = err.what();
  } catch(cr<std::bad_alloc> ba) { // кончилась оперативка
    err_msg = "Need more RAM";
  } catch(cr<std::exception> err) { // std-ошибка
    err_msg = Str{"STD-error: "} + err.what();
  } catch(...) { // нонейм
    err_msg = "Unknown error";
  }

  Port::process_error(err_msg);
  return EXIT_FAILURE;
}
