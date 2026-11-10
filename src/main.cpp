#include "pch.hpp"
#include "port/port.hpp"


int main(int argc, char** argv) {
  Str err_msg;
  try {

    /* TODO
    Port::init() // make window, init port

    while Port::is_ran():
      Port::update() // update input, calc dt
      while accum_update:
        Game::update()
      while accum_redraw:
        Game::redraw()
      Port::render_canvas()
      Port::sync() // double buffer, delay

    Port::finalize() */
    return EXIT_SUCCESS;

  } catch(cr<hpw::Error> err) { // ошибка игры
    err_msg = err.what();
  } catch(cr<std::bad_alloc> ba) { // кончилась оперативка
    err_msg = "Need more RAM";
  } catch(cr<std::exception> err) { // std-ошибка
    err_msg = Str{"STD Error: "} + err.what();
  } catch(...) { // нонейм
    err_msg = "Unknown error";
  }

  Port::process_error(err_msg);
  return EXIT_FAILURE;
}
