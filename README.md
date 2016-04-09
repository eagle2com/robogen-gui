# robogen-gui (WIP)
QT gui for interfacing with the robogen framework: www.robogen.org

## Building
- QT has to be version 5.2 or higher
- Open .pro in qtcreator, build, run

## Usage
- Make sure you have downloaded and compiled robogen from www.robogen.org
- Launch robogen-gui
- Settings tab: set the path to the robogen build folder
- Evolution Config tab: change evolution mode to full (the default robot is only one CoreComponent and nothing would happen in brain-only evolution)
- Hit the big save button on the bottom and select where you want robogen-gui to save the robot/simulation/evolution config files
- Evolve/Simulate tab: set the output directory to where you want the evolve output files to go to (GenerationBest-#.json, ....)
- Evolve Simulate tab: hit Evolve!
- If everything is setup correctly, GenerationBest-#.json lentries should appear in the list on the right.
- Click on any of them and hit Simulate to preview the best robot of the selected generation
- After the evolution finished, you can go to the analyze tab and hit Analyze to plot the graph of the generation stats.

## Issues
- If Evolve stops before reaching the max generations, it is possible that mutations created an unusable configuration (robogen issue)
- It is also possible that the output analysis failed for some reason

## TODO
- Add a default config file so we don't have to set directories each time (project-like?)
