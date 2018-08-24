package(default_visibility = ["//visibility:public"])

# Python binding support
cc_binary(
    name = "pymadara.so",
    srcs = glob(
        [
            "src/*.h",
            "src/*.cpp",
        ],
    ),
    copts = [
        "-w",
    ],
    linkshared = 1,
    deps = [
        "@madara",
        "@org_boost_boost//:boost",
    ],
)

py_library(
    name = "pymadara",
    data = [
        ":pymadara.so",
    ],
    imports = ["."],
)
