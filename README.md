# sms-tools

sms-tools has several small tools made for 120 Shines TAS:

- Spin: Visualize Mario's directions and stick directions like a compass.
- Object Viewer: Display objects' coordinates and other values.
- Map Viewer: Show coins and other items on 2D Map. Currently available only for Pinna's Beach.
- Fluff Manipulator: Search for RNGs that generate fluffs in the specified area. Show current fluffs (and other items) on 2D Map.

Currently available only on JP1.0.

You can download binary from the [releases page](https://github.com/Goldfire711/sms-tools/releases) on this repository.

## How to build (Windows)

Requirement: 

- Visual Studio 2019
- Windows SDK 10.0.18362.0 (Install it with Visual Studio Installer)
- [Qt 5.15.2](https://www.qt.io/download-qt-installer) (MSVC 2019 64-bit)
- Qt Visual Studio Tools (Extension for Visual Studio)

You need to create Qt Account to install Qt.

After installing all of the above, you need to set up Qt Visual Studio Tools.

Open Visual Studio 2019 and click `Extensions->Qt VS Tools->Qt Versions`, then click `<add new Qt version>` and set the Path to qmake.exe 
(`C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe` by default).

Once complete, open `sms-tools.sln` with Visual Studio. Select Debug or Release and build it.
