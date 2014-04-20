

# https://code.google.com/p/libgdx/wiki/DistanceFieldFonts#Generating_the_font


hiero:
	java -cp gdx.jar:gdx-natives.jar:gdx-backend-lwjgl.jar:gdx-backend-lwjgl-natives.jar:extensions/gdx-tools/gdx-tools.jar com.badlogic.gdx.tools.hiero.Hiero

image:
	java -cp extensions/gdx-tools.jar:gdx.jar com.badlogic.gdx.tools.distancefield.DistanceFieldGenerator

conv: out=$(png)_g8.png
conv:
	gm convert $(png)  -operator Matte negate "0" -channel Matte $(out)
