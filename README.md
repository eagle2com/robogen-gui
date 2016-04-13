# robogen-gui (WIP)
QT gui for interfacing with the robogen framework: www.robogen.org

## Building
- QT has to be version 5.2 or higher
- Open .pro in qtcreator, build, run

## Usage
**Read the last point before doing anything**
- Make sure you have downloaded and compiled robogen from www.robogen.org 
- Launch robogen-gui
- Settings tab: set the path to the robogen build folder
- Hit the big save button on the bottom and select where you want robogen-gui to save the robot/simulation/evolution config files
- Evolve/Simulate tab: set the output directory to where you want the evolve output files to go to (GenerationBest-#.json, ....), the folder will be overwritten every time (todo: maybe add a checkbox for that? )
- Evolve Simulate tab: hit Evolve!
- If everything is setup correctly, GenerationBest-#.json entries should appear in the list on the right.
- Click on any of them and hit Simulate to preview the best robot of the selected generation
- After the evolution finished, you can go to the analyze tab and hit Analyze to plot the graph of the generation stats.
- The evolution can be easily parallelized up to the number of logical cores of your CPU - 1 (to not freeze your PC) in the evolve tab.
- (Output from the servers is piped to stdout)
- The robot limbs can be fully edited in Robot tab. Just make sure the configuration is valid, as there are no checks in place yet. (Don't put two parts on the same face for example). The parts can be drag-n-dropped to the appropriate parent. You can also load a robot configuration with load->robot. Please don't load `.txt` with brains in them. `.json` are ok
- You can do all of this with a .json config file that you can load from file->load config. Have a look at sample-config.json for the contents. If the filename is _config.json_ and it is in the launch directory of robogen-gui, it will load it automaticaly at startup . All that is left to do is hit the big save once, and then hit evolve. You have to save everytime you make a modification to the robot/evolution/simulation configuration.

## Issues
- If Evolve stops before reaching the max generations, it is possible that mutations created an unusable configuration (robogen issue)
- It is also possible that the output analysis failed for some reason

## TODO
- Brains
- Add the missing parameters to the configuration
