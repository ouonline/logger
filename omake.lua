project = CreateProject()

target = project:CreateLibrary("logger", STATIC | SHARED)
target:AddSourceFiles("*.c")
target:AddLibrary("../utils", "utils", STATIC)
target:AddSysLibraries("pthread")

return project
