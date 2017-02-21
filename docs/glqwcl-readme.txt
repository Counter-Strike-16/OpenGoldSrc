resolution options
------------------
We had dynamic resolution changing in glqwcl for a while, but every single 
opengl driver I tried it on messed up in one way or another, so it is now 
limited to startup time only.

glqwcl -window
This will start glqwcl in a window on your desktop instead of switching the 
screen to lower resolution and covering everything.

glqwcl -width 800 -height 600
Tries to run glqwcl at the specified resolution.  Combined with -window, it 
creates a desktop window that size, otherwise it tries to set a full screen 
resolution.

texture options
---------------

There is no way to flush already loaded textures, so it is best to change 
these options on the command line, or they will only take effect on some of 
the textures when you change levels.

OpenGL only allows textures to repeat on power of two boundaries (32, 64, 
128, etc), but software quake had a number of textures that repeated at 24 
or 96 pixel boundaries.  These need to be either stretched out to the next 
higher size, or shrunk down to the next lower.  By default, they are filtered 
down to the smaller size, but you can cause it to use the larger size if you 
really want by using: 

glqwcl +gl_round_down 0
This will generally run well on a normal 4 MB 3dfx card, but for other cards 
that have either worse texture management or slower texture swapping speeds, 
there are some additional settings that can drastically lower the amount of 
textures to be managed.

glqwcl +gl_picmip 1
This causes all textures to have one half the dimensions they otherwise would.  
This makes them blurry, but very small.  You can set this to 2 to make the 
textures one quarter the resolution on each axis for REALLY blurry textures.

glqwcl +gl_playermip 1
This is similar to picmip, but is only used for other players in deathmatch.  
Each player in a deathmatch requires an individual skin texture, so this can 
be a serious problem for texture management.  It wouldn't be unreasonable to 
set this to 2 or even 3 if you are playing competatively (and don't care if 
the other guys have smudged skins).  If you change this during the game, it 
will take effect as soon as a player changes their skin colors.

run time options
----------------
At the console, you can set these values to effect drawing.

gl_texturemode GL_NEAREST
Sets texture mapping to point sampled, which may be faster on some GL systems 
(not on 3dfx).

gl_texturemode GL_LINEAR_MIPMAP
This is the default texture mode.

gl_texturemode GL_LINEAR_MIPMAP_LINEAR
This is the highest quality texture mapping (trilinear), but only very high 
end hardware (intergraph intense 3D / realizm) supports it.  Not that big of 
a deal, actually.

gl_finish 0
This causes the game to not issue a glFinish() call each frame, which may make 
some hardware run faster.  If this is cleared, the 3dfx will back up a number 
of frames and not be very playable.

gl_flashblend 0
By default, glqwcl just draws a shaded ball around objects that are emiting 
light.  Clearing this variable will cause it to properly relight the world 
like normal quake, but it can be a significant speed hit on some systems.

gl_ztrick 0
Glquake uses a buffering method that avoids clearing the Z buffer, but some 
hardware platforms don't like it.  If the status bar and console are flashing 
every other frame, clear this variable.

gl_keeptjunctions 0
If you clear this, glqwcl will remove colinear vertexes when it reloads the 
level.  This can give a few percent speedup, but it can leave a couple stray 
blinking pixels on the screen.

novelty features
----------------
These are some rendering tricks that were easy to do in glqwcl.  They aren't 
very robust, but they are pretty cool to look at.

r_shadows 1
This causes every object to cast a shadow.

r_wateralpha 0.7
This sets the opacity of water textures, so you can see through it in properly 
processed maps.  0.3 is very faint, almost like fog.  1 is completely solid 
(the default).  Unfortunately, the standard quake maps don't contain any 
visibility information for seeing past water surfaces, so you can't just play 
quake with this turned on.  If you just want to see what it looks like, you 
can set "r_novis 1", but that will make things go very slow.  When I get a 
chance, I will probably release some maps that have been processed properly 
for this.

r_mirroralpha 0.3
This changes one particular texture (the stained glass texture in the EASY 
start hall) into a mirror.  The value is the opacity of the mirror surface.