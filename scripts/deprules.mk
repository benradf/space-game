#
# MMOEngine Package Dependencies
#

# libpng
$(call PKG_DEP,libpng,zlib)

# libmng
$(call PKG_DEP,libmng,jpeg)
$(call PKG_DEP,libmng,lcms)
$(call PKG_DEP,libmng,zlib)

# tiff
$(call PKG_DEP,tiff,jpeg)
$(call PKG_DEP,tiff,zlib)

# DevIL
$(call PKG_DEP,DevIL,jpeg)
$(call PKG_DEP,DevIL,lcms)
$(call PKG_DEP,DevIL,libpng)
$(call PKG_DEP,DevIL,libmng)
$(call PKG_DEP,DevIL,tiff)
$(call PKG_DEP,DevIL,zlib)

# zziplib
$(call PKG_DEP,zziplib,zlib)

# CEGUI
$(call PKG_DEP,CEGUI,DevIL)
$(call PKG_DEP,CEGUI,freetype)
$(call PKG_DEP,CEGUI,lua)
$(call PKG_DEP,CEGUI,pcre)
$(call PKG_DEP,CEGUI,xerces)

# ogre
$(call PKG_DEP,ogre,CEGUI)
$(call PKG_DEP,ogre,DevIL)
$(call PKG_DEP,ogre,ois)
$(call PKG_DEP,ogre,zziplib)

# net
$(call PKG_DEP,net,core)

# script
$(call PKG_DEP,script,core)

# physics
$(call PKG_DEP,physics,core)

# server
$(call PKG_DEP,server,argtable)
$(call PKG_DEP,server,core)
$(call PKG_DEP,server,enet)
$(call PKG_DEP,server,lua)
$(call PKG_DEP,server,math)
$(call PKG_DEP,server,net)
$(call PKG_DEP,server,physics)
$(call PKG_DEP,server,script)
$(call PKG_DEP,server,tvmet)

# client
$(call PKG_DEP,client,core)
$(call PKG_DEP,client,enet)
$(call PKG_DEP,client,lua)
$(call PKG_DEP,client,math)
$(call PKG_DEP,client,net)
$(call PKG_DEP,client,ogre)
$(call PKG_DEP,client,physics)
$(call PKG_DEP,client,script)
$(call PKG_DEP,client,tvmet)

# zonebuild
$(call PKG_DEP,zonebuild,argtable)
$(call PKG_DEP,zonebuild,core)
$(call PKG_DEP,zonebuild,expat)
$(call PKG_DEP,zonebuild,math)
$(call PKG_DEP,zonebuild,physics)

