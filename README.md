
<h1>FileStream Inverter Binary v2.0</h1>
<h3><i>A malicious program targeted to damage & destroy disk files...</i></h3>

<br>
<img align="right" width="500px" height="298px" src="https://github.com/AKD92/FileStream-Binary-Inverter/raw/master/fstmScreen02.png">

<p align="justify">
This is a malicious program intended to do harm, but actually it is not so harmful at all.
</p>

<p align="justify">
Given a path to a directory on local file system (for example, C:\Test Dir) this program will enumerate and generate a list of all files available inside the directory and their sub-directories, and will attempt to alter (change) data of each file one by one. Upon completion the altered files will no longer contain their original data.. Size of the files will remain same as before but inside of those files are pure garbage.
</p>

<p align="justify">
If this program is run over a large amount of files and folders, the end result is a <b>massacre.</b>
</p>

<img align="right" width="500px" height="298px" src="https://github.com/AKD92/FileStream-Binary-Inverter/raw/master/fstmScreen01.png">

<h3>Background History behind this massacre</h3>
<p align="justify">
When I first created this stupid program, I was temptated to silently cheat with some of my friends in the university, destroying their favourite stupid things on their computers. I was in a hurry and I wrote the first version of this program in Java within a short time. It was only capable of damaging, no way to fix those damages at all.
</p>

<p align="justify">
Later I realized that only doing harm will be no fun. I should devise a methodology so that there will be a way to bring back the original contents of those poor files, revert the damage, quickly and painlessly. Therefore I changed the corruption algorithm entirely without losing its strength, and so here is the second version rewritten entirely in C with full revert capability.
</p>

<h3>Download</h3>
<ul>
<li><b>
<a href="https://github.com/AKD92/FileStream-Inverter-Binary-V2-CLI-Win32/raw/master/bin/fstm_inv_bnry.exe">
Standalone Executable (Win32)</a>
<br>
Compiled using <a href="http://www.mingw.org/">MinGW GCC 4.8.1</a>
</b></li>
<li><b>
<a href="src">Browse for Source Codes</a> on GitHub
</b></li>
</ul>

<h3>License</h3>
<a rel="license" href="http://www.gnu.org/licenses/gpl-3.0-standalone.html"><img alt="GPLv3 License" style="border-width:0" src="http://www.gnu.org/graphics/gplv3-127x51.png" /></a><br />This software is licensed under <a rel="license" href="http://www.gnu.org/licenses/gpl-3.0-standalone.html">GNU General Public License, Version 3</a>.
