/**
 * \file dylib.hpp
 * \brief Cross-platform Dynamic Library Loader
 * \author Martin Olivier
 * \version 1.7.0
 * 
 * MIT License
 * Copyright (c) 2022 Martin Olivier
 */

#pragma once

#include <string>
#include <functional>
#include <exception>
#include <utility>
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define DYLIB_API extern "C" __declspec(dllexport)
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#else
#define DYLIB_API extern "C"
#include <dlfcn.h>
#endif

/**
 *  The dylib class can hold a dynamic library instance and interact with it 
 *  by getting its symbols like functions or global variables
 */
class dylib
{
private:
#if defined(_WIN32) || defined(_WIN64)
    HINSTANCE m_handle{nullptr};
    static HINSTANCE open_lib(const char *path) noexcept
    {
        return LoadLibraryA(path);
    }
    FARPROC get_symbol(const char *name) const noexcept
    {
        return GetProcAddress(m_handle, name);
    }
    void close_lib() noexcept
    {
        FreeLibrary(m_handle);
    }
    static char *get_error_message() noexcept
    {
        constexpr size_t bufSize = 512;
        auto errorCode = GetLastError();
        if (!errorCode)
            return nullptr;
        static char msg[bufSize];
        auto lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        const DWORD len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, lang, msg, bufSize, nullptr);
        if (len > 0)
            return msg;
        return nullptr;
    }
#else
    void *m_handle{nullptr};
    static void *open_lib(const char *path) noexcept
    {
        return dlopen(path, RTLD_NOW | RTLD_LOCAL);
    }
    void *get_symbol(const char *name) const noexcept
    {
        return dlsym(m_handle, name);
    }
    void close_lib() noexcept
    {
        dlclose(m_handle);
    }
    static char *get_error_message() noexcept
    {
        return dlerror();
    }
#endif
    static std::string get_handle_error(const std::string &name)
    {
        auto err = get_error_message();
        if (!err)
            return "error while loading dynamic library \"" + name + "\"";
        return err;
    }
    static std::string get_symbol_error(const std::string &name)
    {
        auto err = get_error_message();
        if (!err)
            return "error while loading symbol \"" + name + "\"";
        return err;
    }

public:

#if defined(_WIN32) || defined(_WIN64)
    static constexpr auto extension = ".dll";
#elif defined(__APPLE__)
    static constexpr auto extension = ".dylib";
#else
    static constexpr auto extension = ".so";
#endif

    /**
     *  This exception is thrown when the dylib class encountered an error
     *
     *  @return the error message by calling what() member function
     */
    class exception : public std::exception
    {
    protected:
        const std::string m_error;
    public:
        explicit exception(std::string &&message) : m_error(std::move(message)) {}
        const char *what() const noexcept override {return m_error.c_str();}
    };

    /**
     *  This exception is thrown when the library failed to load 
     *  or encountered symbol resolution issues
     *
     *  @param message the error message
     */
    class handle_error : public exception
    {
    public:
        explicit handle_error(std::string &&message) : exception(std::move(message)) {}
    };

    /**
     *  This exception is thrown when the library failed to load a symbol. 
     *  This usually happens when you forgot to put <DYLIB_API> before a library function or variable
     *
     *  @param message the error message
     */
    class symbol_error : public exception
    {
    public:
        explicit symbol_error(std::string &&message) : exception(std::move(message)) {}
    };

    dylib(const dylib&) = delete;
    dylib& operator=(const dylib&) = delete;

    dylib(dylib &&other) noexcept
    {
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }

    dylib& operator=(dylib &&other) noexcept
    {
        if (this != &other) {
            close();
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    dylib() noexcept = default;

    /**
     *  Creates a dynamic library instance
     *
     *  @param path path to the dynamic library to load
     *  @param ext use dylib::extension to specify the os extension (optional parameter)
     */
    explicit dylib(const char *path)
    {
        open(path);
    }

    explicit dylib(const std::string &path)
    {
        open(path.c_str());
    }

    dylib(std::string path, const char *ext)
    {
        open(std::move(path), ext);
    }

    ~dylib()
    {
        close();
    }

    /**
     *  Load a dynamic library into the object. 
     *  If a dynamic library was already opened, it will be unload and replaced
     *
     *  @param path the path of the dynamic library to load
     *  @param ext use dylib::extension to detect the current os extension (optional parameter)
     */
    void open(const char *path)
    {
        close();
        if (!path)
            throw handle_error(get_handle_error("(nullptr)"));
        m_handle = open_lib(path);
        if (!m_handle)
            throw handle_error(get_handle_error(path));
    }

    void open(const std::string &path)
    {
        open(path.c_str());
    }

    void open(std::string path, const char *ext)
    {
        close();
        if (!ext)
            throw handle_error("bad extension : (nullptr)");
        path += ext;
        m_handle = open_lib(path.c_str());
        if (!m_handle)
            throw handle_error(get_handle_error(path));
    }

    /**
     *  Get a function from the dynamic library currently loaded in the object
     *
     *  @param T the template argument must be the function prototype. 
     *  it must be the same pattern as the template of std::function
     *  @param name the symbol name of the function to get from the dynamic library
     *
     *  @returns std::function<T> that contains the function
     */
    template<typename T>
    std::function<T> get_function(const char *name) const
    {
        if (!m_handle)
            throw handle_error("error : no dynamic library loaded");
        if (!name)
            throw symbol_error(get_symbol_error("(nullptr)"));
        auto sym = get_symbol(name);
        if (!sym)
            throw symbol_error(get_symbol_error(name));
        return reinterpret_cast<T *>(sym);
    }

    template<typename T>
    std::function<T> get_function(const std::string &name) const
    {
        return get_function<T>(name.c_str());
    }

    /**
     *  Get a global variable from the dynamic library currently loaded in the object
     *
     *  @param T type of the global variable
     *  @param name the name of the global variable to get from the dynamic library
     *
     *  @returns global variable of type <T>
     */
    template<typename T>
    T &get_variable(const char *name) const
    {
        if (!m_handle)
            throw handle_error("error : no dynamic library loaded");
        if (!name)
            throw symbol_error(get_symbol_error("(nullptr)"));
        auto sym = get_symbol(name);
        if (!sym)
            throw symbol_error(get_symbol_error(name));
        return *reinterpret_cast<T *>(sym);
    }

    template<typename T>
    T &get_variable(const std::string &name) const
    {
        return get_variable<T>(name.c_str());
    }

    /**
     *  Close the dynamic library currently loaded in the object. 
     *  This function will be automatically called by the class destructor
     */
    void close() noexcept
    {
        if (m_handle) {
            close_lib();
            m_handle = nullptr;
        }
    }
};