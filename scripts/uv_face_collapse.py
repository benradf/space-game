#!BPY

"""
Name: 'Collapse UV Faces'
Blender: 246
Group: 'UV'
Tooltip: 'Collapse selected UV faces onto active face.'
"""

from Blender import Scene, Mesh, Window, sys
import BPyMessages

def face_collapse(me):
	face_act = me.faces[me.activeFace]
	for f in me.faces:
		if f.sel:
			f.uv = face_act.uv

def main():
	
	# Gets the current scene, there can be many scenes in 1 blend file.
	sce = Scene.GetCurrent()
	
	# Get the active object, there can only ever be 1
	# and the active object is always the editmode object.
	ob_act = sce.objects.active
	me = ob_act.getData(mesh=1)
	
	if not ob_act or ob_act.type != 'Mesh' or not me.faceUV:
		BPyMessages.Error_NoMeshUvActive()
		return 
	
	# Saves the editmode state and go's out of 
	# editmode if its enabled, we cant make
	# changes to the mesh data while in editmode.
	is_editmode = Window.EditMode()
	if is_editmode: Window.EditMode(0)
	
	# Run the mesh editing function
	face_collapse(me)
	
	if is_editmode: Window.EditMode(1)
	
# This lets you can import the script without running it
if __name__ == '__main__':
	main()
