# TODOs

## runs
- [x] ! run the simulation with the fixed diffusion function for q=3, c=3; q=3, c=20; q=2, c=20
- [ ] longer runs for q=2, c=5.4,6 (pick up last checkpoints?) - those should demix!
- [ ] q=3, c=5 periodic boundaries (to make sure those demix - just to see what it looks like)
- [ ] cyclic dependency same parameters - see what happens!

## content
- [x] no need to write bin_bounds when lower and upper bounds are fixed
    - [x] simplify plotting correspondingly: no need to calculate the centers every time
- [x] normalize density: divide by the bin_width
- [x] start in uniform distribution
- [x] checkpoints: save the state (indices + coordinates) after every thousand timesteps
- [x] ! check before running the simulation if data for this config already exists and continue
from the last computed state if so
- [x] time the simulation (write in log?)
- [x] implement symmetric dependency for tripple diffusion
- [x] setup the projext on the on the serveraa
- [x] setup the first long runs on the server for curcular and symmetric dependencies
- [x] write the number of bins in the filename
- [x] write sensing radius (rs) to the filename
- [x] transfer data (or animation) to local machine via ssh?!
- [ ] ! generalize the simulation (executable) and move more input to the config (particle dependencies, 
type of simulation, quadratic/logistic - for the future) Minimize the need to adjust the source code and
recompile
    - [x] type of simulation
    - [ ] particle dependency
    - [ ] quadratic / logistic - maybe?
- [x] ! implement finite sensing radius (non-local case) and setup the run
- [x] ! fix normalization - no fix needed, just rescale y-axis, the densities can be greater than 1
- [x] ! add frame timestep to config and filename
- [x] ! pickup checkpoint (preexisting animation) in visualisation instead of regenerating the entire thing
- [x] ! save data/animations to run folders

- [ ] ! implement different initial densities

## structure
- [x] config file
- [x] automate file naming
- [x] cleanup the structure for slurm (separate folders, proper names for .out and .log files)

## bugs
- [x] what is wrong with the reflective bottom boundary in sticky_top_refl_bottom animation? some particles are reflected to early.
- [x] why is the video file not fully written?
- [x] fix custom config cli argument on the server!

## speedup / storage efficiency
### algorithms and data structures
- [x] rewrite histogram
- [x] calculate histogram on the fly
- [~] cache intermediate results (histograms) and the write to file in chunks --> os already does this for me
- [x] do not close and reopen the file every time but keep an open pointer until throughout the loop
- [x] parallelize
- [ ] postprocess the counts in c - write normalized histograms (except the checkpoints?)
- [ ] use gnuplot to directly generate animations from the data files
- [x] ! do not store all the counts rows in the first place (not used for animations rendering anyway)

### animation
- [x] initial rough check if the results make sense
- [x] rawdog ffmpeg??


## cosmetics
