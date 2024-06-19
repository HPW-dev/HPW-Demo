#pragma once
#include "cmd.hpp"

// команда для выхода
class Cmd_exit final: public Cmd::Command {
public:
  ~Cmd_exit() = default;
  inline Str name() const override { return "exit"; }
  inline Str description() const override { return "exit from game";  }
  void exec(CN<Strs> cmd_and_args) override;
};

// команда кидающая hpw::Error исключение
class Cmd_error final: public Cmd::Command {
public:
  ~Cmd_error() = default;
  inline Str name() const override { return "error"; }
  inline Str description() const override { return "throw hpw::Error with message";  }
  void exec(CN<Strs> cmd_and_args) override;
};

// вывод текста в консоль и на экран
class Cmd_print final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_print(Cmd* master): m_master{master} {}
  ~Cmd_print() = default;
  inline Str name() const override { return "print"; }
  inline Str description() const override
    { return "print <message> - print text to screen & console"; }
  void exec(CN<Strs> cmd_and_args) override;
};

// назначить команде короткое имя
class Cmd_alias final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_alias(Cmd* master): m_master{master} {}
  ~Cmd_alias() = default;
  inline Str name() const override { return "alias"; }
  inline Str description() const override
    { return "alias <name> command args... - set short name for command + parameters";  }
  void exec(CN<Strs> cmd_and_args) override;
};

// включение/отключение вывода в косоль
class Cmd_log_cnosole final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_log_cnosole(Cmd* master): m_master{master} {}
  ~Cmd_log_cnosole() = default;
  inline Str name() const override { return "log_cnosole"; }
  inline Str description() const override
    { return "log_cnosole 1/0 - enable/disable printing text to console";  }
  void exec(CN<Strs> cmd_and_args) override;
};

// включение/отключение вывода на экран
class Cmd_log_screen final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_log_screen(Cmd* master): m_master{master} {}
  ~Cmd_log_screen() = default;
  inline Str name() const override { return "log_screen"; }
  inline Str description() const override
    { return "log_screen 1/0 - enable/disable printing text to game screen"; }
  void exec(CN<Strs> cmd_and_args) override;
};

// показать справку по всем командам
class Cmd_help final: public Cmd::Command {
  Cmd* m_master {};
public:
  inline Cmd_help(Cmd* master): m_master{master} {}
  ~Cmd_help() = default;
  inline Str name() const override { return "help"; }
  inline Str description() const override 
    { return "print this help"; }
  void exec(CN<Strs> cmd_and_args) override;
};

// очистка консоли от текста
class Cmd_cls final: public Cmd::Command {
public:
  ~Cmd_cls() = default;
  inline Str name() const override { return "cls"; }
  inline Str description() const override { return "clear log messages"; }
  void exec(CN<Strs> cmd_and_args) override;
};

// игнорируемый коммент
class Cmd_comment final: public Cmd::Command {
public:
  ~Cmd_comment() = default;
  inline Str name() const override { return "#"; }
  inline Str description() const override { return "start of one-line commentary"; }
  inline void exec(CN<Strs> cmd_and_args) override {}
};


