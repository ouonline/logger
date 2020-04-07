project = CreateProject()

target = project:CreateBinary("test_logger")
target:AddSourceFiles("*.c")
target:AddLibrary("..", "logger", STATIC)

return project
