# Bad Apple!! but it's ImGui (Windows)

![image.png](image.png)

![image.png](image%201.png)

![image.png](image%202.png)

---

### References

[Bad Apple but it&#39;s a Windows virus - YouTube](https://www.youtube.com/watch?v=EZpZwunMzuE&t=35s)

### Special Thanks

ImGui / ocornut : [GitHub - ocornut/imgui: Dear ImGui: Bloat-free Graphical User interface for C++ with minimal dependencies](https://github.com/ocornut/imgui)

BadApple!! PV【影絵】/ あにら氏 : https://www.nicovideo.jp/watch/sm8628149

---

[日本語版](README.md) [English Ver](README-en.md)

---

# Overview

This project is a technical demo that recreates the silhouette animation of Bad Apple!! on ImGui using quadtree space partitioning.  
Rather than just recreating it, the project focuses on efficient processing by implementing video batch processing (streaming processing) and reducing memory usage to keep processing load to a minimum.  

---

# Installation/Usage

Download the latest zip from Releases and extract it.
First, download the Bad Apple!! video and audio files to the Assets folder.
(If the files are missing, an error will occur and the application cannot be executed)  
Please refer to Assets/README.txt for details.  

Press the play button in the Player window (bottom of the image) to play the processed video.  
You can change the display method by switching the Mode in Animation settings.

## System Requirements

- Windows 10 or later

- DirectX11 compatible environment
  (NVIDIA GTX 400 series or later, AMD HD 5000 series or later)

- [Optional] Windows Aero support available

---

## Button/Settings Description

### Settings/Animation settings

- **Mode**
  
    Changes the display method. You can choose from three options: display on ImPlot graph, rectangular display, or ImGui window display.  
    Mode settings allow you to configure settings for each mode.

- **Show trees**
  
    Color-codes each rectangle during display.

- **Show video**
  
    Displays the original video in a separate window.

- **Transparent Window**
  
    Makes the display screen transparent.  
    However, this cannot be used unless Windows Aero is enabled.  
    Also, transparency will not work if the display screen is detached from the main window.

---

### Settings/Video processing settings

※ Please be careful as changing these settings may put significant load on your PC.

- **Quadtree Depth**
  
    Sets the maximum division level for quadtree space partitioning.

- **Frame/Segment**
  
    Settings related to streaming processing.  
    You can set how many frames per segment.

- **Keep load segment**
  
    Settings related to streaming processing.  
    You can set the number of segments to keep loaded at all times.  
    The total number of frames kept loaded is Keep load segment * Frame/Segment.

---

### Player

![image.png](image%203.png)

- **Slider**
  
    Current playback position. You can move it, but if you specify a position outside the loaded segment, it will load synchronously, causing the application to freeze temporarily.

- **Load Segment**
  
    Refers to the gray, green, and cyan bar sections.  
    You can understand the current loaded segments and their status.
  
  | Color    | Description                                                  |
  | -------- | ------------------------------------------------------------ |
  | Green    | Currently loaded segment                                     |
  | Cyan     | Currently loaded segment + current playback position segment |
  | Gray     | Unloaded segment                                             |
  | Red line | Current playback position                                    |

---

# Build Instructions

This application uses the following libraries.  
To build, you need to install the following libraries in the specified locations.  
(Due to licensing, they are not included in this project by default.)

- FMOD Core API
  
    Download FMOD Studio (2.03.08) from the following URL and install as instructed.  
    [https://www.fmod.com/download](https://www.fmod.com/download)
  
    Once installation is complete, run Tools/Import_FMOD.ps1 and select the FMOD installation location from the dialog.  
    It will be automatically imported into the project.

- OpenCV
  
    Download OpenCV 4.12.0 from the following URL, run it, and extract to an appropriate location.  
    (opencv-4.12-windows.exe)  
    [https://github.com/opencv/opencv/releases/tag/4.12.0](https://github.com/opencv/opencv/releases/tag/4.12.0)
  
    Once installation is complete, run Tools/Import_OpenCV.ps1 and select the OpenCV installation location from the dialog.  
    It will be automatically imported into the project.

After all installations are complete, download the video and audio to Assets, open the solution, and run.

---

# Development Environment

- OS: Windows 11
- Language: C++23(MSVC)
- Libraries/Engine: DirectX11, stb_image, OpenCV, FMOD, ImGui, ImPlot

# Disclaimer

In no event shall the developer be liable for any direct, indirect, incidental, or consequential damages, including but not limited to any damages arising from or related to the use of this application.

Made using FMOD Studio by Firelight Technologies Pty Ltd.  
Copyright © 2025 himawarialadin All rights reserved.  

If you have any issues, please contact us through Issues.

(This is my first public GitHub repository, so please be gentle)