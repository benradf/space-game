load("@rules_pkg//pkg:zip.bzl", "pkg_zip")

def assemble_assets(name):

    # Convert xml mesh data to ogre binary format.
    mesh_converter = "@ogre//:bin/OgreXMLConverter"
    native.genrule(
        name = "convert_{}_mesh".format(name),
        srcs = [":{name}/{name}.mesh.xml".format(name = name)],
        tools = [mesh_converter],
        cmd = """$(execpath {mesh_converter}) \
            $(rootpath {name}/{name}.mesh.xml) \
            $(execpath {name}.mesh)
        """.format(
            name = name,
            mesh_converter = mesh_converter,
        ),
        outs = ["{}.mesh".format(name)],
        visibility = ["//visibility:public"],
    )

    # Package the assets into a zip.
    pkg_zip(
        name = "zip_{}_assets".format(name),
        out = name + ".zip",
        strip_prefix = name,
        srcs = native.glob([
            name + "/*.tga",
            name + "/*.material",
        ]) + [ ":{}.mesh".format(name) ],
        visibility = ["//visibility:public"],
    )

def assemble_map(name):
    assemble_assets(name)

    # Build kdtree for collision detection.
    native.genrule(
        name = "collision",
        srcs = [":{name}/{name}.collision.xml".format(name = name)],
        tools = ["//common/src:zonebuild"],
        cmd = """$(execpath //common/src:zonebuild) \
            $(rootpath {name}/{name}.collision.xml) \
            $(execpath {name}.dat)
        """.format(name = name),
        outs = ["{}.dat".format(name)],
        visibility = ["//visibility:public"],
    )



#def enumerate_resources():
#    config = []
#
#    config.append("[Bootstrap]")
#    config.append("Zip=ogrecore.zip")
#
#    config.append("[General]")
#    #for zip in native.glob([":data/maps/*.zip"]):
#    #for zip in native.glob(["data/maps/*/*"]):
#    for zip in native.glob(["src/*/*.hpp"]):
#        config.append("Zip=" + zip)
#    #print(str(native.glob(["**/*"])))
#
#    config.append("[CEGUI]")
#
##[Bootstrap]
##Zip=ogrecore.zip
##
##[General]
##find $1 -name "*.zip" -exec printf "Zip=%s\n" "{}" \; | sed "s;$1;;"
##
##[CEGUI]
##FileSystem=
#
#    return config
