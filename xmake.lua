add_rules("mode.debug", "mode.release")

target("ezcli")
    set_kind("static")
    set_languages("c++23")
    add_files("src/*.cpp")
    add_includedirs("include", {public = true})

-- includes("example")