project = CreateProject()

target = project:CreateLibrary("logger")
target:AddSourceFile("*.c")
target:AddStaticLibrary("../utils", "utils")
target:AddSystemDynamicLibraries("pthread")

return project
