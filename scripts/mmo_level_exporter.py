#!BPY

"""
Name: 'MMO Level Exporter'
Blender: 246
Group: 'Export'
Tooltip: 'Export level and collision data.'
"""

from Blender import Scene, Mesh, Window, sys
import BPyMessages

def face_collapse(me):
	face_act = me.faces[me.activeFace]
	for f in me.faces:
		if f.sel:
			f.uv = face_act.uv

def output_triangle(file, verts):
	file.write("    <triangle>\n")
	for vcoord in [ v.co for v in verts ]:
		file.write("        <vertex \"")
		file.write("x=\"" + str(vcoord.x) + "\" ")
		file.write("y=\"" + str(vcoord.y) + "\" ")
		file.write("z=\"" + str(vcoord.z) + "\"/>\n")
	file.write("    </triangle>\n")

def main():
	
	# Gets the current scene, there can be many scenes in 1 blend file.
	sce = Scene.GetCurrent()
	
	# Get the active object, there can only ever be 1
	# and the active object is always the editmode object.
	ob_act = sce.objects.active
	me = ob_act.getData(mesh=1)
	
	if not ob_act or ob_act.type != 'Mesh':
		BPyMessages.Error_NoMeshUvActive()
		return 
	
	# Saves the editmode state and go's out of 
	# editmode if its enabled, we cant make
	# changes to the mesh data while in editmode.
	is_editmode = Window.EditMode()
	if is_editmode: Window.EditMode(0)
	
	file = open("/tmp/collision.xml", "w")

	for face in me.faces:
		v = face.verts
		output_triangle(file, [ v[0], v[1], v[2] ])
		if len(v) == 4:
			output_triangle(file, [ v[2], v[3], v[0] ])
	
	if is_editmode: Window.EditMode(1)
	
# This lets you can import the script without running it
if __name__ == '__main__':
	main()
