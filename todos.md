# TODOs

## runs
- [x] ! run the simulation with the fixed diffusion function for q=3, c=3; q=3, c=20; q=2, c=20
- [x] longer runs for q=2, c=5.4,6 (pick up last checkpoints?) - those should demix!
- [x] q=3, c=5 per boundaries (to make sure those demix - just to see what it looks like)
- [x] cy dependency same parameters - see what happens!
- [x] sym q2 c3-9 dt0.00001 nr100000

- [ ] sym q2 c3-12 dt0.0001 nr10000 +
- [ ] sym q2 c6 dt0.0001 nr1000-1000000 +
- [ ] sym q2 c6 dt 1e-8 - 0.1 nr10000 +
- [ ] sym q2 c7 dt 1e-8 - 0.1 nr10000 +

## content
- [x] no need to write bin_bounds when lower and upper bounds are fixed
    - [x] simplify plotting correspondingly: no need to calculate the centers every time
- [x] normalize density: divide by the bin_width
- [x] start in uniform distribution
- [x] checkpoints: save the state (indices + crds) after every thousand timesteps
- [x] check before running the simulation if data for this config already exists and continue
from the last computed state if so
- [x] time the simulation (write in log?)
- [x] implement sym dependency for tripple diffusion
- [x] setup the projext on the on the serveraa
- [x] setup the first long runs on the server for curcular and sym dependencies
- [x] write the number of bins in the filename
- [x] write sensing radius (rs) to the filename
- [x] transfer data (or animation) to local machine via ssh?!
- [x] generalize the simulation (executable) and move more input to the config (particle dependencies, 
type of simulation, quadratic/log - for the future) Minimize the need to adjust the source code and
recompile
    - [x] type of simulation
    - [x] particle dependency
    - [x] quadratic / log - maybe?
- [x] implement finite sensing radius (non-local case) and setup the run
- [x] fix normalization - no fix needed, just rescale y-axis, the densities can be greater than 1
- [x] add frame timestep to config and filename
- [x] pickup checkpoint (preexisting animation) in visualisation instead of regenerating the entire thing
- [x] save data/animations to run folders

- [x] implement different initial densities

## structure
- [x] config file
- [x] automate file naming
- [x] cleanup the structure for slurm (separate folders, proper names for .out and .log files)

## bugs
- [x] what is wrong with the reflective bottom boundary in sticky_top_refl_bottom animation? some particles are reflected to early.
- [x] why is the video file not fully written?
- [x] fix custom config cli argument on the server!
- [x] fix sensing radius asymmetry

## speedup / storage efficiency
### algorithms and data structures
- [x] rewrite histogram
- [x] calculate histogram on the fly
- [~] cache intermediate results (histograms) and the write to file in chunks --> os already does this for me, no microoptimization needed
- [x] do not close and reopen the file every time but keep an open pointer until throughout the loop
- [x] parallelize
- [?] postprocess the cnts in c - write normalized histograms (except the checkpoints?)
- [?] use gnuplot to directly generate animations from the data files
- [x] do not store all the cnts rows in the first place (not used for animations rendering anyway)

### animation
- [x] initial rough check if the results make sense
- [x] rawdog ffmpeg??


## cosmetics



---
id: bachelor_thesis
aliases:
  - bachelor_thesis
  - Bachelorarbeit
tags: []
---

# bachelor_thesis

## Time till next meetup with Lindner

* meetup with Lindnder on              3.09
* free days with today and weekends:   13
* with 2 days off, without today:       10

## TODOs

### Today
- [x] planning
- [x] figure out how much data to save
- [x] preferably implement checkpoint pickup 
- [x] set up a long run with a parameter sweep (how many parameters??). let slurm distribute the work smartly
    - [x] q=2, c_crit = 16/3
    - [x] q=3, c_crit = 2

### Simulation

- [x] Exkurs: *0.5*
    - [x] implement periodic boundaries & a short run
    - [x] implement and sigmoid dependency & a short run
- [x] research/consider tooling: *0.25*
    - [x] python & Co. (Numba??)
    - [x] GPU parallelization (OpenACC??)
- [x] implement different initial conditions (adding a perturbation, starting with a separated densities) *0.25*
- [x] structural and general cleanup *0.5*
- [ ] 

### Analysis

- [!] numerical solution of full partial differential equations (without linear approximation). -> Python libraries for stochastic differential equations?? *1*
- [x] stability analysis for three particles to the end
    - [x] understand bistability analysis (Cahn Hilliard formalism, define potential??) and calculate the separated densities and confirm with the simulation: *2*
- [x] understand the sensing radius analysis and apply to my calculations: *2*
- [x] write up the most important calculations in latex *1*
- [x] read and understand the first paper fully *2*
- [!] read the second paper (just to get a superficial understanding) *1*
- [ ] 

### Achieved Extras
- [x] fully automated project pipeline (simulation and slurm configs generation in pyhon -> folder structure creation in C -> visualization etc. in Python using the project folder structure)

### Days Plan:


0. (today) 
- [x] planning
- [x] research/consider tooling: *0.25*
    - [x] python & Co. (Numba??) - NO!
    - [x] GPU parallelization (OpenACC??) - NO!
- [x] figure out how much data to save
- [x] preferably implement checkpoint pickup  *did not manage, left for later, since correct checkpoint saving is implemented*
- [x] set up a long run with a parameter sweep (how many parameters??). let slurm distribute the work smartly *finished on the second day, buffer gone*
    - [x] q=2, c_crit = 16/3
    - [x] q=3, c_crit = 2

