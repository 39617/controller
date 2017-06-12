/**
\mainpage IOT Project
<BR>
<BR>
<H2>Hardware</H2>
<UL>
<LI>CC1310 - LaunchPad</LI>
</UL>
<BR>
\image html project_image.JPG
<BR>
<H2>Compiling</H2>
On the main directory:
make TARGET=srf06-cc26xx BOARD=launchpad/cc1310 - will compile to a release version, usually on the end of the project
<BR>
<BR>
For debugging we need to enable the debug flags:
<BR>
#define DEBUG 0
<BR>
#define DEBUG 1
<BR>
In every page we want to debug
<BR>
<BR>
*/