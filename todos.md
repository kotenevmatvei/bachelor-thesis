# TODOs

## content
- [x] no need to write bin_bounds when lower and upper bounds are fixed
    - [x] simplify plotting correspondingly: no need to calculate the centers every time
- [x] normalize density: divide by the bin_width
- [x] start in uniform distribution
- [x] checkpoints: save the state (indices + coordinates) after every thousand timesteps
- [ ] check before running the simulation if data for this config already exists and continue
from the last computed state if so
- [x] time the simulation (write in log?)
- [ ] time the visualization (write in log?)
- [x] implement symmetric dependency for tripple diffusion
- [ ] setup the projext on the on the server
- [ ] setup the first long runs on the server for curcular and symmetric dependencies
- [ ] write the number of bins in the filename
- [x] transfer data (or animation) to local machine via ssh?!

## structure
- [x] config file
- [x] automate file naming

## bugs
- [x] what is wrong with the reflective bottom boundary in sticky_top_refl_bottom animation? some particles are reflected to early.
- [x] why is the video file not fully written?
- [ ] fix custom config cli argument on the server!

## speedup
### algorithms and data structures
- [x] rewrite histogram
- [x] calculate histogram on the fly
- [~] cache intermediate results (histograms) and the write to file in chunks --> os already does this for me
- [x] do not close and reopen the file every time but keep an open pointer until throughout the loop
- [x] parallelize
- [ ] postprocess the counts in c - write normalized histograms (except the checkpoints?)
- [ ] use gnuplot to directly generate animations from the data files

### animation
- [x] initial rough check if the results make sense
- [x] rawdog ffmpeg??


## cosmetics
