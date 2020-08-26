project = Project()

project:CreateBinary("test_logger"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("*.c")
        :AddFlags({"-Wall", "-Werror", "-Wextra"})
        :AddStaticLibraries("..", "logger_static"))

return project
