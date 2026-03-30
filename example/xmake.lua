add_rules("mode.debug", "mode.release")

target("ezcli_example")
    set_kind("binary")
    set_languages("c++23")
    add_files("example.cpp")
    add_deps("ezcli")
    add_includedirs("../include")