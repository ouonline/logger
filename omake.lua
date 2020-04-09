project = CreateProject()

dep = project:CreateDependency()
dep:AddSourceFiles("*.c")
dep:AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")
dep:AddStaticLibrary("../utils", "utils_static")
dep:AddSysLibraries("pthread")

a = project:CreateStaticLibrary("logger_static")
a:AddDependencies(dep)

so = project:CreateSharedLibrary("logger_shared")
so:AddDependencies(dep)

return project
