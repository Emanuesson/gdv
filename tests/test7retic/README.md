Test7 - reallocate axes
-----------------------

One of the main problems of the Gdv-library is the automatic generation and deleting of widgets, such as Tics and Mtics. This process is usually connected to calculating the need for space by all widgets and the given properties of the axis. This process is solved in Gdv by using the start values, allocate the tics and calculate the available space. Than the process is iterated until most of the necessary properties are satisfied. During this process all widgets, such as tics, ticlabels and the axis-bar itself, are invisible to prevent any clipping problems.

The following test is used to solve this problem as the process of continously redrawing of all axis-features is extremely annoying. Another issue, that is addressed by this test-application is the problem of different tic-intervals after resetting.
