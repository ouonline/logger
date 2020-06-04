project = CreateProject()

project:CreateBinary("test_logger"):AddDependencies(
    project:CreateDependency()
        :AddSourceFiles("*.c")
        :AddFlags("-Wall", "-Werror", "-Wextra")
        :AddStaticLibrary("..", "logger_static"))

return project
