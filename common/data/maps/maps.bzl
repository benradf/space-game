def assemble_map(name):
    native.genrule(
        name = "collision-data",
        srcs = [":{name}/{name}.collision.xml".format(name = name)],
        cmd = "touch $(execpath {}.collision.dat)".format(name),
        outs = ["{}.collision.dat".format(name)],
        visibility = ["//visibility:public"],
    )
  #  native.genrule(
  #      name = "rendering-data",
  #      srcs = [":"],
  #  )
