Full robot algorithm for the course 5XIB0 Venus Exploration

Documentation
===

The circumference of the circle of the wheels is 36.128cm
When each wheel has given one encoder pulse, that means the rotated angle is
(1.316 / 36.128) * 360 = 13.11326014595066683023762986702


Ideas
===

- Make syncing of wheels by checking delays between pulses, getting the longest delay and slowing the speed of the other servo to that value

- INSTEAD of braking on pulse, we can do that if the (leftPulses + rightPulses / 2) is lower than DRIVE_BRAKE_PULSES, then we adjust speed in loop() for both wheels at the same time