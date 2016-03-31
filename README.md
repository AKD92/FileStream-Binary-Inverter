# FileStream Inverter Binary v2.0
<b><i>A Command Line Interface program, based on Windows 32bit platform</i></b>

This is seriously a funny utility program for corrupting data stored on local files. Given a path of a directory in your file system (for example, C:\Test Folder) this program will grab a list of all files available inside the directory and their sub-directories, and will attempt to change(corrupt) the data of those files, one by one. After completion, the original data of those poor files will be lost forever. Size of the files will remain same but there will be left nothing but garbages.

Pure massacre though, surely those files suddenly become unreadable and useless.

### Background History behind this massacre
When I first created this stupid program, I was temptated to silently cheat with some of my friends in the university, destroying their favourite stupid things on their computers. I was in a hurry and I wrote the first version of this program in Java within a short time. It was only capable of polluting, no way to fix those at all. And yes I made my destructions :D

Later I realized that only doing harm will be no fun. I should devise a methodology so that there will be a way to bring back the original contents of those poor files, revert the damage, quickly and painlessly. Therefore I changed the corruption algorithm entirely without losing its strength, and so here is the second version rewritten entirely in C with full revert capability.

## Screenshots

<img align="right" width="500px" height="298" src="https://github.com/AKD92/FileStream-Binary-Inverter/raw/master/fstmScreen01.png">
<br>1) The application was started on a specified directory</br>


<p>
<img align="right" width="500px" height="298" src="https://github.com/AKD92/FileStream-Binary-Inverter/raw/master/fstmScreen01.png">
<br>2) After the completion of inversion procedure</br>
</p>
