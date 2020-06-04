project = CreateProject()

dep = project:CreateDependency()
    :AddSourceFiles("*.c")
    :AddFlags("-Wall", "-Werror", "-Wextra", "-fPIC")
    :AddStaticLibrary("../utils", "utils_static")
    :AddSysLibraries("pthread")

project:CreateStaticLibrary("logger_static"):AddDependencies(dep)

project:CreateSharedLibrary("logger_shared"):AddDependencies(dep)

return project
