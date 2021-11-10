# FEAR-Code
- This project contains the the source code of the fear-16 current regulation circuit.
- It shouldn't be necessary to add your C-Files or include-paths to your makefile. There is a semi-automatic scandepth implemented which can scan subfolders down to 15 subfolders within the Sourcecode-Folder. When there are special cases where files has to be added then use the **C_SOURCES**, **C_INCLUDES**, **ASM_SOURCES** and **ASM_INCLUDES** variables within the *makefile*.
- The FEAR-16 project also consist of <not implemented yet>.

***Note:***
The project was created on Windows for a TM4C123GH6PM (TIVA-Launchpad) and needs some tools.

## Setup necessary toolchain
Follow the installation guidance from the GRIPS-Course [MC - Übungen zu Mikrocontroller][MC - Übungen zu Mikrocomputertechnik].


File written by HAUM &copy; 2021

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)
  [MC - Übungen zu Mikrocomputertechnik]: https://elearning.uni-regensburg.de/course/view.php?id=42784
