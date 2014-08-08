# SurfaceTools gui init module
# (c) 2001 Juergen Riegel LGPL

class SurfaceToolsWorkbench ( Workbench ):
	"SurfaceTools workbench object"
	MenuText = "SurfaceTools"
	ToolTip = "SurfaceTools workbench"
	def Initialize(self):
		# load the module
		import SurfaceToolsGui
	def GetClassName(self):
		return "SurfaceToolsGui::Workbench"

Gui.addWorkbench(SurfaceToolsWorkbench())
