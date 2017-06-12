/**
\mainpage IOT Project
<<<<<<< HEAD
<H2>Introduction</H2>
----- Introdução -----
=======
>>>>>>> 31a91c53a67ceb420f1757df1ae916382b744a7b
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
<<<<<<< HEAD
Entering on the directory ./src/APP
There are two alternatives
<H3>Debug</H3>
make debug - will compile to a debug version 
<H3>Release</H3>
make release - will compile to a release version, usually on the end of the project
*/
=======
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
>>>>>>> 31a91c53a67ceb420f1757df1ae916382b744a7b
