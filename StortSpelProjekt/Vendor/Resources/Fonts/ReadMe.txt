Hiero is a tool to generate new fonts in fnt format.
Download: http://www.mediafire.com/file/hlwbhemfgog51tu/hiero.jar/file

The result of this will be 2 files, one containing information about the font and characters, and one containing the font image.

First select the font you want in the top left combo box (it is also possible to choose a custom .ttf file).
Below the combo box, select the "Java" radio button for "Rendering". This will enable the "Effects" combo box on the top right. 
For this tutorial, we will not be using any effects, but you may want to implement distance field once you have your text rendering.

Next select the "Glyph Cache" radio button above the font preview box at the bottom. By default "Sample Text" is selected. 
This will give us more options for the output image. You will see "Page width" and "Page height" to the right of the font preview box,
under where you have selected "Glyph Cache". We will be making a 512x512 font image, so set these both to 512.

Now back up to the top left, right above where you set the rendering to "Java", you will see a size input.
What you want to do is increment the size to just before "Pages" is 2 
(you will see "Pages" above where you set the width and height of the output image). 
This will increase the size of the font as large as can possibly fit on one page (all the characters fit in the 512x512 image). 
With the Arial font i was able to increase the size to 73 before some characters had to be moved to the next page, 
where you would have two 512x512 font images instead of just one.

On the bottom right is 4 input boxes for padding, for left, top, right and bottom.
We want to give each character a little padding in the final output image so we have less a chance of sampling surrounding
characters in the shaders when sampling the texture for a character. When the quad on the screen that we are drawing a character
is too small, sampling from the texture is less accurate and may end up getting values from the next character.
Let's set all these to 5 pixels for padding.

That should be it for setting up. Let's get our bitmap font files now. Click on "File" in the top menu, 
then click "Save BMFont files (text)...". Choose a directory and name for the output files. I named it Arial.fnt. 
When you click save, it will save a .fnt file containing information about rendering the font, and .png containing the font image.

Having troubles running the program?
Make sure that you have the latest Java Runtime installed.