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
$(call PKG_DEP,CEGUI,freetype)
$(call PKG_DEP,CEGUI,lua)
$(call PKG_DEP,CEGUI,pcre)
$(call PKG_DEP,CEGUI,xerces)

# net
$(call PKG_DEP,net,core)

# script
$(call PKG_DEP,script,core)

# server
$(call PKG_DEP,server,argtable)
$(call PKG_DEP,server,core)
$(call PKG_DEP,server,enet)
$(call PKG_DEP,server,lua)
$(call PKG_DEP,server,net)
$(call PKG_DEP,server,script)

# client
$(call PKG_DEP,client,core)
$(call PKG_DEP,client,net)
$(call PKG_DEP,client,script)
$(call PKG_DEP,client,lua)
$(call PKG_DEP,client,enet)

