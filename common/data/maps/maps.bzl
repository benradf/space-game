def assemble_map(name):

    # Build kdtree for collision detection.
    native.genrule(
        name = "collision-data",
        srcs = [":{name}/{name}.collision.xml".format(name = name)],
        tools = ["//common/src:zonebuild"],
        cmd = """$(execpath //common/src:zonebuild) \
            $(rootpath {name}/{name}.collision.xml) \
            $(execpath {name}.collision.dat)
        """.format(name = name),
        outs = ["{}.collision.dat".format(name)],
        visibility = ["//visibility:public"],
    )

    # Convert xml mesh data to ogre binary format.
    mesh_converter = "@ogre//:bin/OgreXMLConverter"
    native.genrule(
        name = "mesh",
        srcs = [":{name}/{name}.mesh.xml".format(name = name)],
        tools = [mesh_converter],
        cmd = """$(execpath {mesh_converter}) \
            $(rootpath {name}/{name}.mesh.xml) \
            $(execpath {name}.mesh.dat)
        """.format(
            name = name,
            mesh_converter = mesh_converter,
        ),
        outs = ["{}.mesh.dat".format(name)],
        visibility = ["//visibility:public"],
    )
