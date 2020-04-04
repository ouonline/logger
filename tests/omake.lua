project = CreateProject()

target = project:CreateBinary("test_logger")
target:AddSourceFile("*.c")
target:AddStaticLibrary("..", "logger")

return project
