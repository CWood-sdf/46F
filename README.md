# 46F
This project is the main program for 46F

It contains many parts

<h1>Wheelbase controls</h1>
<h2>Pure Pursuit</h2>
Use pure pursuit to control the robot along curve paths
<h2>Ramsete</h2>
Also use ramsete to move the robot along curves
<h2>PID</h2>
Use a basic PID loop to move the robot in straight lines
<h2>Bezier path generation</h2>
Bezier curves are used to produce smooth paths, then target speeds are generated to allow the robot to optimally follow the path
<h1>Sensors</h1>
<h2>Line Tracker</h2>
Uses line tracker for detection of close objects
<h2>Distance sensor</h2>
Kalmanized distance sensor
<h2>Potentiometer</h2>
Integrated potentiometer auton selection
<h1>Brain Screen</h1>
Has windows loader, data output, and auton selector. Any function can be plugged in, including those written with lvgl or regular Brain.Screen.draw[Name] controls
<h1>Future</h1>
<h2>Vision</h2>
EMA Filtering of vision objects. 
Using vision sensor to determine an objects position relative to the vision sensor, then use odometry data to convert that to a global position. 
Using vision sensor looking at the goals to determine the robot's position (this would probably just be a tool to confirm the normal odometry position). 
