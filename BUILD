package(default_visibility = ["//visibility:public"])

# TODO provide a generated file with every SHA
genrule(
    name = "generate_version",
    srcs = ["VERSION.txt"],
    outs = ["include/madara/Version.h"],
    cmd = "echo 'std::string version (\"'`cat $(location VERSION.txt)`' (SHA NOT SUPPORTED)\");\n' > $@",
)

cc_library(
    name = "cereal",
    hdrs = glob(
        [
            "include/cereal/**/*.h",
            "include/cereal/**/*.hpp",
        ],
    ),
    copts = ["-w"],
    strip_include_prefix = "include",
)

NAMES = ["madara", "madara_python"]
DEFINES = [[], ["_MADARA_PYTHON_CALLBACKS_"]]
DEPS = [[],["@org_boost_boost//:python"]]

[cc_library(
    name = NAME,
    srcs = glob(
               [
                   "include/**/*.h",
                   "include/**/*.cpp",
               ],
               exclude = [
                   "include/madara/transport/splice/*",
                   "include/madara/transport/ndds/*",
                   "include/madara/transport/zmq/*",
                   "include/cereal/**/*",
                   "include/madara/cid/*",
                   "include/madara/kats/*",
                   "include/pugi/*",
                   "include/**/java/*",
               ],
           ) + ["include/madara/Version.h"] +
           select({
               "@bazel_module//bazel_rules:zmq": glob([
                   "include/madara/transport/zmq/*.cpp",
                   "include/madara/transport/zmq/*.h",
               ]),
               "//conditions:default": [],
           }),
    hdrs = glob(
        [
            "include/**/*.h",
            "include/**/*.inl",
            "include/**/*.cpp",
        ],
        exclude = [
            "include/cereal/**/*",
        ],
    ) + ["include/madara/Version.h"],
    copts = ["-w"],
    defines = [
        "MADARA_FEATURE_SIMTIME",
    ] + select({
        "@bazel_module//bazel_rules:zmq": ["_MADARA_USING_ZMQ_"],
        "//conditions:default": [],
    }) + DEFINE,
    linkopts = ["-pthread"],
    strip_include_prefix = "include",
    textual_hdrs = glob(["include/**/*.inl"]),
    deps = [
        "@org_boost_boost//:boost",
        "@org_capnproto_capnproto//:capnp-lib",
        ":cereal",
    ] + select({
        "@bazel_module//bazel_rules:zmq": ["@org_zeromq_libzmq//:libzmq"],
        "//conditions:default": [],
    }) + DEP,
) for NAME, DEFINE, DEP in zip(NAMES, DEFINES, DEPS)]

cc_library(
    name = "madara_jni_h",
    srcs = glob(["port/java/jni/*.h"]),
    hdrs = glob(["port/java/jni/*.h"]),
    copts = ["-w"],
    strip_include_prefix = "port/java/jni",
)

cc_binary(
    name = "karl",
    srcs = ["tools/karl.cpp"],
    copts = ["-w"],
    deps = [":madara"],
)

cc_library(
    name = "jni_headers",
    srcs = [
        "@local_jdk//:jni_header",
        "@local_jdk//:jni_md_header-linux",
    ],
    hdrs = [
        "@local_jdk//:jni_header",
        "@local_jdk//:jni_md_header-linux",
    ],
    copts = ["-w"],
    linkstatic = 1,
    visibility = [
        "//visibility:public",
    ],
)

# JAR java library
java_library(
    name = "madara_java",
    srcs = glob(
        ["port/java/src/main/java/ai/madara/**/*.java"],
        exclude = ["port/java/src/main/java/ai/madara/tests/**/*.java"],
    ),
    deps = [
        "@org_capnproto//jar",
    ],
)

# Dependency including source files + the java support
# We can't depend on :madara here, since the java ports
# include the headers using relative path (such `../Buffers.h`),
# making it impossible to use headers provided by other libraries.
cc_library(
    name = "madara_jni",
    srcs = glob(
        [
            "include/**/*.h",
            "include/**/*.cpp",
        ],
        exclude = [
            "include/madara/transport/splice/*",
            "include/madara/transport/ndds/*",
            "include/madara/transport/zmq/*",
            "include/madara/cid/*",
            "include/madara/kats/*",
            "include/pugi/*",
        ],
    ) + ["include/madara/Version.h"],
    hdrs = glob([
        "include/**/*.h",
        "include/**/*.inl",
        "include/**/*.cpp",
    ]) + ["include/madara/Version.h"],
    copts = [
        "-I external/local_jdk/include",
        "-I external/local_jdk/include/linux",
        "-w",
    ],
    defines = ["_MADARA_JAVA_"],
    linkstatic = 1,
    strip_include_prefix = "include",
    textual_hdrs = glob(["include/**/*.inl"]),
    deps = [
        ":cereal",
        ":jni_headers",
        ":madara_jni_h",
        "@org_boost_boost//:boost",
        "@org_capnproto_capnproto//:capnp-lib",
    ],
)

# Shared binary with the JNI support
cc_binary(
    name = "libMADARA.so",
    srcs = glob([
        "port/java/jni/*.cpp",
        "port/java/jni/*.h",
    ]) + [
        "@local_jdk//:jni_header",
        "@local_jdk//:jni_md_header-linux",
    ],
    copts = [
        "-I external/local_jdk/include",
        "-I external/local_jdk/include/linux",
        "-w",
    ],
    linkshared = 1,
    deps = [":madara_jni"],
)
