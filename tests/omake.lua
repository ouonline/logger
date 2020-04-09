project = CreateProject()

target = project:CreateBinary("test_logger")
target:AddSourceFiles("*.c")
target:AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")
target:AddStaticLibrary("..", "logger_static")

return project
