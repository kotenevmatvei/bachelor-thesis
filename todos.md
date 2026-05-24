# TODOs

## bugs
- [ ] what is wrong with the reflective bottom boundary in sticky_top_refl_bottom animation? some particles are reflected to early.
- [ ] why is the video file not fully written?

## speedup
### algorithms and data structures
- [x] rewrite histogram
- [x] calculate histogram on the fly

### animation
- [x] initial rough check if the results make sense
- [x] rawdog ffmpeg??

### IO
- [ ] cache intermediate results (histograms) and the write to file in chunks
- [ ] do not close and reopen the file every time but keep an open pointer until throughout the loop

## cosmetics
- [ ] bigger font on the plots
- [ ] current timestep in the animation
