Full robot algorithm for the course 5XIB0 Venus Exploration

Documentation
===

The circumference of the circle of the wheels is 36.128cm
When each wheel has given one encoder pulse, that means the rotated angle is
(1.316 / 36.128) * 360 = 13.11326014595066683023762986702

To-Do
===
- Interrupts on the encoders instead (ask Erik)
- Positional tracking
	+ Start at x, y = 0, 0 with 0 degrees
	+ On every turn we need to take the degrees into account
	+ Whenever we're not standing at 90 degrees, which will be most of the time, we need to apply trigonometry to get the approximate x and y change
	+ Things taken into account: abrupt stopping will make the robot jump
- Braking
	+ INSTEAD of braking on pulse, we can do that if the (leftPulses + rightPulses / 2) is lower than DRIVE_BRAKE_PULSES, then we adjust speed in loop() for both wheels at the same time
	
Ideas
===
- Make syncing of wheels by checking delays between pulses, getting the longest delay and slowing the speed of the other servo to that value