root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.Vm = {
    StartScript: 0,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.MO6TW,
	UseReturnIds: false
};

include('mo6tw/vfs.js');
include('mo6tw/sprites.js');
include('common/animation.js');
include('mo6tw/charset.js');
include('mo6tw/font.js');
include('mo6tw/dialogue.js');
include('mo6tw/hud/saveicon.js');
include('mo6tw/hud/loadingdisplay.js');
include('mo6tw/hud/datedisplay.js');
//include('mo6tw/hud/titlemenu.js');
//include('mo6tw/hud/mainmenu.js');
include('mo6tw/hud/sysmesboxdisplay.js');