project = CreateProject()

dep = project:CreateDependency()
dep:AddSourceFiles("*.c")
dep:AddFlags("-Wall", "-Werror", "-Wextra")
dep:AddStaticLibrary("..", "logger_static")

target = project:CreateBinary("test_logger")
target:AddDependencies(dep)

return project
