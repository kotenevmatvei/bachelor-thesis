# TODOs

## content
- [x] no need to write bin_bounds when lower and upper bounds are fixed
    - [x] simplify plotting correspondingly: no need to calculate the centers every time
- [x] normalize density: divide by the bin_width
- [x] start in uniform distribution

## structure
- [ ] config file
- [ ] automate file naming

## bugs
- [x] what is wrong with the reflective bottom boundary in sticky_top_refl_bottom animation? some particles are reflected to early.
- [x] why is the video file not fully written?

## speedup
### algorithms and data structures
- [x] rewrite histogram
- [x] calculate histogram on the fly
- [~] cache intermediate results (histograms) and the write to file in chunks --> os already does this for me
- [x] do not close and reopen the file every time but keep an open pointer until throughout the loop
- [ ] parallelize

### animation
- [x] initial rough check if the results make sense
- [x] rawdog ffmpeg??


## cosmetics
