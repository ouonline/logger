project = Project()

dep = project:CreateDependency()
    :AddSourceFiles("*.c")
    :AddFlags({"-Wall", "-Werror", "-Wextra", "-fPIC"})
    :AddStaticLibraries("../utils", "utils_static")
    :AddSysLibraries("pthread")

project:CreateStaticLibrary("logger_static"):AddDependencies(dep)
project:CreateSharedLibrary("logger_shared"):AddDependencies(dep)

return project
