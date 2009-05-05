#!BPY

"""
Name: 'MMO Collision Exporter'
Blender: 246
Group: 'Export'
Tooltip: 'Export collision data.'
"""

from Blender import Scene, Mesh, Window, sys
import BPyMessages

import Blender

idprops = Blender.Scene.GetCurrent().properties
if ("mmo_collision_export_file" in idprops):
	export_file = idprops["mmo_collision_export_file"]
else:
	export_file = "/tmp/collision.xml"

export_file_button = Blender.Draw.Create(export_file)

def gui_draw():
	global export_file_button
	Blender.BGL.glClear(Blender.BGL.GL_COLOR_BUFFER_BIT)
	export_file_button = Blender.Draw.String("Path: ", 1, 10, 20, 750, 20, export_file_button.val, 255, "File to export collision data to")
	Blender.Draw.PushButton("Export", 2, 760, 20, 100, 20, "Begin export")

def gui_event(event, value):
	if event == Blender.Draw.ESCKEY:
		Blender.Draw.Exit()
		return

def gui_button(event):
	if event == 2:
		do_export()

def face_collapse(me):
	face_act = me.faces[me.activeFace]
	for f in me.faces:
		if f.sel:
			f.uv = face_act.uv

def output_triangle(file, verts):
	file.write("    <triangle>\n")
	for vcoord in [ v.co for v in verts ]:
		file.write("        <vertex ")
		file.write("x=\"" + str(vcoord.x) + "\" ")
		file.write("y=\"" + str(vcoord.y) + "\" ")
		file.write("z=\"" + str(vcoord.z) + "\"/>\n")
	file.write("    </triangle>\n")

def do_export():
	export_file = export_file_button.val
	idprops["mmo_collision_export_file"] = export_file
	#dict = {}
	#dict['export_file'] = export_file
	#Blender.Registry.SetKey("mmo_collision", dict, True)

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
	
	file = open(export_file_button.val, "w")
	file.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n")
	file.write("<collidable>\n")

	for face in me.faces:
		v = face.verts
		output_triangle(file, [ v[0], v[1], v[2] ])
		if len(v) == 4:
			output_triangle(file, [ v[2], v[3], v[0] ])

	file.write("</collidable>\n")

	if is_editmode: Window.EditMode(1)


def main():
	Blender.Draw.Register(gui_draw, gui_event, gui_button)


# This lets you can import the script without running it
if __name__ == '__main__':
	main()