1.
- [x] read and understand the first paper fully *2* *finished in one day on the second day (+extract specific TODOs for particle simulation, analysis and numerics! buffer is so back!)*
2.
- [x] stability analysis for three particles to the end
    - [x] understand bistability analysis (Cahn Hilliard formalism, define potential??) and calculate the separated densities and confirm with the simulation: *2*
3.
- [x] read and understand the first paper fully *2* 
4.
- [x] stability analysis for three particles to the end
    - [x] understand bistability analysis (Cahn Hilliard formalism, define potential??) and calculate the separated densities and confirm with the simulation: *2*

5.
- [!] numerical solution of full partial differential equations (without linear approximation). -> Python libraries for stochastic differential equations?? *1*

5. 
- [x] understand the sensing radius analysis and apply to my calculations: *2*

6. 
- [x] understand the sensing radius analysis and apply to my calculations: *2*

7.
- [x] Exkurs: *0.5* *all this on days 3 and 4*
    - [x] implement periodic boundaries & a short run
    - [x] implement and sigmoid dependency & a short run
- [x] implement different initial conditions (adding a perturbation, starting with a separated densities) *0.25*
- [x] structural and general cleanup *0.5*

8.
- [!] read the second paper (just to get a superficial understanding) *1*

9.
- [x] write up the most important calculations in latex *1*

10.
- [x] **BUFFER**




#######################################################################




## Sprint 2
- next meeting with Lindner: 15.10.2026
- total days till then: 35


### Analysis
- [ ] stability analysis of the demixed state
    - [ ] normal with p_h and p_l - sheeesh that is more difficult than expected, but gotta try
          another ansatz.
    - [ ] tripple with p_h, p_l and p_m (p_m assumed by all species simultaneously)
- [ ] done calculations for other periodic boundaries and cyclic dependency - for now not so relevant

### Simulation Features

### Bugs
- [ ] init uniform first bin almost empty - for now not so relevant

### Simulation Runs
- [x] figure out large N_realizations and small delta_t so that the visuals are well visible
- [x] q=2, c=6, dt=10-8, N_r=10+6, init-demixed, r_s = 0 - did not stabilize in something reasonable :(
- [x] q=2, c=6, dt=10-8, N_r=10+6, init-uniform, r_s = 0 - did not leave the homohegeneous state ;(
- [ ] degree of (in)stability (the rest of params for proper visuals):
    - [x] q=2, c=3, dt=10-6
    - [x] q=2, c=4, dt=10-6
    - [x] q=2, c=5, dt=10-6
    - [x] q=2, c=5.2, dt=10-6
    - [x] q=2, c=5.3, dt=10-6
    - [x] q=2, c=5.33, dt=10-6
    - [x] q=2, c=5.5., dt=10-6
    - [x] q=2, c=5.6, dt=10-6
    - [x] q=2, c=6, dt=10-6
    - [x] q=2, c=7, dt=10-6
    - [x] q=2, c=9, dt=10-6
            -> did not demix from the homogeneous satate in such fine time resolution :(
            -> from demixed state - stayed in a funcky 3-denisities (transient? metastable?) state
               - no idea what that is.
- [o] dt-sweep - maybe really need it now, cuz seems to have crazy influence -> formalized in the meeting, prepare for the presentation
- [x] other boundaries conditons -> nothing super interesting so far, for cyclic + periodic - strange
      "skewed uniform" state.

### Visualization
- [x] plot theoretical predictions
- [ ] compute rolling average of the densities to compare the speed of decrease/increase of fluctuations
- [ ] other plots from the paper (heat map etc.) - identify!
    - [x] init vs demixed with theory (fig 1)
    - [ ] heat map + slices (fig 2)
    - [ ] sharpness of the interface depending on dt (fig 3)

### Numerics

### LaTeX

### Structure
- [ ] write nr in scientific notation
- [~] point the plotting script to a run folder to rerun visualization (need to copy over the config) - unnecessary
- [x] do not build for every run on the server
- [x] remove progress bar vor visualization - brought back, just don't visualize on the server
- [x] pickup checkpoints independently of N_t
- [~] append log for new iterations instead of overwriting - unnecessary
- [x] delete tmp_frames after rendering?



## Presentation on 29.10.26
### runs
- [x] sym q2 c3-9 dt0.00001 nr100000
- [ ] sym q2 c3-12 dt0.0001 nr10000
- [ ] sym q2 c6 dt0.0001 nr1000-1000000
- [ ] sym q2 c6 dt 1e-8 - 0.1
- [ ] sym q2 c7 dt 1e-8 - 0.1
- [ ] 

## data analysis and visualisation

- [x] order parameter
    - [ ] add the perfect order line!
- [x] length (thikness) of the (three) demixed levels
    - [x] find a way to identify the x-position of the interfaces!
- [ ] (asymptotic) rolling time average and define some criteria for convergence
       - [x] for now just take the last rows average
       - [ ] ideally: calculate a rolling delta over multiple (averaged) rows (how many)
             and check when it's not changing much anymore
    
- [ ] put all together
    - [x] need to figure out which counts are for the middle interface, it's not
          always A species -> density maximum between -0.5 and 0.5
    - [x] plot averaged densities
    - [x] normierung
    - [ ] cosmetics: axes, legend, title, ticks and labels etc.

